#include "village_generator.h"
#include <cmath>

/**
 * Terraform the land around plots using a linear interpolation function
 * Formula: block_height(d, yg, yp, p) = round(yg + (yp - yg) * (p - d) / p)
 * where d is distance from plot edge, yg is ground height, yp is plot height, p is plot_border
 */
void VillageGenerator::terraformPlots(const std::vector<Plot>& plots) {
    for (const auto& plot : plots) {
        int plot_height = plot.height;
        
        // Terraform the border area around each plot
        int border_min_x = plot.origin.x - plot_border;
        int border_max_x = plot.bound.x + plot_border;
        int border_min_z = plot.origin.z - plot_border;
        int border_max_z = plot.bound.z + plot_border;
        
        for (int x = border_min_x; x <= border_max_x; x++) {
            for (int z = border_min_z; z <= border_max_z; z++) {
                // Skip if inside the plot itself
                if (x >= plot.origin.x && x <= plot.bound.x &&
                    z >= plot.origin.z && z <= plot.bound.z) {
                    continue;
                }
                
                // Calculate distance to nearest plot edge
                int dist_x = 0;
                if (x < plot.origin.x) {
                    dist_x = plot.origin.x - x;
                } else if (x > plot.bound.x) {
                    dist_x = x - plot.bound.x;
                }
                
                int dist_z = 0;
                if (z < plot.origin.z) {
                    dist_z = plot.origin.z - z;
                } else if (z > plot.bound.z) {
                    dist_z = z - plot.bound.z;
                }
                
                int distance = std::max(dist_x, dist_z);
                
                if (distance > 0 && distance <= plot_border) {
                    // Get current ground height
                    mcpp::Coordinate highest = mcpp::Coordinate(x, 255, z);
                    for (int y = 255; y >= 0; y--) {
                        mcpp::Block block = mcpp::getBlock(mcpp::Coordinate(x, y, z));
                        if (block.id != 0) {
                            highest.y = y;
                            break;
                        }
                    }
                    
                    int ground_height = highest.y;
                    
                    // Linear interpolation: closer to plot = more influence from plot height
                    double factor = (double)(plot_border - distance) / plot_border;
                    int target_height = (int)std::round(ground_height + 
                                        (plot_height - ground_height) * factor);
                    
                    // Modify terrain to target height
                    if (target_height > ground_height) {
                        // Fill up
                        for (int y = ground_height + 1; y <= target_height; y++) {
                            mcpp::setBlock(mcpp::Coordinate(x, y, z), mcpp::Block(3)); // Dirt
                        }
                    } else if (target_height < ground_height) {
                        // Remove blocks
                        for (int y = ground_height; y > target_height; y--) {
                            mcpp::setBlock(mcpp::Coordinate(x, y, z), mcpp::Block(0)); // Air
                        }
                    }
                }
            }
        }
        
        // Flatten the plot itself
        for (int x = plot.origin.x; x <= plot.bound.x; x++) {
            for (int z = plot.origin.z; z <= plot.bound.z; z++) {
                // Remove everything above plot height
                for (int y = plot_height + 1; y <= 255; y++) {
                    mcpp::setBlock(mcpp::Coordinate(x, y, z), mcpp::Block(0)); // Air
                }
                
                // Fill up to plot height if needed
                mcpp::Coordinate highest = mcpp::Coordinate(x, 0, z);
                for (int y = 255; y >= 0; y--) {
                    mcpp::Block block = mcpp::getBlock(mcpp::Coordinate(x, y, z));
                    if (block.id != 0) {
                        highest.y = y;
                        break;
                    }
                }
                
                if (highest.y < plot_height) {
                    for (int y = highest.y + 1; y <= plot_height; y++) {
                        mcpp::setBlock(mcpp::Coordinate(x, y, z), mcpp::Block(3)); // Dirt
                    }
                }
            }
        }
    }
}

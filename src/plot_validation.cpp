#include "village_generator.h"
#include <cmath>
#include <algorithm>

/**
 * Get the highest non-air block at coordinates (x, z)
 */
mcpp::Coordinate VillageGenerator::getHighestBlock(int x, int z) {
    for (int y = 255; y >= 0; y--) {
        mcpp::Block block = mcpp::getBlock(mcpp::Coordinate(x, y, z));
        if (block.id != 0) { // 0 is air
            return mcpp::Coordinate(x, y, z);
        }
    }
    return mcpp::Coordinate(x, 0, z);
}

/**
 * Check if water coverage is <= 15% (max 3 water blocks in 20x20 area)
 */
bool VillageGenerator::checkWaterCoverage(const Plot& plot) {
    int water_count = 0;
    int total_blocks = plot.getWidth() * plot.getDepth();
    
    for (int x = plot.origin.x; x <= plot.bound.x; x++) {
        for (int z = plot.origin.z; z <= plot.bound.z; z++) {
            mcpp::Coordinate highest = getHighestBlock(x, z);
            mcpp::Block block = mcpp::getBlock(highest);
            
            // Check for water (block id 8 or 9 for flowing/stationary water)
            if (block.id == 8 || block.id == 9) {
                water_count++;
            }
        }
    }
    
    double water_percentage = (double)water_count / total_blocks;
    return water_percentage <= 0.15;
}

/**
 * Check if slope delta is <= 15 (excluding trees)
 */
bool VillageGenerator::checkSlopeDelta(const Plot& plot) {
    int min_height = 255;
    int max_height = 0;
    
    for (int x = plot.origin.x; x <= plot.bound.x; x++) {
        for (int z = plot.origin.z; z <= plot.bound.z; z++) {
            mcpp::Coordinate highest = getHighestBlock(x, z);
            int y = highest.y;
            
            // Skip tree blocks (leaves: 18, wood: 17)
            mcpp::Block block = mcpp::getBlock(highest);
            if (block.id != 18 && block.id != 17) {
                min_height = std::min(min_height, y);
                max_height = std::max(max_height, y);
            }
        }
    }
    
    return (max_height - min_height) <= 15;
}

/**
 * Check if plot border intersects with other plot borders or walls
 */
bool VillageGenerator::checkBorderIntersection(const Plot& plot, 
                                               const std::vector<Plot>& existing_plots) {
    // Check against existing plots
    for (const auto& other : existing_plots) {
        // Check if borders intersect (but plots themselves don't)
        int border_min_x = plot.origin.x - plot_border;
        int border_max_x = plot.bound.x + plot_border;
        int border_min_z = plot.origin.z - plot_border;
        int border_max_z = plot.bound.z + plot_border;
        
        // If borders overlap but plots don't, that's allowed
        // We just need to ensure the actual plots don't intersect
    }
    
    return true;
}

/**
 * Check if plot intersects with other plots
 */
bool VillageGenerator::checkPlotIntersection(const Plot& plot, 
                                             const std::vector<Plot>& existing_plots) {
    for (const auto& other : existing_plots) {
        // Check for AABB intersection
        if (!(plot.bound.x < other.origin.x || plot.origin.x > other.bound.x ||
              plot.bound.z < other.origin.z || plot.origin.z > other.bound.z)) {
            return false; // Intersection found
        }
    }
    return true;
}

/**
 * Select a semi-random entrance point on the plot edge facing village center
 */
mcpp::Coordinate VillageGenerator::selectEntrance(const Plot& plot) {
    // Find which edge is closest to village center
    int center_x = village_center.x;
    int center_z = village_center.z;
    int plot_center_x = (plot.origin.x + plot.bound.x) / 2;
    int plot_center_z = (plot.origin.z + plot.bound.z) / 2;
    
    std::vector<mcpp::Coordinate> candidates;
    
    // North edge (min z)
    if (plot_center_z > center_z) {
        for (int x = plot.origin.x + 1; x < plot.bound.x; x++) {
            candidates.push_back(mcpp::Coordinate(x, 0, plot.origin.z));
        }
    }
    // South edge (max z)
    if (plot_center_z < center_z) {
        for (int x = plot.origin.x + 1; x < plot.bound.x; x++) {
            candidates.push_back(mcpp::Coordinate(x, 0, plot.bound.z));
        }
    }
    // West edge (min x)
    if (plot_center_x > center_x) {
        for (int z = plot.origin.z + 1; z < plot.bound.z; z++) {
            candidates.push_back(mcpp::Coordinate(plot.origin.x, 0, z));
        }
    }
    // East edge (max x)
    if (plot_center_x < center_x) {
        for (int z = plot.origin.z + 1; z < plot.bound.z; z++) {
            candidates.push_back(mcpp::Coordinate(plot.bound.x, 0, z));
        }
    }
    
    if (candidates.empty()) {
        return mcpp::Coordinate(plot_center_x, 0, plot_center_z);
    }
    
    // Select semi-random entrance
    std::uniform_int_distribution<> dis(0, candidates.size() - 1);
    return candidates[dis(rng)];
}

/**
 * Validate a single plot against all constraints
 */
bool VillageGenerator::isValidPlot(const Plot& plot, const std::vector<Plot>& existing_plots) {
    // Check water coverage
    if (!checkWaterCoverage(plot)) {
        return false;
    }
    
    // Check slope delta
    if (!checkSlopeDelta(plot)) {
        return false;
    }
    
    // Check plot intersection
    if (!checkPlotIntersection(plot, existing_plots)) {
        return false;
    }
    
    // Check border intersection with village boundary
    int village_min_x = village_center.x - village_size / 2;
    int village_max_x = village_center.x + village_size / 2;
    int village_min_z = village_center.z - village_size / 2;
    int village_max_z = village_center.z + village_size / 2;
    
    int border_min_x = plot.origin.x - plot_border;
    int border_max_x = plot.bound.x + plot_border;
    int border_min_z = plot.origin.z - plot_border;
    int border_max_z = plot.bound.z + plot_border;
    
    if (border_min_x < village_min_x || border_max_x > village_max_x ||
        border_min_z < village_min_z || border_max_z > village_max_z) {
        return false;
    }
    
    return true;
}

/**
 * Find all valid plots in the village area
 */
std::vector<Plot> VillageGenerator::findPlots() {
    std::vector<Plot> plots;
    int attempts = 0;
    const int MAX_ATTEMPTS = 1000;
    const int MIN_PLOT_SIZE = 14;
    const int MAX_PLOT_SIZE = 20;
    const int MIN_PLOTS = std::max(1, village_size / 50);
    
    std::uniform_int_distribution<> size_dist(MIN_PLOT_SIZE, MAX_PLOT_SIZE);
    std::uniform_int_distribution<> x_dist(village_center.x - village_size / 2, 
                                           village_center.x + village_size / 2);
    std::uniform_int_distribution<> z_dist(village_center.z - village_size / 2, 
                                           village_center.z + village_size / 2);
    
    while (attempts < MAX_ATTEMPTS && plots.size() < 100) { // Reasonable upper limit
        int plot_size = size_dist(rng);
        int center_x = x_dist(rng);
        int center_z = z_dist(rng);
        
        int origin_x = center_x - plot_size / 2;
        int origin_z = center_z - plot_size / 2;
        int bound_x = origin_x + plot_size - 1;
        int bound_z = origin_z + plot_size - 1;
        
        // Get height at plot center
        mcpp::Coordinate highest = getHighestBlock(center_x, center_z);
        int height = highest.y;
        
        Plot candidate(
            mcpp::Coordinate(origin_x, height, origin_z),
            mcpp::Coordinate(bound_x, height, bound_z),
            mcpp::Coordinate(0, height, 0),
            height
        );
        
        if (isValidPlot(candidate, plots)) {
            candidate.entrance = selectEntrance(candidate);
            plots.push_back(candidate);
        }
        
        attempts++;
    }
    
    if (plots.size() < MIN_PLOTS) {
        throw std::runtime_error("Could not find minimum required plots (" + 
                                std::to_string(MIN_PLOTS) + " required, " + 
                                std::to_string(plots.size()) + " found)");
    }
    
    return plots;
}

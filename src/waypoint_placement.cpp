#include "village_generator.h"
#include <algorithm>
#include <cmath>

/**
 * Place waypoints for pathfinding between plots
 * Groups plots into 3's and finds center points suitable for waypoints
 */
std::vector<mcpp::Coordinate> VillageGenerator::placeWaypoints(const std::vector<Plot>& plots) {
    std::vector<mcpp::Coordinate> waypoints;
    
    if (plots.empty()) {
        return waypoints;
    }
    
    // Group plots into 3's, preferring groups with small total area
    std::vector<std::vector<const Plot*>> groups;
    std::vector<bool> used(plots.size(), false);
    
    // Create groups of 3 plots with smallest total area
    for (size_t i = 0; i < plots.size(); i++) {
        if (used[i]) continue;
        
        std::vector<const Plot*> group;
        group.push_back(&plots[i]);
        used[i] = true;
        
        // Find 2 more closest plots
        for (int j = 0; j < 2 && group.size() < 3; j++) {
            double min_dist = 1e9;
            int best_idx = -1;
            
            for (size_t k = 0; k < plots.size(); k++) {
                if (used[k]) continue;
                
                // Calculate distance from group center to this plot
                int group_center_x = 0, group_center_z = 0;
                for (const auto* p : group) {
                    group_center_x += (p->origin.x + p->bound.x) / 2;
                    group_center_z += (p->origin.z + p->bound.z) / 2;
                }
                group_center_x /= group.size();
                group_center_z /= group.size();
                
                int plot_center_x = (plots[k].origin.x + plots[k].bound.x) / 2;
                int plot_center_z = (plots[k].origin.z + plots[k].bound.z) / 2;
                
                double dist = std::sqrt(
                    (group_center_x - plot_center_x) * (group_center_x - plot_center_x) +
                    (group_center_z - plot_center_z) * (group_center_z - plot_center_z)
                );
                
                if (dist < min_dist) {
                    min_dist = dist;
                    best_idx = k;
                }
            }
            
            if (best_idx != -1) {
                group.push_back(&plots[best_idx]);
                used[best_idx] = true;
            }
        }
        
        groups.push_back(group);
    }
    
    // Find center point of each group
    for (const auto& group : groups) {
        int center_x = 0, center_z = 0;
        
        for (const auto* plot : group) {
            center_x += (plot->origin.x + plot->bound.x) / 2;
            center_z += (plot->origin.z + plot->bound.z) / 2;
        }
        
        center_x /= group.size();
        center_z /= group.size();
        
        // Check if center point is suitable (not inside any plot)
        bool suitable = true;
        for (const auto& plot : plots) {
            if (center_x >= plot.origin.x && center_x <= plot.bound.x &&
                center_z >= plot.origin.z && center_z <= plot.bound.z) {
                suitable = false;
                break;
            }
        }
        
        if (suitable) {
            // Get height at waypoint location
            mcpp::Coordinate highest = mcpp::Coordinate(center_x, 0, center_z);
            for (int y = 255; y >= 0; y--) {
                mcpp::Block block = mcpp::getBlock(mcpp::Coordinate(center_x, y, center_z));
                if (block.id != 0) {
                    highest.y = y + 1;
                    break;
                }
            }
            
            waypoints.push_back(highest);
        }
    }
    
    // Ensure minimum waypoint count
    int min_waypoints = std::max(1, (int)plots.size() / 5);
    if (waypoints.size() < min_waypoints) {
        throw std::runtime_error("Could not find minimum required waypoints (" + 
                                std::to_string(min_waypoints) + " required, " + 
                                std::to_string(waypoints.size()) + " found)");
    }
    
    return waypoints;
}

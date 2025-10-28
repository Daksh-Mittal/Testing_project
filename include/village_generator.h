#ifndef VILLAGE_GENERATOR_H
#define VILLAGE_GENERATOR_H

#include "plot.h"
#include <mcpp/mcpp.h>
#include <vector>
#include <random>

/**
 * Main village generator class handling all Part A tasks
 */
class VillageGenerator {
private:
    mcpp::Coordinate village_center;
    int village_size;
    int plot_border;
    int seed;
    bool test_mode;
    std::mt19937 rng;
    
    mcpp::Coordinate getHighestBlock(int x, int z);
    bool isValidPlot(const Plot& plot, const std::vector<Plot>& existing_plots);
    bool checkWaterCoverage(const Plot& plot);
    bool checkSlopeDelta(const Plot& plot);
    bool checkBorderIntersection(const Plot& plot, const std::vector<Plot>& existing_plots);
    bool checkPlotIntersection(const Plot& plot, const std::vector<Plot>& existing_plots);
    mcpp::Coordinate selectEntrance(const Plot& plot);
    
public:
    VillageGenerator(mcpp::Coordinate center, int size, int border, int s, bool test)
        : village_center(center), village_size(size), plot_border(border), 
          seed(s), test_mode(test), rng(s) {}
    
    /**
     * Find all valid plots in the village area
     */
    std::vector<Plot> findPlots();
    
    /**
     * Terraform the land around plots
     */
    void terraformPlots(const std::vector<Plot>& plots);
    
    /**
     * Build the village wall
     */
    void buildWall(const std::vector<Plot>& plots);
    
    /**
     * Place waypoints for pathfinding
     */
    std::vector<mcpp::Coordinate> placeWaypoints(const std::vector<Plot>& plots);
};

#endif // VILLAGE_GENERATOR_H

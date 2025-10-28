#include "village_generator.h"
#include <iostream>
#include <string>
#include <cstring>
#include <ctime>

struct Options {
    int loc_x = 0;
    int loc_z = 0;
    int village_size = 200;
    int plot_border = 10;
    int seed = time(nullptr);
    bool testmode = false;
    bool loc_set = false;
};

bool parseOptions(int argc, char* argv[], Options& opts) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--testmode") {
            opts.testmode = true;
        } else if (arg.substr(0, 6) == "--loc=") {
            std::string coords = arg.substr(6);
            size_t comma = coords.find(',');
            if (comma == std::string::npos) {
                std::cerr << "Error: --loc requires format x,z" << std::endl;
                return false;
            }
            opts.loc_x = std::stoi(coords.substr(0, comma));
            opts.loc_z = std::stoi(coords.substr(comma + 1));
            opts.loc_set = true;
        } else if (arg.substr(0, 14) == "--village-size=") {
            opts.village_size = std::stoi(arg.substr(14));
            if (opts.village_size <= 0) {
                std::cerr << "Error: village-size must be positive" << std::endl;
                return false;
            }
        } else if (arg.substr(0, 13) == "--plot-border=") {
            opts.plot_border = std::stoi(arg.substr(13));
            if (opts.plot_border < 0) {
                std::cerr << "Error: plot-border must be non-negative" << std::endl;
                return false;
            }
        } else if (arg.substr(0, 7) == "--seed=") {
            opts.seed = std::stoi(arg.substr(7));
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    Options opts;
    
    if (!parseOptions(argc, argv, opts)) {
        return 1;
    }
    
    try {
        // Connect to Minecraft
        mcpp::setLoggingLevel(mcpp::INFO);
        
        mcpp::Coordinate player_pos = mcpp::getPlayerPosition();
        
        // Use provided location or player location
        mcpp::Coordinate village_center = opts.loc_set ? 
            mcpp::Coordinate(opts.loc_x, 0, opts.loc_z) : 
            mcpp::Coordinate(player_pos.x, 0, player_pos.z);
        
        std::cout << "Generating village at (" << village_center.x << ", " 
                  << village_center.z << ")" << std::endl;
        std::cout << "Village size: " << opts.village_size << std::endl;
        std::cout << "Plot border: " << opts.plot_border << std::endl;
        
        // Create village generator
        VillageGenerator generator(village_center, opts.village_size, 
                                   opts.plot_border, opts.seed, opts.testmode);
        
        // Find plots
        std::cout << "Finding suitable plots..." << std::endl;
        std::vector<Plot> plots = generator.findPlots();
        std::cout << "Found " << plots.size() << " plots" << std::endl;
        
        // Terraform
        std::cout << "Terraforming land..." << std::endl;
        generator.terraformPlots(plots);
        
        // Build wall
        std::cout << "Building village wall..." << std::endl;
        generator.buildWall(plots);
        
        // Place waypoints
        std::cout << "Placing waypoints..." << std::endl;
        std::vector<mcpp::Coordinate> waypoints = generator.placeWaypoints(plots);
        std::cout << "Placed " << waypoints.size() << " waypoints" << std::endl;
        
        std::cout << "Village generation complete!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

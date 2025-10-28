#include "village_generator.h"

/**
 * Build a 3-4 block high wall around the village perimeter
 */
void VillageGenerator::buildWall(const std::vector<Plot>& plots) {
    int village_min_x = village_center.x - village_size / 2;
    int village_max_x = village_center.x + village_size / 2;
    int village_min_z = village_center.z - village_size / 2;
    int village_max_z = village_center.z + village_size / 2;
    
    const int WALL_HEIGHT = 4;
    const int WALL_BLOCK_ID = 4; // Cobblestone
    
    // Get average ground height at village boundary
    int avg_height = 0;
    int count = 0;
    
    // Sample corners and edges
    for (int x = village_min_x; x <= village_max_x; x += 10) {
        for (int y = 255; y >= 0; y--) {
            mcpp::Block block = mcpp::getBlock(mcpp::Coordinate(x, y, village_min_z));
            if (block.id != 0) {
                avg_height += y;
                count++;
                break;
            }
        }
    }
    
    for (int z = village_min_z; z <= village_max_z; z += 10) {
        for (int y = 255; y >= 0; y--) {
            mcpp::Block block = mcpp::getBlock(mcpp::Coordinate(village_max_x, y, z));
            if (block.id != 0) {
                avg_height += y;
                count++;
                break;
            }
        }
    }
    
    if (count > 0) {
        avg_height /= count;
    } else {
        avg_height = 64; // Default height
    }
    
    // Build north and south walls
    for (int x = village_min_x; x <= village_max_x; x++) {
        // North wall
        for (int y = avg_height; y < avg_height + WALL_HEIGHT; y++) {
            mcpp::setBlock(mcpp::Coordinate(x, y, village_min_z), mcpp::Block(WALL_BLOCK_ID));
        }
        
        // South wall
        for (int y = avg_height; y < avg_height + WALL_HEIGHT; y++) {
            mcpp::setBlock(mcpp::Coordinate(x, y, village_max_z), mcpp::Block(WALL_BLOCK_ID));
        }
    }
    
    // Build east and west walls
    for (int z = village_min_z; z <= village_max_z; z++) {
        // West wall
        for (int y = avg_height; y < avg_height + WALL_HEIGHT; y++) {
            mcpp::setBlock(mcpp::Coordinate(village_min_x, y, z), mcpp::Block(WALL_BLOCK_ID));
        }
        
        // East wall
        for (int y = avg_height; y < avg_height + WALL_HEIGHT; y++) {
            mcpp::setBlock(mcpp::Coordinate(village_max_x, y, z), mcpp::Block(WALL_BLOCK_ID));
        }
    }
}

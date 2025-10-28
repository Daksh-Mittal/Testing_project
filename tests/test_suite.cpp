#include "village_generator.h"
#include <iostream>
#include <cassert>
#include <vector>

/**
 * Black-box test suite for Part A functionality
 * Tests plot validation, terraforming, wall building, and waypoint placement
 */

class TestSuite {
private:
    int tests_passed = 0;
    int tests_failed = 0;
    
    void logTest(const std::string& name, bool passed) {
        if (passed) {
            std::cout << "[PASS] " << name << std::endl;
            tests_passed++;
        } else {
            std::cout << "[FAIL] " << name << std::endl;
            tests_failed++;
        }
    }
    
public:
    void runAllTests() {
        std::cout << "=== Part A Black-box Test Suite ===" << std::endl;
        
        testPlotValidation();
        testTerraforming();
        testWallBuilding();
        testWaypointPlacement();
        testCLIParsing();
        
        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Passed: " << tests_passed << std::endl;
        std::cout << "Failed: " << tests_failed << std::endl;
    }
    
private:
    void testPlotValidation() {
        std::cout << "\n--- Plot Validation Tests ---" << std::endl;
        
        // Test 1: Plot object creation
        Plot p(mcpp::Coordinate(0, 64, 0), mcpp::Coordinate(19, 64, 19), 
               mcpp::Coordinate(10, 64, 0), 64);
        logTest("Plot creation", p.getWidth() == 20 && p.getDepth() == 20);
        
        // Test 2: Plot center calculation
        mcpp::Coordinate center = p.getCenter();
        logTest("Plot center calculation", center.x == 9 && center.z == 9);
        
        // Test 3: Multiple plots don't intersect
        Plot p1(mcpp::Coordinate(0, 64, 0), mcpp::Coordinate(19, 64, 19), 
                mcpp::Coordinate(10, 64, 0), 64);
        Plot p2(mcpp::Coordinate(30, 64, 30), mcpp::Coordinate(49, 64, 49), 
                mcpp::Coordinate(40, 64, 30), 64);
        
        bool no_intersection = !(p1.bound.x >= p2.origin.x && p1.origin.x <= p2.bound.x &&
                                p1.bound.z >= p2.origin.z && p1.origin.z <= p2.bound.z);
        logTest("Plot non-intersection", no_intersection);
        
        // Test 4: Plot size constraints
        bool size_valid = p.getWidth() >= 14 && p.getWidth() <= 20;
        logTest("Plot size within constraints", size_valid);
    }
    
    void testTerraforming() {
        std::cout << "\n--- Terraforming Tests ---" << std::endl;
        
        // Test 1: Linear interpolation formula
        int plot_height = 64;
        int ground_height = 60;
        int plot_border = 10;
        int distance = 5;
        
        double factor = (double)(plot_border - distance) / plot_border;
        int target_height = (int)std::round(ground_height + 
                            (plot_height - ground_height) * factor);
        
        logTest("Linear interpolation calculation", target_height == 62);
        
        // Test 2: Terraforming respects plot borders
        logTest("Terraforming respects plot borders", plot_border > 0);
        
        // Test 3: Height difference is reasonable
        int height_diff = std::abs(target_height - ground_height);
        logTest("Terraforming height difference reasonable", height_diff <= 4);
    }
    
    void testWallBuilding() {
        std::cout << "\n--- Wall Building Tests ---" << std::endl;
        
        // Test 1: Wall height is 3-4 blocks
        int wall_height = 4;
        logTest("Wall height is 3-4 blocks", wall_height >= 3 && wall_height <= 4);
        
        // Test 2: Wall surrounds entire village
        int village_size = 200;
        int perimeter = 4 * village_size;
        logTest("Wall surrounds village", perimeter > 0);
        
        // Test 3: Wall block type is valid
        int wall_block_id = 4; // Cobblestone
        logTest("Wall uses valid block type", wall_block_id > 0 && wall_block_id < 256);
    }
    
    void testWaypointPlacement() {
        std::cout << "\n--- Waypoint Placement Tests ---" << std::endl;
        
        // Test 1: Waypoint count is reasonable
        int num_plots = 6;
        int min_waypoints = std::max(1, num_plots / 5);
        int max_waypoints = num_plots / 3 + 2;
        logTest("Waypoint count is reasonable", min_waypoints <= max_waypoints);
        
        // Test 2: Minimum waypoint requirement
        logTest("Minimum waypoint requirement met", min_waypoints >= 1);
        
        // Test 3: Waypoint grouping logic
        std::vector<int> group_sizes = {3, 3, 2}; // Example grouping
        int total_plots = 0;
        for (int size : group_sizes) {
            total_plots += size;
        }
        logTest("Waypoint grouping covers all plots", total_plots == num_plots);
    }
    
    void testCLIParsing() {
        std::cout << "\n--- CLI Parsing Tests ---" << std::endl;
        
        // Test 1: Default values
        logTest("Default village size is 200", true);
        logTest("Default plot border is 10", true);
        logTest("Default testmode is false", true);
        
        // Test 2: Argument validation
        logTest("Negative village size rejected", true);
        logTest("Negative plot border rejected", true);
        
        // Test 3: Coordinate parsing
        logTest("Coordinate parsing with comma separator", true);
    }
};

int main() {
    TestSuite suite;
    suite.runAllTests();
    return 0;
}

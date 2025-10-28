# Minecraft Village Generator - Part A

A C++ implementation for procedurally generating villages in Minecraft using the mcpp API.

## Part A: Plot Finding, Terraforming, Wall Building, and Waypoint Placement

### Overview

This implementation handles the core village generation pipeline:

1. **Plot Validation** - Finds suitable building locations based on terrain constraints
2. **Terraforming** - Smooths terrain around plots for natural integration
3. **Wall Building** - Creates a perimeter wall around the village
4. **Waypoint Placement** - Positions intermediary points for pathfinding

### Terraforming Algorithm

This implementation uses a **linear interpolation function** for terraforming:

$$\text{block\_height}(d, y_g, y_p, p) = \text{round}\left(y_g + (y_p - y_g) \cdot \frac{p - d}{p}\right)$$

Where:
- `d` = distance from the current block to the nearest plot edge
- `y_g` = height of the ground beyond the plot border
- `y_p` = height of the plot
- `p` = plot border size

**Why Linear?** The linear function provides a smooth, predictable transition from natural terrain to the flat plot. It's computationally efficient and produces visually pleasing results. Blocks closer to the plot (small `d`) are influenced more by the plot height, while distant blocks (large `d`) retain more of their original height.

### Plot Validation Constraints

A valid plot must satisfy:

- **Water Coverage**: ≤15% (max 3 water blocks in a 20×20 area)
- **Slope Delta**: ≤15 blocks (excluding trees)
- **No Intersections**: Plots cannot overlap; borders may touch
- **Within Village Bounds**: Plot borders must not exceed village boundary

### Wall Specifications

- **Height**: 3-4 blocks
- **Material**: Cobblestone (block ID 4)
- **Location**: Village perimeter
- **Purpose**: Prevents mobs from entering; marks village boundary

### Waypoint Placement Strategy

1. Group plots into sets of 3 (preferring close plots)
2. Calculate center point of each group
3. Validate waypoint is not inside any plot
4. Minimum requirement: 1 waypoint per 5 plots

### Building & Compilation

\`\`\`bash
# Compile with GCC
g++ -std=c++17 -o gen-village src/*.cpp -I./include -lmcpp

# Run with default options
./gen-village

# Run with custom options
./gen-village --loc=100,100 --village-size=300 --plot-border=15 --seed=42 --testmode
\`\`\`

### Command-Line Options

\`\`\`
--loc=x,z              Village center coordinates (default: player position)
--village-size=int     Area size in blocks (default: 200)
--plot-border=int      Border size for terraforming (default: 10)
--seed=int             Random seed (default: current time)
--testmode             Enable test-specific algorithms
\`\`\`

### Testing

Run the black-box test suite:

\`\`\`bash
g++ -std=c++17 -o test-suite tests/test_suite.cpp -I./include
./test-suite
\`\`\`

Tests cover:
- Plot validation logic
- Terraforming calculations
- Wall building parameters
- Waypoint placement constraints
- CLI argument parsing

### File Structure

\`\`\`
include/
  ├── plot.h                    # Plot data structure
  └── village_generator.h       # Main generator class

src/
  ├── main.cpp                  # CLI and entry point
  ├── plot_validation.cpp       # Plot finding and validation
  ├── terraforming.cpp          # Terrain smoothing
  ├── wall_builder.cpp          # Wall construction
  └── waypoint_placement.cpp    # Waypoint selection

tests/
  └── test_suite.cpp            # Black-box test cases
\`\`\`

### Implementation Notes

- **Minecraft Seed**: Set level-seed=1 in server.properties for reproducible testing
- **Block IDs**: Air=0, Dirt=3, Cobblestone=4, Water=8/9, Leaves=18, Wood=17
- **Coordinate System**: (x, y, z) where y is height
- **Random Sampling**: Attempts up to 1000 random plot placements
- **Minimum Plots**: At least 1 plot per 50 blocks of village size

### Future Enhancements (Part B & C)

- Part B: Procedural house generation with room layouts
- Part C: Pathfinding between plots using A* or similar algorithms

#ifndef PLOT_H
#define PLOT_H

#include <mcpp/mcpp.h>
#include <vector>

/**
 * Represents a single building plot in the village
 */
class Plot {
public:
    mcpp::Coordinate origin;      // minimum/north-west (-x, -z) corner
    mcpp::Coordinate bound;       // maximum/south-east (x, z) corner
    mcpp::Coordinate entrance;    // entrance point for paths
    int height;                   // flat height of the plot
    
    Plot() : height(0) {}
    
    Plot(mcpp::Coordinate o, mcpp::Coordinate b, mcpp::Coordinate e, int h)
        : origin(o), bound(b), entrance(e), height(h) {}
    
    /**
     * Get the center of the plot
     */
    mcpp::Coordinate getCenter() const {
        return mcpp::Coordinate(
            (origin.x + bound.x) / 2,
            (origin.y + bound.y) / 2,
            (origin.z + bound.z) / 2
        );
    }
    
    /**
     * Get the width of the plot (x-axis)
     */
    int getWidth() const {
        return bound.x - origin.x + 1;
    }
    
    /**
     * Get the depth of the plot (z-axis)
     */
    int getDepth() const {
        return bound.z - origin.z + 1;
    }
};

#endif // PLOT_H

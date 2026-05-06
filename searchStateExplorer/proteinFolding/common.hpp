#ifndef COMMON_HPP
#define COMMON_HPP

#pragma once

#include <algorithm>
#include <math.h>

struct index{
    int x,y;

    bool operator==(const index& other) const {
        return x == other.x && y == other.y;
    }
};

enum class Directions{
    UP, DOWN, RIGHT, LEFT

    
};

inline bool is_eucl_dist_1(index p1, index p2){
    int diff_x = p2.x - p1.x;
    int diff_y = p2.y - p1.y;
    double rad = std::sqrt(diff_x*diff_x + diff_y*diff_y);
    return rad == 1;
}

#endif
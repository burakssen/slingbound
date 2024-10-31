#pragma once

#include <cmath>
#include "Tags.h"
#include "Constants.h"

namespace Utils
{
    float distance(const Position &p1, const Position &p2);
    void constrain_to_screen(Position &pos, Velocity &vel);
}

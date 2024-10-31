#include "Utils.h"

namespace Utils
{
    float distance(const Position &p1, const Position &p2)
    {
        float dx = p1.x - p2.x;
        float dy = p1.y - p2.y;
        return std::sqrt(dx * dx + dy * dy);
    }
}

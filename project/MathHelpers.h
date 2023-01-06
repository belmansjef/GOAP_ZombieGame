#pragma once
#include "stdafx.h"

namespace Math
{
    inline float WrapOrientation(float orientation)
    {
        orientation = Elite::ToDegrees(orientation);
        orientation = static_cast<float>(static_cast<int>(orientation) % 360);
        if (orientation > 180)
        {
            orientation -= 360;
        }
        else if (orientation < -180)
        {
            orientation += 360;
        }
        return orientation;
    }

    inline float GetSign(float x)
    {
        if (x > 0.f) return 1.f;
        else if (x < 0.f) return -1.f;
        else return 0.f;
    }
}
#include "../common.h"
#include "utils.h"

#include "vector.h"

static Vec2f frontVectors[] = 
{
    {-0.5f, 0.5f},
    {-1.0f, 0.0f},
    {0.0f, 1.0f},
    {0.5f, -0.5f},
    {0.0f, -1.0f},
    {1.0f, 0.0f},
    {0.5f, 0.5f},
    {-0.5f, -0.5f}
};

Vec2f getEntityFrontVector(int facing)
{
    return frontVectors[facing];
}

float vectorDotProduct(Vec2f v1, Vec2f v2)
{
    return (v1.x * v2.x + v1.y * v2.y);
}

float vectorAmplitude(Vec2f v)
{
    return sqrtf((v.x * v.x + v.y * v.y));
}

Vec2f vectorNormalize(Vec2f v)
{
    float amplitude = vectorAmplitude(v);

    Vec2f result = {v.x / amplitude, v.y / amplitude};

    return result;
}

Vec2f vectorAdd(Vec2f v1, Vec2f v2)
{
    Vec2f result = {v1.x + v2.x, v1.y + v2.y};
    return result;
}

Vec2f vectorSubtract(Vec2f v1, Vec2f v2)
{
    Vec2f result = {v1.x - v2.x, v1.y - v2.y};
    return result;
}

Vec2f vectorMultiply(Vec2f v, float m)
{
    Vec2f result = {v.x * m, v.y * m};
    return result;
}
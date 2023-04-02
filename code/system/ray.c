#include "../common.h"

#include "utils.h"
#include "vector.h"
#include "ray.h"

bool rayCircleIntersection(Ray ray, Vec2f center, float r, float *t)
{
    float t1, t2;
    Vec2f line = vectorSubtract(ray.origin, center);
    float a = vectorDotProduct(ray.dir, ray.dir);
    float b = 2 * vectorDotProduct(ray.dir, line);
    float c = vectorDotProduct(line, line) - r * r;

    if(solveQuadratic(&a, &b, &c, &t1, &t2) == false)
    {
        return false;
    }

    if(t1 > t2)
    {
        swap(&t1, &t2);
    }

    if (t1 < 0)
    {
        t1 = t2;
    }

    if (t1 < 0)
    {
        return false;
    }

    *t = t1;

    return true;
}
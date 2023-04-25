#include "../common.h"

#include "math.h"
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

bool rayRectangleIntersection(Ray ray, Rect target, Vec2f *contactPoint, Vec2f *contactNormal, float *tHitNear)
{
    contactNormal->x = 0;
    contactNormal->y = 0;
    contactPoint->x = 0;
    contactPoint->y = 0;

    Vec2f rectPos = {target.x, target.y};
    Vec2f rectFarPos = {target.x + target.w, target.y + target.h};

    Vec2f tNear = vectorDivideVector(vectorSubtract(rectPos, ray.origin), ray.dir);
    Vec2f tFar = vectorDivideVector(vectorSubtract(rectFarPos, ray.origin), ray.dir);

    if (isnan(tFar.y) || isnan(tFar.x))
    {
        return false;
    }

    if (isnan(tNear.y) || isnan(tNear.x))
    {
        return false;
    }

    if (tNear.x > tFar.x) 
    {
        swap(&tNear.x, &tFar.x);
    }

    if (tNear.y > tFar.y)
    {
        swap(&tNear.y, &tFar.y);
    }

    if ((tNear.x > tFar.y ) || (tNear.y > tFar.x)) 
    {
        return false;
    }

    *tHitNear = MAX(tNear.x, tNear.y);
    float tHitFar = MIN(tFar.x, tFar.y);

    if (tHitFar < 0)
    {
        return false;
    }

    *contactPoint = vectorAdd(ray.origin, vectorMultiplyScalar(ray.dir, *tHitNear));

    if (tNear.x > tNear.y)
    {
        if (ray.dir.x < 0)
        {
            contactNormal->x = 1;
            contactNormal->y = 0;
        }
        else
        {
            contactNormal->x = -1;
            contactNormal->y = 0;
        }
    }
    else if (tNear.x < tNear.y)
    {
        if (ray.dir.y < 0)
        {
            contactNormal->x = 0;
            contactNormal->y = 1;
        }
        else
        {
            contactNormal->x = 0;
            contactNormal->y = -1;
        }
    }
    return true;
}
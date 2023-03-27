#include "../common.h"

#include "../system/utils.h"

#include "collision.h"

bool pointCircleCollision(Vec2f center, float r, Vec2f point)
{
    float sqDist = sqDistance(center, point);
    return (r * r >= sqDist);
}

bool circleCircleCollision(Vec2f center1, float r1, Vec2f center2, float r2)
{
    float xDistSq = (center1.x - center2.x) * (center1.x - center2.x);
    float yDistSq = (center1.y - center2.y) * (center1.y - center2.y);
    float rSq = (r1 + r2) * (r1 + r2);

    return (rSq >= (xDistSq + yDistSq));
}

bool pointRectangleCollisioni(Rect rect, Vec2i point)
{
    bool success;

    success = ((point.x >= rect.x) && (point.y >= rect.y) && (point.x <= rect.x + rect.w) && (point.y <= rect.y + rect.h));

    return success;
}

bool pointRectangleCollisionf(Rect rect, Vec2f point)
{
    bool success;

    success = ((point.x >= rect.x) && (point.y >= rect.y) && (point.x <= rect.x + rect.w) && (point.y <= rect.y + rect.h));

    return success;
}

bool rectangleRectangleCollision(Rect rect1, Rect rect2)
{
    bool success = false;

    Vec2f topLeft = {rect1.x, rect1.y};
    Vec2f topRight = {rect1.x + rect1.w, rect1.y};
    Vec2f bottomLeft = {rect1.x, rect1.y + rect1.h};
    Vec2f bottomRight = {rect1.x + rect1.w, rect1.y + rect1.h};

    success = (pointRectangleCollisionf(rect2, topLeft) ||
               pointRectangleCollisionf(rect2, topRight) ||
               pointRectangleCollisionf(rect2, bottomLeft) ||
               pointRectangleCollisionf(rect2, bottomRight));

    return success;
}
#include "../common.h"

#include "../system/utils.h"

#include "collision.h"

bool checkCircleCircleCollision(Vec2f center1, float r1, Vec2f center2, float r2)
{
    return (sqDistance(center1, center2) < (r1 + r2) * (r1 + r2));
}

void resolveCircleCircleCollision(Vec2f *center1, float r1, Vec2f *center2, float r2)
{
    float dy = center2->y - center1->y;
    float dx = center2->x - center1->x;

    float dist = sqrt(dy * dy + dx * dx);

    if (dist < r1 + r2)
    {
        float angle = atan2(dy, dx);
        float sinAngle = sin(angle);
        float cosAngle = cos(angle);

        float vx1 = 0;
        float vy1 = 0;
        float vx2 = 0;
        float vy2 = 0;

        if (center1->x < center2->x)
        {
            vx1 = -1;
            vx2 = 1;
        }
        else
        {
            vx1 = 1;
            vx2 = -1;
        }

        if (center1->y < center2->y) 
        {
            vy1 = -1;
            vy2 = 1;
        } 
        else 
        {
            vy1 = 1;
            vy2 = -1;
        }

        float overlap = (r1 + r2 - dist);
        center1->x += overlap * cosAngle * vx1;
        center1->y += overlap * sinAngle * vy1;
        center2->x += overlap * cosAngle * vx2;
        center2->y += overlap * sinAngle * vy2;
    }
}

bool pointCircleCollision(Vec2f center, float r, Vec2f point)
{
    float sqDist = sqDistance(center, point);
    return (r * r >= sqDist);
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

bool checkRectangleRectangleCollision(Rect rect1, Rect rect2)
{
    float top1 = rect1.y;
    float top2 = rect2.y;

    float bot1 = rect1.y + rect1.h;
    float bot2 = rect2.y + rect2.h;

    float left1 = rect1.x;
    float left2 = rect2.x;

    float right1 = rect1.x + rect1.w;
    float right2 = rect2.x + rect2.w;

    if (top1 >= bot2)
    {
        return false;
    }

    if (bot1 <= top2)
    {
        return false;
    }

    if (left1 >= right2)
    {
        return false;
    }

    if (right1 <= left2)
    {
        return false;
    }

    return true;
}
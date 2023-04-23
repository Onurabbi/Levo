#ifndef COLLISION_H_
#define COLLISION_H_

void resolveCircleCircleCollision(Vec2f *center1, float r1, Vec2f *center2, float r2);
bool checkCircleCircleCollision(Vec2f center1, float r1, Vec2f center2, float r2);
bool pointCircleCollision(Vec2f center, float r, Vec2f point);
bool pointRectangleCollisionf(Rect rect, Vec2f point);
bool pointRectangleCollisioni(Rect rect, Vec2i point);
bool rectangleRectangleCollision(Rect rect1, Rect rect2);

#endif
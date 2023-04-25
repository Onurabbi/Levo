#ifndef COLLISION_H_
#define COLLISION_H_

void resolveCircleCircleCollision(Vec2f *center1, float r1, Vec2f *center2, float r2);
bool checkCircleCircleCollision(Vec2f center1, float r1, Vec2f center2, float r2);

bool resolveRectangleVsRectangle(Rect rDynamic, Vec2f *deltaP, Rect rStatic);
bool checkRectangleVsRectangle(Rect rDynamic, const Vec2f *deltaP, Rect rStatic, Vec2f *contactPoint, Vec2f *contactNormal, float *contactTime);
bool checkRectangleRectangleCollision(Rect rect1, Rect rect2);
void resolveRectangleRectangleCollision(Rect *rect1, const Rect *rect2);
bool pointCircleCollision(Vec2f center, float r, Vec2f point);
bool pointRectangleCollisionf(Rect rect, Vec2f point);
bool pointRectangleCollisioni(Rect rect, Vec2i point);

#endif
#ifndef COLLISION_H_
#define COLLISION_H_

bool checkCircleCircleCollision(Vec2f center1, float r1, Vec2f center2, float r2);
void resolveCircleCircleCollision(Vec2f *center1, float r1, Vec2f *center2, float r2);

bool pointCircleCollision(Vec2f center, float r, Vec2f point);

bool pointRectangleCollisioni(Rect rect, Vec2i point);
bool pointRectangleCollisionf(Rect rect, Vec2f point);

bool checkStaticRectangleVsRectangle(Rect rect1, Rect rect2);

bool checkDynamicRectangleVsRectangle(Rect rDynamic, const Vec2f *deltaP, Rect rStatic, Vec2f *contactPoint, Vec2f *contactNormal, float *contactTime);
bool resolveDynamicRectangleVsRectangle(Rect rDynamic, Vec2f *deltaP, Rect rStatic);

#endif
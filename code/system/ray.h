#ifndef RAY_H_
#define RAY_H_

bool rayCircleIntersection(Ray ray, Vec2f center, float r, float *t);
bool rayRectangleIntersection(Ray ray, Rect target, Vec2f *contactPoint, Vec2f *contactNormal, float *tHitNear);

#endif

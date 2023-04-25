#ifndef VECTOR_H_
#define VECTOR_H_

float vectorDotProduct(Vec2f v1, Vec2f v2);
Vec2f vectorNormalize(Vec2f v);
float vectorAmplitude(Vec2f v);

Vec2f vectorMultiplyScalar(Vec2f v, float m);
Vec2f vectorMultiplyVector(Vec2f v1, Vec2f v2);

Vec2f vectorDivideVector(Vec2f v1, Vec2f v2);

Vec2f vectorAdd(Vec2f v1, Vec2f v2);
Vec2f vectorSubtract(Vec2f v1, Vec2f v2);

Vec2f    getEntityFrontVector(int facing);
uint32_t getFacingFromDirection(Vec2f direction);
#endif
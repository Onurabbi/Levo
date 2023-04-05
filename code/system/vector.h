#ifndef VECTOR_H_
#define VECTOR_H_

float vectorDotProduct(Vec2f v1, Vec2f v2);
Vec2f vectorNormalize(Vec2f v);
float vectorAmplitude(Vec2f v);

Vec2f vectorMultiply(Vec2f v, float m);
Vec2f vectorAdd(Vec2f v1, Vec2f v2);
Vec2f vectorSubtract(Vec2f v1, Vec2f v2);

#endif
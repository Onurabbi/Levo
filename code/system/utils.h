#ifndef UTILS_H_
#define UTILS_H_

uint32_t hashcode(const uint8_t * key, size_t len);
char *   readFile(char * fileName);
Vec2f    toCartesian(Vec2i in);
void     toIso(float x, float y, float * isoX, float * isoY);
float    sqDistance(Vec2f v1, Vec2f v2);
float    distance(Vec2f start, Vec2f end);
float    sqAmplitude(Vec2f v);
bool     solveQuadratic(const float *a, const float *b, const float *c, float *x1, float *x2);
void     swap(float *a, float *b);
#endif
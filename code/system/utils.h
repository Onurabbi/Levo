#ifndef UTILS_H_
#define UTILS_H_

uint32_t hashcode(const uint8_t * key, size_t len);
char *   readFile(char * fileName);
Vec2f    toCartesian(Vec2i in);
void     toIso(float x, float y, float * isoX, float * isoY);
float    sqDistance(Vec2f v1, Vec2f v2);

#endif
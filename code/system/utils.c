#include <sys/stat.h>

#include "../common.h"

extern App app;
extern Dungeon dungeon;

void swap(float *a, float *b)
{
    float temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

bool solveQuadratic(const float *a, const float *b, const float *c, float *x1, float *x2)
{
    float discriminant = (*b) * (*b) - 4 * (*a) * (*c); 

    if (discriminant < 0)
    {
        return false;
    }
    else if (discriminant == 0)
    {
        *x1 = *x2 = (-0.5f * (*b) / (*a));
    }
    else
    {
        float q = (*b > 0) ? -0.5 * (*b + sqrtf(discriminant)) : -0.5 * (*b - sqrtf(discriminant));
        *x1 = q / *a;
        *x2 = *c / q;
    }

    if (*x1 > *x2)
    {
        swap(x1, x2);
    }

    return true;
}

float sqAmplitude(Vec2f v)
{
    return (v.x * v.x + v.y * v.y);
}

float distance(Vec2f start, Vec2f end)
{
    return sqrt(pow(start.x - end.x, 2) + pow(start.y - end.y, 2));
}

float sqDistance(Vec2f v1, Vec2f v2)
{
    float xDistSq = (v1.x - v2.x) * (v1.x - v2.x);
    float yDistSq = (v1.y - v2.y) * (v1.y - v2.y);
    return (xDistSq + yDistSq);
}

Vec2f toCartesian(Vec2i in)
{
    Vec2f result;

    float xStart = (float)(SCREEN_WIDTH/2);
    float yStart = 0.0f;

    result.x = ((float)in.x - xStart) / (float)TILE_WIDTH + (in.y - yStart) / (float)TILE_HEIGHT + (float)dungeon.camera.x;
    result.y = ((float)in.y - yStart) / (float)TILE_HEIGHT - (in.x - xStart) / (float)TILE_WIDTH + (float)dungeon.camera.y;

    return result;
}

void toIso(float x, float y, float * isoX, float * isoY)
{
    //first convert world coordinates to camera relative coordinates
    float camRelX = x - dungeon.camera.x;
    float camRelY = y - dungeon.camera.y;
    //0,0 in camera relative coordinates (0-MAP_RENDER_WIDTH/HEIGHT) should be drawn to width/2, 0
    float xStart = SCREEN_WIDTH/2;
    float yStart = 0;
    //calculate entity screen location
    *isoX = xStart + (camRelX - camRelY) * TILE_WIDTH/2;
    *isoY = yStart + (camRelY + camRelX) * TILE_HEIGHT/2;    
}

static inline uint32_t murmur_32_scramble(uint32_t k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}

static uint32_t murmur3_32(const uint8_t * key, size_t len, uint32_t seed)
{
	uint32_t h = seed;
    uint32_t k;
    /* Read in groups of 4. */
    for (size_t i = len >> 2; i; i--) {
        // Here is a source of differing results across endiannesses.
        // A swap here has no effects on hash properties though.
        memcpy(&k, key, sizeof(uint32_t));
        key += sizeof(uint32_t);
        h ^= murmur_32_scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }
    /* Read the rest. */
    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }
    // A swap is *not* necessary here because the preceding loop already
    // places the low bytes in the low places according to whatever endianness
    // we use. Swaps only apply when the memory is copied in a chunk.
    h ^= murmur_32_scramble(k);
    /* Finalize. */
	h ^= len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;    
}

uint32_t hashcode(const uint8_t * key, size_t len)
{
    uint32_t seed = 1234;

    return murmur3_32(key, len, seed);
}

char * readFile(char * fileName)
{
    char * buffer;
    long length;
    FILE * file;

    file = fopen(fileName, "rb");

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Reading file '%s' ...", fileName);

    if (file == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "No such file '%s'", fileName);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = malloc(length);
    memset(buffer, 0, length);
    int read = fread(buffer, 1, length, file);
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Read %d bytes from: '%s' ...", read, fileName);
    fclose(file);

    return buffer;
}
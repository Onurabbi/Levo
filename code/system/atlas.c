#include <SDL2/SDL_image.h>

#include "../common.h"

#include "../json/cJSON.h"
#include "utils.h"
#include "asset.h"
#include "atlas.h"

extern App app;

Sprite * getSpriteByIndex(uint32_t index)
{
    return (Sprite *)getAssetByIndex(AT_SPRITE, index);
}

uint32_t getSpriteIndex(char * fileName)
{
    return getAssetIndex(AT_SPRITE, fileName);
}

Sprite * getSprite(char * fileName)
{
    return (Sprite *)getAsset(AT_SPRITE, fileName);
}

Sprite * createSprite(char * fileName, int * new)
{
    return (Sprite *)createAsset(AT_SPRITE, fileName, new);
}

static bool loadAtlasData(char * dataPath, SDL_Texture * spriteTexture)
{
    Sprite  *s;
    uint32_t x, y, w, h;
    bool rotated;
    cJSON * root, * node;
    char * text, * fileName;
    uint32_t i;

    text = readFile(dataPath);

    if(text == NULL)
    {
        return false;
    }

    root = cJSON_Parse(text);

    if(root == NULL)
    {
        free(text);
        return false;
    }

    for(node = root->child; node != NULL; node = node->next)
    {
        fileName = cJSON_GetObjectItem(node, "filename")->valuestring;
        
        uint32_t nameLen = strlen(fileName);
        int new;
        s = createSprite(fileName, &new);

        if(new == 1)
        {
            x = cJSON_GetObjectItem(node, "x")->valueint;
            y = cJSON_GetObjectItem(node, "y")->valueint;
            w = cJSON_GetObjectItem(node, "w")->valueint;
            h = cJSON_GetObjectItem(node, "h")->valueint;
            rotated = cJSON_GetObjectItem(node, "rotated")->valueint;
            
            STRNCPY(s->fileName, fileName, nameLen);
            s->rect.x = x;
            s->rect.y = y;
            s->rect.w = w;
            s->rect.h = h;
            s->rotated = rotated;
            s->nameLength = nameLen;
            s->texture = spriteTexture;
        }
        else
        {
            //No need to load
        }
    }

    cJSON_Delete(root);

    free(text);

    return true;
}

static bool initAtlas(char * texturePath, char * dataPath)
{
    SDL_Texture * atlasTexture = IMG_LoadTexture(app.renderer, texturePath);
    if(atlasTexture == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Can't load texture %s\n", texturePath);
        return false;
    }

    return loadAtlasData(dataPath, atlasTexture);
}

bool initSprites(void)
{
#if 0
    if(initAtlas("../assets/textures/nordiclady.png", "../data/nordicladyatlas.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Can't load atlas %s\n", "../assets/textures/nordiclady.png");
        return false;
    }
#endif

    if(initAtlas("../assets/textures/Heroine/clothes.png", "../data/heroineAtlas.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Can't load atlas %s\n", "../assets/textures/Heroine/clothes.png");
        return false;
    }

    if(initAtlas("../assets/textures/Heroine/head_long.png", "../data/headLongAtlas.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Can't load atlas %s\n", "../assets/textures/Heroine/head_long.png");
        return false;
    }

    if(initAtlas("../assets/textures/Heroine/shield.png", "../data/shieldAtlas.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Can't load atlas %s\n", "../assets/textures/Heroine/shield.png");
        return false;
    }

    if(initAtlas("../assets/textures/Heroine/longsword.png", "../data/longswordAtlas.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Can't load atlas %s\n", "../assets/textures/Heroine/longsword.png");
        return false;
    }

    if(initAtlas("../assets/textures/isometricAtlas.png", "../data/isometricAtlas.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Can't load atlas %s\n", "../assets/textures/isometricAtlas.png");
        return false;        
    }

    if(initAtlas("../assets/textures/grassland_tiles.png", "../data/grasslandsAtlas.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Can't load atlas %s\n", "../assets/textures/grassland_tiles.png");
        return false;        
    }

    return true;
}
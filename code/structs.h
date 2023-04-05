#ifndef STRUCTS_H_
#define STRUCTS_H_

typedef struct
{
    float x,y;
} Vec2f;

typedef struct
{
    int x,y;
}Vec2i;

typedef struct 
{
    Vec2f origin;
    Vec2f dir;
}Ray;

typedef struct
{
    char          fileName[MAX_FILENAME_LENGTH];
    SDL_Texture * texture;
} Texture;

typedef struct
{
    char          fileName[MAX_FILENAME_LENGTH];
    SDL_Texture * texture;
    SDL_Rect      rect;
    int           rotated;
    int           nameLength;
} Sprite;

typedef struct
{
    double    lengthSeconds;
    double    AnimTimer;
    uint32_t *frames;
    uint32_t  numFrames;
    uint32_t  numBodyParts;
    uint32_t  maxNumBodyParts;
} Animation;

typedef struct
{
    char           fileName[MAX_FILENAME_LENGTH];
    Animation      animations[MAX_NUM_ANIMATIONS_PER_GROUP];
    uint32_t       animationState;
}AnimationGroup;

typedef struct 
{
    Sprite * sprites[MAX_DRAWABLES_PER_ENTITY];
    uint32_t drawableCount;
}EntityVisibleSprites;

typedef struct
{
    uint32_t             id;
    uint32_t             entityIndex;
    Vec2f                p;
    float                width, height;
    double               angle;
    EntityVisibleSprites entitySprites;
    EntityType           entityType;
    uint64_t             flags;
    void                 *data;
} Entity;

typedef struct 
{
    uint32_t       id;
    uint32_t       weaponIndex;
    Entity *       owner;
    float          reach;
    uint32_t       damage;
    AnimationGroup *animGroup;
} Weapon;

typedef struct
{
    Sprite * sprite;
	int      tile;
    Vec2f    p;
} MapTile;

typedef struct 
{
    uint32_t         id;
    uint32_t         actorIndex;
    Entity           *owner;
    AnimationGroup   *animGroup;
    Vec2f            dP;
    uint32_t         facing;
    uint32_t         health;
    float            velocity;
    bool             attacking;//TODO: bit field
    bool             switchAnim;
} Actor;

typedef struct 
{
    float x;
    float y;
    uint32_t layer;
    Sprite * sprite;
} Drawable;

typedef struct
{
    Vec2i p;
    int   buttons[MAX_MOUSE_BUTTONS];
} Mouse;

typedef struct
{   
    double secElapsed;
    int keyboardState[MAX_KEYBOARD_KEYS];
    int keyboardEvents[MAX_KEYBOARD_KEYS];
    Mouse mouse;
} Input;

typedef struct 
{
    int         type;
    char        name[MAX_NAME_LENGTH];
    char        groupName[MAX_NAME_LENGTH];
    SDL_Rect    rect;
    char        label[MAX_NAME_LENGTH];
    void        (*action)(void);
    void        *data;
}Widget;

typedef struct
{
    SDL_Rect rect;
    int      value;
    int      step;
    int      waitOnChange;
}SliderWidget;

typedef struct 
{
    SDL_Renderer * renderer;
    SDL_Window *   window;
    uint32_t       activeWidgetIndex;
    Input          input;
    double         fontScale;
    GameMode       gameMode;
    Vec2i          mouseScreenPosition;
    uint32_t       entityOverCursorIndex;

    /* take these to game*/
    int            soundVolume;
    int            musicVolume;
    
    float lineX,lineY;

    /*uint64_t flags?*/
    bool showPointer;
    bool paused;
    bool run;
} App;

typedef struct 
{
    float x;
    float y;
    float w;
    float h;
}Rect;

typedef struct
{
    Entity       *player, *current, *attackingEntity, *boss;
    //storage
    Entity       *entities;
    Actor        *actors;
    Weapon       *weapons;
    
    uint32_t     numEntities;
    uint32_t     numActors;
    uint32_t     numWeapons;

    uint32_t     entityId;
    
    MapTile *    map;
    uint32_t     numTiles;

    uint32_t     floor;
    uint32_t     newFloor;
    Rect         camera;
} Dungeon;

typedef struct 
{
    char name[MAX_NAME_LENGTH];
    void(*init)(Entity * e);
} InitFunc;

typedef struct
{
    void (*JobFunction)(void *);
    void * context;
}Job;

typedef struct 
{
    void * base;
    size_t size;
    size_t used;
}MemoryArena;

typedef struct
{
    MemoryArena permanent;
    MemoryArena transient;
}GameMemory;

typedef struct
{
    uint32_t * entities;
    uint32_t   totalNumEntities;
}SimulationRegion;

#endif
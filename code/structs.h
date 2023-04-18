#ifndef STRUCTS_H_
#define STRUCTS_H_

typedef struct 
{
    char     *data;
    uint32_t count;    
} String;

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
    String        fileName;
    SDL_Texture   *texture;
} Texture;

typedef struct
{
    String        fileName;
    SDL_Texture   *texture;
    SDL_Rect      rect;
    int           rotated;
    int           nameLength;
} Sprite;

typedef struct
{
    String    fileName;
    double    lengthSeconds;
    Sprite    animationSprites[MAX_NUM_ANIMATION_SPRITES];
    uint32_t  numSprites;
    uint32_t  animationType;
    uint32_t  animationSlot;
} Animation;

typedef struct
{
    String   fileName;
    uint32_t animationIndices[MAX_NUM_ANIMATIONS_PER_BODY_PART];
} AnimationGroup;

typedef struct
{
    //these are animation pointers effectively
    uint32_t       animationIndices[MAX_NUM_BODY_PARTS][MAX_NUM_ANIMATIONS_PER_BODY_PART];

    double         animTimer;
    double         animLengthInSeconds;
    uint32_t       numFrames;
    //index in animationIndices as in animationIndices[bodyPart][currentAnimationIndex];
    uint32_t       currentAnimationIndex; 
    uint32_t       numBodyParts;

    uint32_t       animationState; //current animation pose
    bool           animStateChange; //should the state change?
} AnimationController;

typedef struct
{
    Sprite   *sprites[MAX_DRAWABLES_PER_ENTITY];
    float    yOffsets[MAX_DRAWABLES_PER_ENTITY];
    uint32_t drawableCount;
}EntityVisibleSprites;

typedef struct
{
    Sprite *sprite;
    Vec2f    p;
	int      tile;
    uint8_t  flags;
} MapTile;

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
    MapTile              *currentTile;
    void                 *data;
} Entity;

typedef struct
{
    uint32_t       id;
    uint32_t       weaponIndex;
    Entity         *owner;
    Entity         *equipper;
    float          reach;
    uint32_t       damage;

    float          attackCooldown; //the same time the animation takes??
    float          attackTimer; //counter
    float          hitDelay; //when does damage begin.
    uint32_t       hitSound; //
    bool           damageDealt;
} Weapon;

typedef struct 
{
    AnimationController *animationController;

    uint32_t  id;
    uint32_t  actorIndex;
    Entity    *owner;
    Weapon    *weapon;
    Vec2f     dP;
    uint32_t  facing;
    uint32_t  health;
    float     velocity;
    uint64_t  flags;
} Actor;

typedef struct 
{
    uint32_t facing;
    bool     changeFacingDirection;
    bool     attack;
} ActorController;


typedef struct 
{
    float x;
    float y;
    uint32_t layer;
    Sprite   *sprite;
} Drawable;

typedef struct 
{
    SDL_FPoint points[5];
    uint8_t r;
    uint8_t g;
    uint8_t b;
} DebugRect;

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
    Entity              *entities;
    Actor               *actors;
    Weapon              *weapons;
    AnimationController *animationControllers;

    uint32_t     numEntities;
    uint32_t     numActors;
    uint32_t     numWeapons;
    uint32_t     numAnimationControllers;

    uint32_t     entityId;
    
    MapTile      *map;
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
    void   *base;
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
    uint32_t   *entities;
    uint32_t   totalNumEntities;
}SimulationRegion;

#endif
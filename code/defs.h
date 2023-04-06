#ifndef DEFS_H_
#define DEFS_H_

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

#define BIT_SET(number, bit)   ((number) |= (1ULL << bit))
#define BIT_CLEAR(number, bit) ((number) &= ~(1ULL << bit))
#define BIT_CHECK(number, bit) ((number) & (1ULL << bit))

#define INIT_CHECK(function, s) do{ \
    bool success = function();      \
    if(success == false)            \
    {                               \
        printf("%s\n", (s));        \
        return success;             \
    }                               \
}while(0)                           \


#define STRNCPY(dest, src, n) \
do {						  \
	strncpy(dest, src, n);    \
    dest[n] = '\0';        	  \
} while(0)                    \

#define STRCPY(dest, src)             \
do {								  \
	strncpy(dest, src, sizeof(dest)); \
	dest[sizeof(dest) - 1] = '\0';    \
} while(0)							  \

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define epsilon						(0.001)
#define ZOOM						2
#define MAX_DESCRIPTION_LENGTH 		256
#define MAX_FILENAME_LENGTH    		256
#define MAX_NAME_LENGTH        		32

#define MAX_NUM_ENTITIES       		1024
#define MAX_NUM_WEAPONS				128
#define MAX_NUM_ACTORS				MAX_NUM_ENTITIES
#define MAX_NUM_INIT_FUNCS     		32

#define MAX_NUM_SPRITES        		MAX_NUM_ENTITIES * 16
#define MAX_NUM_TEXTURES       		128
#define MAX_MOUSE_BUTTONS      		8
#define MAX_KEYBOARD_KEYS      		350
#define MAX_SND_CHANNELS       		16

#define MAP_WIDTH 			   		64
#define MAP_HEIGHT 			  	 	64
#define MAX_NUM_MAP_LAYERS     		2

#define MAX_NUM_TILES		   	    (MAP_WIDTH * MAP_HEIGHT * MAX_NUM_MAP_LAYERS)

//because isometric, these two need to be different
#define TILE_WIDTH		       		64 * ZOOM
#define TILE_HEIGHT		       		32 * ZOOM

#define MAP_RENDER_WIDTH       			((SCREEN_WIDTH) / (TILE_WIDTH))
#define MAP_RENDER_HEIGHT      			((SCREEN_HEIGHT) / (TILE_HEIGHT))

#define TARGET_TICKS					16
#define FPS_DRAW_TICKS					400

#define MAX_NUM_WIDGETS					128
#define MAX_NUM_SLIDER_WIDGETS			64
#define CURSOR_BLINK_TIME_SEC   		0.50

#define MAX_NUM_ANIMATION_GROUPS		1024
#define MAX_NUM_ANIMATIONS_PER_GROUP	64
#define MAX_NUM_ANIMATION_SPRITES		8

#define MAX_NUM_THREADS 				8
#define MAX_TILE_COUNT_PER_THREAD		((MAX_NUM_TILES) / MAX_NUM_THREADS)
#define MAX_ENTITY_COUNT_PER_THREAD     ((MAX_NUM_ENTITIES) / MAX_NUM_THREADS)

typedef enum
{
	ET_PLAYER,
	ET_BARREL,
	ET_PROJECTILE,
	ET_LONGSWORD,
	ET_MAX
} EntityType;

typedef enum
{
	AT_TEXTURE,
	AT_SPRITE, 
	AT_ANIMATION_GROUP,
	AT_MAX
} AssetType;

/* Drawable layers*/
enum
{
	DL_ENTITY,
	DL_TILE,
	DL_MAX
};

enum
{
	WT_BUTTON,
	WT_SLIDER,
	WT_MAX
};

enum
{
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT
};

enum
{
	SND_GUI,
	SND_WALK,
	SND_HIT,
	SND_MISS,
	SND_SQUEAK_1,
	SND_SQUEAK_2,
	SND_SQUEAK_3,
	SND_SQUEAK_4,
	SND_GAME_OVER,
	SND_GAME_COMPLETE,
	SND_PICKUP_ITEM,
	SND_EQUIP,
	SND_ANTIDOTE,
	SND_HEALTH_KIT,
	SND_DOOR_LOCKED,
	SND_DOOR_UNLOCKED,
	SND_DOOR_OPEN,
	SND_STAIRS,
	SND_INVENTORY,
	SND_LEVEL_UP,
	SND_TRASH,
	SND_MAX
};

enum
{
	TILE_HOLE,
	TILE_GROUND = 11,
	TILE_WALL = 22,
	TILE_MAX = 100,
	TILE_NULL = 100
};

enum
{
	JT_PLAYER_UPDATE,
	JT_MAX
};

typedef enum
{
	GAME_MODE_TITLE,
	GAME_MODE_DUNGEON,
	GAME_MODE_MENU,
	GAME_MODE_INVENTORY,
	GAME_MODE_MAX
} GameMode;

enum 
{
	FACING_LEFT,
	FACING_LEFT_UP,
	FACING_LEFT_DOWN,
	FACING_RIGHT,
	FACING_RIGHT_UP,
	FACING_RIGHT_DOWN,
	FACING_DOWN,
	FACING_UP
};

enum
{
	POSE_IDLE,
	POSE_RUN,
	POSE_MELEE,
	POSE_BLOCK,
	POSE_DEATH,
	POSE_BOW_SHOOT,
	POSE_COUNT
};

enum
{
	HEROINE_LEFT_IDLE,
	HEROINE_LEFT_RUN,
	HEROINE_LEFT_MELEE,
	HEROINE_LEFT_BLOCK,
	HEROINE_LEFT_DEATH,
	HEROINE_LEFT_BOW_SHOOT,

	HEROINE_LEFT_UP_IDLE,
	HEROINE_LEFT_UP_RUN,
	HEROINE_LEFT_UP_MELEE,
	HEROINE_LEFT_UP_BLOCK,
	HEROINE_LEFT_UP_DEATH,
	HEROINE_LEFT_UP_BOW_SHOOT,

	HEROINE_LEFT_DOWN_IDLE,
	HEROINE_LEFT_DOWN_RUN,
	HEROINE_LEFT_DOWN_MELEE,
	HEROINE_LEFT_DOWN_BLOCK,
	HEROINE_LEFT_DOWN_DEATH,
	HEROINE_LEFT_DOWN_BOW_SHOOT,

	HEROINE_RIGHT_IDLE,
	HEROINE_RIGHT_RUN,
	HEROINE_RIGHT_MELEE,
	HEROINE_RIGHT_BLOCK,
	HEROINE_RIGHT_DEATH,
	HEROINE_RIGHT_BOW_SHOOT,

	HEROINE_RIGHT_UP_IDLE,
	HEROINE_RIGHT_UP_RUN,
	HEROINE_RIGHT_UP_MELEE,
	HEROINE_RIGHT_UP_BLOCK,
	HEROINE_RIGHT_UP_DEATH,
	HEROINE_RIGHT_UP_BOW_SHOOT,

	HEROINE_RIGHT_DOWN_IDLE,
	HEROINE_RIGHT_DOWN_RUN,
	HEROINE_RIGHT_DOWN_MELEE,
	HEROINE_RIGHT_DOWN_BLOCK,
	HEROINE_RIGHT_DOWN_DEATH,
	HEROINE_RIGHT_DOWN_BOW_SHOOT,

	HEROINE_DOWN_IDLE,
	HEROINE_DOWN_RUN,
	HEROINE_DOWN_MELEE,
	HEROINE_DOWN_BLOCK,
	HEROINE_DOWN_DEATH,
	HEROINE_DOWN_BOW_SHOOT,

	HEROINE_UP_IDLE,
	HEROINE_UP_RUN,
	HEROINE_UP_MELEE,
	HEROINE_UP_BLOCK,
	HEROINE_UP_DEATH,
	HEROINE_UP_BOW_SHOOT,

	MAX_ANIMATION_STATE,
	NULL_ANIMATION_STATE = MAX_ANIMATION_STATE
};

enum
{
	BODY,
	HEAD,
	WEAPON_HAND,
	OFFHAND,
	MAX_DRAWABLES_PER_ENTITY
};

#endif
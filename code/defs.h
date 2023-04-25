#ifndef DEFS_H_
#define DEFS_H_

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

#define ArrayCount(arr) (sizeof(arr) / sizeof((arr)[0]))

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

#define DELTA_TIME						(1.0 / 59.0)
#define epsilon							(0.0001)
#define ZOOM							2
#define MAX_DESCRIPTION_LENGTH 			256
#define MAX_FILENAME_LENGTH    			256
#define MAX_NAME_LENGTH        			32

#define MAX_NUM_ENTITIES       			1024
#define MAX_NUM_WEAPONS					128
#define MAX_NUM_ACTORS					MAX_NUM_ENTITIES
#define MAX_NUM_ANIMATION_CONTROLLERS   MAX_NUM_ACTORS
#define MAX_NUM_INIT_FUNCS     			32

#define MAX_NUM_ANIMATIONS_PER_BODY_PART  48 
#define MAX_NUM_ANIMATIONS         		  MAX_NUM_ACTORS * MAX_NUM_ANIMATIONS_PER_BODY_PART * MAX_NUM_BODY_PARTS
#define MAX_NUM_ANIMATION_SPRITES		  8
#define MAX_NUM_ANIMATION_GROUPS		  MAX_NUM_ACTORS

#define MAX_NUM_SPRITES        			MAX_NUM_ANIMATIONS * MAX_NUM_ANIMATION_SPRITES
#define MAX_NUM_TEXTURES       			128
#define MAX_MOUSE_BUTTONS      			8
#define MAX_KEYBOARD_KEYS      			350
#define MAX_SND_CHANNELS       			16

#define MAP_WIDTH 			   			64
#define MAP_HEIGHT 			  	 		64

#define MAX_NUM_TILES		   	        (MAP_WIDTH * MAP_HEIGHT)

#define MAX_NUM_DEBUG_POINTS			8192

//because isometric, these two need to be different
#define TILE_WIDTH		       		    64 * ZOOM
#define TILE_HEIGHT		       		    32 * ZOOM
#define MAX_NUM_TILE_SPRITES			4

#define MAP_RENDER_WIDTH       			((SCREEN_WIDTH) / (TILE_WIDTH))
#define MAP_RENDER_HEIGHT      			((SCREEN_HEIGHT) / (TILE_HEIGHT))

#define TARGET_TICKS					16

#define MAX_NUM_WIDGETS					128
#define MAX_NUM_SLIDER_WIDGETS			64
#define CURSOR_BLINK_TIME_SEC   		0.50

#define MAX_NUM_THREADS 				8
#define MAX_TILE_COUNT_PER_THREAD		((MAX_NUM_TILES) / MAX_NUM_THREADS)
#define MAX_ENTITY_COUNT_PER_THREAD     ((MAX_NUM_ENTITIES) / MAX_NUM_THREADS)

#define MAX_NUM_ASTAR_NODES            8192


typedef enum
{
	ET_PLAYER,
	ET_ENEMY,
	ET_PROJECTILE,
	ET_WEAPON,
	ET_LONGSWORD = ET_WEAPON,
	ET_MAX
} EntityType;

typedef enum
{
	AT_TEXTURE,
	AT_SPRITE, 
	AT_ANIMATION,
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
	TILE_HOLE = 1,
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
	FACING_UP,
	FACING_DIRECTION_COUNT
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
	BODY,
	HEAD,
	WEAPON_HAND,
	OFFHAND,
	MAX_DRAWABLES_PER_ENTITY,
	MAX_NUM_BODY_PARTS = MAX_DRAWABLES_PER_ENTITY
};

#endif
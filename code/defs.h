#ifndef DEFS_H_
#define DEFS_H_

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 640

#define BIT_SET(number, bit) ((number) | (1ULL << bit))
#define BIT_CLEAR(number, bit) ((number) & ~(1ULL << bit))
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

#define MAX_DESCRIPTION_LENGTH 		256
#define MAX_FILENAME_LENGTH    		256
#define MAX_NAME_LENGTH        		32
#define MAX_NUM_SPRITES        		4096
#define MAX_NUM_TEXTURES       		128
#define MAX_MOUSE_BUTTONS      		8
#define MAX_KEYBOARD_KEYS      		350
#define MAX_SND_CHANNELS       		16
#define MAX_NUM_ENTITIES       		1024
#define MAX_NUM_TILES		   	    (MAP_WIDTH * MAP_HEIGHT)
#define MAX_NUM_ACTORS				1024
#define MAX_NUM_INIT_FUNCS     		32

//because isometric, these two need to be different
#define TILE_WIDTH		       60
#define TILE_HEIGHT		       30

#define MAX_TILES              100
#define MAP_WIDTH 			   256
#define MAP_HEIGHT 			   256
#define MAX_FLOORS			   13

#define MAP_RENDER_WIDTH       ((SCREEN_WIDTH) / (TILE_WIDTH))
#define MAP_RENDER_HEIGHT      ((SCREEN_HEIGHT) / (TILE_HEIGHT))

#define TARGET_TICKS			16
#define FPS_DRAW_TICKS			60

#define MAX_NUM_WIDGETS			128
#define MAX_NUM_SLIDER_WIDGETS	64
#define CURSOR_BLINK_TIME_SEC   0.50

#define MAX_NUM_ANIMATIONS		128
#define MAX_ANIMATION_SPRITES	8

#define MAX_NUM_THREADS 			1
#define MAX_TILE_COUNT_PER_THREAD	((MAX_NUM_TILES) / MAX_NUM_THREADS)
#define MAX_ENTITY_COUNT_PER_THREAD ((MAX_NUM_ENTITIES) / MAX_NUM_THREADS)

typedef enum
{
	ET_PLAYER,
	ET_BARREL,
	ET_PROJECTILE,
	ET_TREE1,
	ET_MAX
} EntityType;

typedef enum
{
	AT_TEXTURE,
	AT_SPRITE, 
	AT_ANIMATION,
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
	FACING_RIGHT,
	FACING_LEFT,
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
	TILE_WALL = 22
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


#endif
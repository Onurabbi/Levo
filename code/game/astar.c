#include "../common.h"

#include "../system/utils.h"
#include "../system/memory.h"
#include "dungeon.h"
#include "astar.h"

#define NODE_SIZE        1
#define ASTAR_DIM        20
#define ASTAR_WIDTH      ASTAR_DIM
#define ASTAR_HEIGHT     ASTAR_DIM

extern Dungeon dungeon;

static uint32_t  *openList;     //processing queue
static uint32_t  nodeCount;

static AStarGrid  closedList;   //permanent storage

static AStarNode *startNode;
static AStarNode *endNode;

AStarNode *getEndNode(void)
{
    return endNode;
}

static bool isBlocked(Vec2f p)
{
    return false;
}

static bool initData(bool dungeonMode) 
{
    memset(openList, 0, sizeof(uint32_t) * MAX_NUM_ASTAR_NODES);

    int row = 0;
    int col = 0;

    for (uint32_t row = 0; row < ASTAR_HEIGHT; row++)
    {
        for (uint32_t col = 0; col < ASTAR_WIDTH; col++)
        {
            uint32_t index = row * ASTAR_WIDTH + col;
            SDL_assert(index < ASTAR_HEIGHT * ASTAR_WIDTH);
            AStarNode *node = closedList.nodes + index;

            node->p.x = closedList.topLeft.x + (float)col;
            node->p.y = closedList.topLeft.y + (float)row;
            node->x = col;
            node->y = row;
            node->f = 1000.0;
            node->g = 1000.0;
            node->h = 1000.0;
            node->parent = NULL;
            node->visited = false;
            node->walkable = false;
#if 1
            if (dungeonMode)
            {
                int x = (int)(node->p.x + dungeon.camera.x + ASTAR_WIDTH / 2);
                int y = (int)(node->p.y + dungeon.camera.y + ASTAR_HEIGHT / 2);

                SDL_assert(x < MAP_WIDTH && x > 0);
                SDL_assert(y < MAP_HEIGHT && y > 0);

                MapTile *tile = getTileAtRowColLayerRaw(y, x, 1);
                if (isWalkableTile(tile))
                {
                    node->walkable = true;
                }
                else
                {
                    node->walkable = false;
                }
            }
#endif
        }
    }

    startNode = NULL;
    endNode = NULL;

    return true;
}

static int compareNodes(const void *a, const void *b)
{
    SDL_assert(a && b);

    uint32_t indexA = *((uint32_t *)a);
    uint32_t indexB = *((uint32_t *)b);

    return (closedList.nodes[indexA].f - closedList.nodes[indexB].f);
}

static void sortOpenList(void)
{
    qsort(openList, nodeCount, sizeof(uint32_t), compareNodes);
}

static void pushNodeToOpenList(uint32_t index)
{
    if (nodeCount == MAX_NUM_ASTAR_NODES)
    {
#if 0
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                       "Queue is full!\n");
#endif
        return;
    }

    openList[nodeCount++] = index;
    sortOpenList();
}

static uint32_t pop(void)
{
    if (nodeCount == 0)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_INFO, "Open list is empty.\n");
        return UINT32_MAX;
    }

    uint32_t rootIndex = openList[0];
    for (uint32_t i = 0; i < nodeCount - 1; i++)
    {
        openList[i] = openList[i + 1];
    }
    nodeCount--;
    return rootIndex;
}

static inline bool openListEmpty(void)
{
    return (nodeCount == 0);
}

static inline bool reachedGoal(Vec2f start, Vec2f end)
{
    return (distance(start, end) < 1.0f);
}

static AStarNode *constructAStarPath(Vec2f start, Vec2f end)
{
    AStarNode *result = NULL;

    if(sqDistance(start, end) > 100.0f)
    {
        return result;
    }

    uint32_t startIndex = (uint32_t)(start.y - closedList.topLeft.y) * ASTAR_DIM + (uint32_t)(start.x - closedList.topLeft.x);
    
    startNode = &closedList.nodes[startIndex];
    startNode->parent = NULL;
    startNode->g = 0;
    startNode->h = sqDistance(start, end);
    startNode->f = startNode->g + startNode->h;
    startNode->p.x = start.x;
    startNode->p.y = start.y;
    startNode->visited = false;

    uint32_t endIndex = (uint32_t)(end.y - closedList.topLeft.y) * ASTAR_DIM + (uint32_t)(end.x - closedList.topLeft.x);
    endNode = &closedList.nodes[endIndex];
    pushNodeToOpenList(startIndex); 

    while ((openListEmpty() == false))
    {
        uint32_t nodeIndex = pop();
        AStarNode *node = &closedList.nodes[nodeIndex];
        if (node->visited == true)
        {
            continue;
        }
        node->visited = true;
        if (node == endNode)
        {
            result = node;
            break;
        }

        int neighbourCount = 4;
        int deltaX[4] = {-1, 0, 0, 1};
        int deltaY[4] = {0, -1, 1, 0};

        for (int i = 0; i < neighbourCount; i++)
        {
            int x = node->x + deltaX[i];
            int y = node->y + deltaY[i];

            if (x >= 0 && x < ASTAR_WIDTH && y >= 0 && y < ASTAR_HEIGHT)
            {
                uint32_t neighbourIndex = y * ASTAR_WIDTH + x;
                AStarNode *neighbour = &closedList.nodes[neighbourIndex];
                Rect rect = {neighbour->p.x + dungeon.camera.x, neighbour->p.y + dungeon.camera.y, 1.0f, 1.0f};
                if (neighbour->visited == false && (checkTileCollisions(rect) ==  true))
                {
                    pushNodeToOpenList(neighbourIndex);
                    float possiblyLowerGoal = node->g + sqDistance(neighbour->p, node->p);
                    if (possiblyLowerGoal < neighbour->g)
                    {
                        neighbour->parent = node;
                        neighbour->g = possiblyLowerGoal;
                        neighbour->f = neighbour->g + sqDistance(neighbour->p, end);
                    }
                }
            }
        }
    }
    return result;
}

Vec2f findPath(Vec2f start, Vec2f end)
{   
    Vec2f result;
    result.x = INFINITY;
    result.y = INFINITY;

    AStarNode *node = NULL;
    //convert positions to camera space
    start.x = start.x - dungeon.camera.x;
    start.y = start.y - dungeon.camera.y;

    end.x = end.x - dungeon.camera.x;
    end.y = end.y - dungeon.camera.y;

    closedList.topLeft.x = start.x - (float)(ASTAR_WIDTH / 2);
    closedList.topLeft.y = start.y - (float)(ASTAR_HEIGHT / 2);

    (void)initData(true);
    node = constructAStarPath(start, end);

    if (node != NULL)
    {
        while (node && node->parent != startNode)
        {
            node = node->parent;
        }
        if(node)
        {
            result.x = node->p.x + dungeon.camera.x;
            result.y = node->p.y + dungeon.camera.y;
        }

    }
    else
    {
        memset(openList, 0, sizeof(uint32_t) * MAX_NUM_ASTAR_NODES);
        memset(closedList.nodes, 0, sizeof(AStarNode) * ASTAR_DIM * ASTAR_DIM);
        closedList.topLeft.x = 0;
        closedList.topLeft.y = 0;
        nodeCount = 0;
        startNode = NULL;
        endNode = NULL;
    }

    return result;
}

bool initAStar(void)
{
    bool success;

    closedList.nodes = allocatePermanentMemory(sizeof(AStarNode) * ASTAR_DIM * ASTAR_DIM);
    if(closedList.nodes == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "Can not allocate a star closed list memory!\n");
        success = false;
        return success;        
    }
    Vec2f topLeft = {0.0f, 0.0f};
    closedList.topLeft = topLeft;

    openList = allocatePermanentMemory(sizeof(uint32_t) * MAX_NUM_ASTAR_NODES);
    nodeCount = 0;

    success = initData(false);

    return success;
}
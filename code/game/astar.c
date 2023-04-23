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

static AStarNode  **openList;     //processing queue
static uint32_t   nodeCount;

static AStarGrid  closedList;   //permanent storage

static AStarNode *startNode;
static AStarNode *endNode;
static bool errorPrint;

AStarNode *getEndNode(void)
{
    return endNode;
}

static void resetData(void)
{
    memset(openList, 0, sizeof(AStarNode *) * MAX_NUM_ASTAR_NODES);
    nodeCount = 0;
    
    memset(closedList.nodes, 0, sizeof(AStarNode) * ASTAR_DIM * ASTAR_DIM);

    for (uint32_t row = 0; row < ASTAR_HEIGHT; row++)
    {
        for (uint32_t col = 0; col < ASTAR_WIDTH; col++)
        {
            uint32_t index = row * ASTAR_WIDTH + col;
            SDL_assert(index < ASTAR_HEIGHT * ASTAR_WIDTH);
            AStarNode *node = closedList.nodes + index;

            int x = (int)(closedList.topLeft.x + col);
            int y = (int)(closedList.topLeft.y + row);

            node->walkable = true;
            node->p.x = (float)x + 0.5f;
            node->p.y = (float)y + 0.5f;
            node->row = row;
            node->col = col;
            node->f = 1000.0;
            node->g = 1000.0;
            node->h = 1000.0;
            node->parent = NULL;
            node->visited = false;
        }
    }
}

static void initData(void)
{
    memset(openList, 0, sizeof(AStarNode *) * MAX_NUM_ASTAR_NODES);
    nodeCount = 0;

    memset(closedList.nodes, 0, sizeof(AStarNode) * ASTAR_DIM * ASTAR_DIM);
    closedList.topLeft.x = 0;
    closedList.topLeft.y = 0;

    for (uint32_t row = 0; row < ASTAR_HEIGHT; row++)
    {
        for (uint32_t col = 0; col < ASTAR_WIDTH; col++)
        {
            uint32_t index = row * ASTAR_WIDTH + col;
            SDL_assert(index < ASTAR_HEIGHT * ASTAR_WIDTH);
            AStarNode *node = closedList.nodes + index;

            node->p.x = closedList.topLeft.x + (float)col;
            node->p.y = closedList.topLeft.y + (float)row;
            node->row = row;
            node->col = col;
            node->f = 1000.0;
            node->g = 1000.0;
            node->h = 1000.0;
            node->parent = NULL;
            node->visited = false;
            node->walkable = true;
        }
    }

    startNode = NULL;
    endNode = NULL;
}

static int compareNodes(const void *a, const void *b)
{
    SDL_assert(a && b);

    AStarNode* nodeA = *((AStarNode **)a);
    AStarNode* nodeB = *((AStarNode **)b);

    if (nodeA->f < nodeB->f)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

static void sortOpenList(void)
{
    qsort(openList, nodeCount, sizeof(AStarNode *), compareNodes);
}

static void pushNodeToOpenList(AStarNode* node)
{
    if (nodeCount == MAX_NUM_ASTAR_NODES)
    {
#if 0
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                       "Queue is full!\n");
#endif
        return;
    }
    openList[nodeCount++] = node;
    sortOpenList();
}

static AStarNode *pop(void)
{
    if (nodeCount == 0)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_INFO, "Open list is empty.\n");
        return NULL;
    }

    AStarNode *root = openList[0];
    for (uint32_t i = 0; i < nodeCount - 1; i++)
    {
        openList[i] = openList[i + 1];
    }
    nodeCount--;
    return root;
}

static inline bool openListEmpty(void)
{
    return (nodeCount == 0);
}

static inline AStarNode *nodeFromWorldPos(Vec2f worldPos)
{
    AStarNode *result = NULL;
    int y = (int)(worldPos.y - closedList.topLeft.y);
    int x = (int)(worldPos.x - closedList.topLeft.x);

    if (x >= 0 && x < ASTAR_WIDTH && y >= 0 && y < ASTAR_HEIGHT)
    {
        result = &closedList.nodes[y * ASTAR_DIM + x];
    }
    return result;
}

static void printPath(AStarNode *node)
{
    printf("printing path:\n");
    int count = 0;
    while (node)
    {
        printf("Node %d = x: %f y: %f\n", count++, node->p.x, node->p.y);
        node = node->parent;
    }
}

static AStarNode *constructAStarPath(Entity *e, Vec2f start, Vec2f end)
{
    AStarNode *result = NULL;
    
    startNode = nodeFromWorldPos(start);
    startNode->parent = NULL;
    startNode->g = 0;
    startNode->h = sqDistance(start, end);
    startNode->f = startNode->g + startNode->h;
    startNode->p.x = start.x;
    startNode->p.y = start.y;
    startNode->visited = false;

    endNode = nodeFromWorldPos(end);
    endNode->parent = NULL;
    endNode->g = 0;
    endNode->h = 0;
    endNode->f = 0;
    endNode->p.x = end.x;
    endNode->p.y = end.y;
    endNode->visited = false;

    pushNodeToOpenList(startNode);

    while ((openListEmpty() == false))
    {
        AStarNode *node =  pop();
        if (node->visited == true)
        {
            continue;
        }
        node->visited = true;

        float sqDist = sqDistance(node->p, endNode->p);

        if (sqDist < 2.0f)
        {
            result = node;
            break;
        }

        int neighbourCount = 4;
        int deltaX[4] = {1, 0, 0, -1};
        int deltaY[4] = {0, 1, -1, 0};

        for (int i = 0; i < neighbourCount; i++)
        {
            int x = node->col + deltaX[i];
            int y = node->row + deltaY[i];

            if(x >= 0 && x < ASTAR_WIDTH && y >= 0 && y < ASTAR_WIDTH)
            {
                AStarNode *neighbour = &closedList.nodes[y * ASTAR_DIM + x];

                if (neighbour != NULL)
                {
                    Rect neighbourRect;
                    neighbourRect.x = neighbour->p.x;
                    neighbourRect.y = neighbour->p.y;
                    neighbourRect.w = e->width;
                    neighbourRect.h = e->height;

                    bool visited = neighbour->visited;
                    bool walkable = neighbour->walkable;
                    bool canMove = checkTileCollisions(neighbourRect);

                    if (!visited && walkable && canMove)
                    {
                        pushNodeToOpenList(neighbour);
                        float possiblyLowerGoal = node->g + sqDistance(neighbour->p, node->p);
                        if (possiblyLowerGoal < neighbour->g)
                        {
                            neighbour->parent = node;
                            neighbour->g = possiblyLowerGoal;
                            neighbour->f = neighbour->g + sqDistance(neighbour->p, end);
                        }
                        else
                        {

                        }
                    }
                    else
                    {

                    }
                }
                else
                {
                }
            }
            else
            {
            }
        }
    }
    return result;
}

Vec2f findPath(Entity *e, Vec2f end)
{   
    Vec2f result;
    result.x = INFINITY;
    result.y = INFINITY;

    Vec2f start = e->p;
    closedList.topLeft.x = start.x - (float)(ASTAR_WIDTH / 2);
    closedList.topLeft.y = start.y - (float)(ASTAR_HEIGHT / 2);

    resetData();

    AStarNode *node = NULL;
    node = constructAStarPath(e, start, end);
    if (node != NULL)
    {
        while (node->parent && node->parent != startNode)
        {
            node = node->parent;
        }

        int x = (int)(node->p.x);
        int y = (int)(node->p.y);

        MapTile *tile = getTileAtRowColLayerRaw(y, x, 1);
        tile->flags = 0;

        result.x = node->p.x;
        result.y = node->p.y;
    }
    else
    {
        memset(openList, 0, sizeof(AStarNode *) * MAX_NUM_ASTAR_NODES);
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
    bool success = true;

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

    openList = allocatePermanentMemory(sizeof(AStarNode *) * MAX_NUM_ASTAR_NODES);
    if (openList == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "Can not allocate a star open list memory!\n");
        success = false;
        return success;          
    }
    nodeCount = 0;

    initData();
    errorPrint = true;
    return success;
}
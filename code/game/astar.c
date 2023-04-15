#include "../common.h"

#include "../system/memory.h"
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

static void swap(uint32_t *node1, uint32_t *node2)
{
    if (!node1 || !node2)
    {
        return;
    }
    uint32_t temp = *node1;
    *node1 = *node2;
    *node2 = temp;
}

static float distance(Vec2f start, Vec2f end)
{
    return sqrt(pow(start.x - end.x, 2) + pow(start.y - end.y, 2));
}

static bool initData(void) 
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
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                       "Queue is full!\n");
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

static bool constructAStarPath(Vec2f start, Vec2f end)
{
    bool success = false;

    if(distance(start, end) > 10.0f)
    {
        return false;
    }

    uint32_t startIndex = (uint32_t)(start.y - closedList.topLeft.y) * ASTAR_DIM + (uint32_t)(start.x - closedList.topLeft.x);
    
    startNode = &closedList.nodes[startIndex];
    //add first node to the open list;
    startNode->parent = NULL;
    startNode->g = 0;
    startNode->h = distance(start, end);
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
            success = true;
            break;
        }

        for (int row = -1; row <= 1; row++)
        {
            for (int col = -1; col <= 1; col++)
            {
                int x = node->x + col;
                int y = node->y + row;

                if (x >= 0 && x < ASTAR_WIDTH && y >= 0 && y < ASTAR_HEIGHT)
                {
                    uint32_t neighbourIndex = y * ASTAR_WIDTH + x;
                    AStarNode *neighbour = &closedList.nodes[neighbourIndex];
                    if (neighbour->visited == false)
                    {
                        pushNodeToOpenList(neighbourIndex);
                        float possiblyLowerGoal = node->g + distance(neighbour->p, node->p);
                        if (possiblyLowerGoal < neighbour->g)
                        {
                            neighbour->parent = node;
                            neighbour->g = possiblyLowerGoal;
                            neighbour->f = neighbour->g + distance(neighbour->p, end);
                        }
                    }
                }
            }
        }
    }
    return success;
}

void showPath(Vec2f start, Vec2f end)
{   
    //convert positions to camera space
    start.x = start.x - dungeon.camera.x;
    start.y = start.y - dungeon.camera.y;

    end.x = end.x - dungeon.camera.x;
    end.y = end.y - dungeon.camera.y;

    closedList.topLeft.x = start.x - (float)(ASTAR_WIDTH / 2);
    closedList.topLeft.y = start.y - (float)(ASTAR_HEIGHT / 2);

    (void)initData();

    //construct astar path.
    if (constructAStarPath(start, end) == true)
    {
        printf("found path!\n");
    }
    else
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                       "Couldn't find a path from x: %f, y: %f to x: %f, y: %f\n", start.x, start.y, end.x, end.y);
        memset(openList, 0, sizeof(uint32_t) * MAX_NUM_ASTAR_NODES);
        memset(closedList.nodes, 0, sizeof(AStarNode) * ASTAR_DIM * ASTAR_DIM);
        closedList.topLeft.x = 0;
        closedList.topLeft.y = 0;
        nodeCount = 0;
        startNode = NULL;
        endNode = NULL;
    }
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

    success = initData();

    return success;
}
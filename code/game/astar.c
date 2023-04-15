#include "../common.h"

#include "../system/memory.h"
#include "astar.h"

#define ASTAR_DIM        32
#define ASTAR_WIDTH      ASTAR_DIM
#define ASTAR_HEIGHT     ASTAR_DIM

extern Dungeon dungeon;

static PriorityQueue openList;      //processing queue
static AStarGrid     closedList;   //permanent storage

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
    printf("swap!\n");
    if (!node1 || !node2)
    {
        printf("wtf\n");
        return;
    }
    uint32_t temp = *node1;
    *node1 = *node2;
    *node2 = temp;
}

static float distance(Vec2f start, Vec2f end)
{
    printf("distance!\n");
    return sqrtf(powf(start.x - end.x, 2) + powf(start.y - end.y, 2));
}

static bool initData(void) 
{
    printf("initdata!\n");
    memset(&openList, 0, sizeof(PriorityQueue));

    int row = 0;
    int col = 0;

    for (uint32_t row = 0; row < ASTAR_HEIGHT; row++)
    {
        for (uint32_t col = 0; col < ASTAR_WIDTH; col++)
        {
            uint32_t index = row * ASTAR_WIDTH + col;
            SDL_assert(index < MAX_NUM_ASTAR_NODES);
            AStarNode * node = closedList.nodes + index;

            node->p.x = closedList.topLeft.x + (float)col;
            node->p.y = closedList.topLeft.y + (float)row;
            node->f = INFINITY;
            node->g = INFINITY;
            node->h = INFINITY;
            node->parent = NULL;
            node->visited = false;
        }
    }

    startNode = NULL;
    endNode = NULL;

    return true;
}

static void heapify(uint32_t i)
{
    uint32_t smallest = i;
    uint32_t left = 2 * i + 1;
    uint32_t right = 2 * i + 2;
    uint32_t leftIndex = openList.count;
    uint32_t rightIndex = openList.count;

    if (left < openList.count)
    {
        uint32_t leftIndex = openList.queue[left];
    }
    if (right < openList.count)
    {
        uint32_t rightIndex = openList.queue[right];
    }

    uint32_t smallestIndex = openList.queue[smallest];

    if ((left < openList.count) && (closedList.nodes[leftIndex].f < closedList.nodes[smallestIndex].f))
    {
        smallest = left;
    }

    if ((right < openList.count) && (closedList.nodes[rightIndex].f < closedList.nodes[smallestIndex].f))
    {
        smallest = right;
    }
    printf("heapify smallest: %d i: %d!\n", smallest, i);
    if (smallest != i)
    {
        swap(&openList.queue[smallest], &openList.queue[i]);
        heapify(smallest);
    }
}

static void pushNodeToOpenList(uint32_t index)
{
    if (openList.count == MAX_NUM_ASTAR_NODES)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                       "Queue is full!\n");
        return;
    }

    uint32_t i = openList.count;
    openList.queue[i] = index;
    openList.count++;
    printf("count: %d\n", openList.count);
    while (i != 0 && (closedList.nodes[openList.queue[(i-1)/2]].f > closedList.nodes[openList.queue[i]].f))
    {
        swap(&openList.queue[i], &openList.queue[(i - 1)/2]);
        i = (i - 1)/2;
    }
}

static uint32_t pop(void)
{
    printf("pop!\n");
    if (openList.count == 0)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_INFO, "Open list is empty.\n");
        return UINT32_MAX;
    }

    uint32_t rootIndex = openList.queue[0];
    openList.queue[0] = openList.queue[openList.count - 1];
    openList.count--;
    heapify(0);
    return rootIndex;
}

static inline bool openListEmpty(void)
{
    printf("emtpy!\n");
    return (openList.count == 0);
}

static inline bool reachedGoal(Vec2f start, Vec2f end)
{
    printf("reachedgoal!\n");
    return (distance(start, end) < 1.0f);
}

static bool constructAStarPath(Vec2f start, Vec2f end)
{
    bool success = false;

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
    
    if (endIndex < MAX_NUM_ASTAR_NODES)
    {
        endNode = &closedList.nodes[endIndex];
    }
    else
    {
        //logging
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "end out of bounds!\n");
        return success;
    }

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

        //get node row and column
        int nodeX = (int)node->p.x - closedList.topLeft.x;
        int nodeY = (int)node->p.y - closedList.topLeft.y;

        for (int row = -1; row <= 1; row++)
        {
            for (int col = -1; col <= 1; col++)
            {
                int x = nodeX + col;
                int y = nodeY + row;

                if (x >= 0 && x < ASTAR_WIDTH && y >= 0 && y < ASTAR_HEIGHT)
                {
                    uint32_t neighbourIndex = y * ASTAR_WIDTH + x;
                    AStarNode *neighbour = &closedList.nodes[neighbourIndex];
                    if (neighbour->visited == false)
                    {
                        printf("neighbourIndex: %d\n", neighbourIndex);
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
        AStarNode *node = endNode;
    }
    else
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                       "Couldn't find a path from x: %f, y: %f to x: %f, y: %f\n", start.x, start.y, end.x, end.y);
    }
}

bool initAStar(void)
{
    bool success;

    closedList.nodes = allocatePermanentMemory(sizeof(AStarNode) * MAX_NUM_ASTAR_NODES);
    if(closedList.nodes == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "Can not allocate a star closed list memory!\n");
        success = false;
        return success;        
    }
    Vec2f topLeft = {0.0f, 0.0f};
    closedList.topLeft = topLeft;
    success = initData();

    return success;
}
#ifndef ASTAR_H_
#define ASTAR_H_

typedef struct AStarNode 
{
    struct AStarNode * parent;
    Vec2f    p;
    uint32_t row;
    uint32_t col;
    float    g; //cost of moving from start to here
    float    f; //total cost
    float    h; //cost of moving to the end(heuristic)
    bool  visited;
    bool  walkable;
}AStarNode;

typedef struct AStarGrid
{
    Vec2f     topLeft;
    AStarNode *nodes;
}AStarGrid;

typedef struct
{
    uint32_t  queue[MAX_NUM_ASTAR_NODES];
    uint32_t  count;
}PriorityQueue;

bool      initAStar(void);
Vec2f     findPath(Entity *e, Vec2f end);
bool      resetAStar(void);
AStarNode *getEndNode(void);

#endif
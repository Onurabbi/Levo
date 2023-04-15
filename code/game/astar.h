#ifndef ASTAR_H_
#define ASTAR_H_

typedef struct AStarNode 
{
    Vec2f p;
    float g; //cost of moving from start to here
    float f; //total cost
    float h; //cost of moving to the end(heuristic)
    bool  visited;
    struct AStarNode * parent;
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
void      showPath(Vec2f start, Vec2f end);
AStarNode *getEndNode(void);

#endif
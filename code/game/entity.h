#ifndef ENTITY_H_
#define ENTITY_H_

#define ENTITY_IS_ALIVE_BIT (0)
#define ENTITY_CAN_UPDATE_BIT (1)
#define ENTITY_CAN_COLLIDE_BIT (2)
#define ENTITY_IS_VISIBLE_BIT (3)

bool  isEntityAlive(Entity * entity);
bool  isEntityVisible(Entity *entity);

bool  initEntities(void);
void  updateEntities(void);
void  addEntityToUpdateList(uint32_t index, uint32_t thread);
void  moveEntity(Entity *e, float dx, float dy);
void  resetEntityUpdates(void);
void  denselyPackEntities(void);

void  die(Entity *entity);

SimulationRegion getSimulationRegion(void);

#endif
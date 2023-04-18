#ifndef ENTITY_FACTORY_H_
#define ENTITY_FACTORY_H_

bool     initEntityFactory(Dungeon *dungeon);
Entity * initEntity(char *name);
void     removeEntity(Entity * e);
void     removeActor(Actor *a);
#endif
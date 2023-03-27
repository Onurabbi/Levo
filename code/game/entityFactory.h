#ifndef ENTITY_FACTORY_H_
#define ENTITY_FACTORY_H_

bool     initEntityFactory(void);
Entity * initEntity(char *name);
void     removeEntity(Entity * e);

#endif
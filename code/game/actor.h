#ifndef ACTOR_H_
#define ACTOR_H_

void updateActor(Actor *actor); 
void takeDamage(Actor * actor, Actor * attacker, uint32_t damage);
void removeActor(Actor *actor);

#endif
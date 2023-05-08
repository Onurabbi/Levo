#ifndef ACTOR_H_
#define ACTOR_H_

void updateActor(Actor *actor, ActorController controller);
void attack(Actor *actor);

void takeDamage(Actor * actor, Actor *attacker, uint32_t damage);
void removeActor(Actor *actor);

bool isActorAttacking(Actor *actor);
bool canActorMove(Actor *actor);

#endif
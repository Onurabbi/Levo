#ifndef ANIMATION_H_
#define ANIMATION_H_

Animation      *getAnimationByIndex(uint32_t index);
AnimationGroup *getAnimationGroupByIndex(uint32_t index);

uint32_t getAnimationIndex(char * fileName);
uint32_t getAnimationGroupIndex(char *fileName);

Animation      *getAnimation(char * fileName);
AnimationGroup *getAnimationGroup(char *fileName);

Animation *createAnimation(char * fileName, int * new);
bool       initAnimations(void);
bool       addAnimationGroupToAnimationController(char *filePath, 
                                                  AnimationController *controller, 
                                                  char *animationSlot);

bool isAnimationInMovableState(AnimationController *animController);
void updateAnimation(EntityVisibleSprites *entitySprites, AnimationController *animController, ActorMovement movement);

#endif
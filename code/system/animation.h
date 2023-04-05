#ifndef ANIMATION_H_
#define ANIMATION_H_

AnimationGroup * getAnimationGroupByIndex(uint32_t index);
uint32_t         getAnimationGroupIndex(char * fileName);
AnimationGroup * getAnimationGroup(char * fileName);
AnimationGroup * createAnimationGroup(char * fileName, int * new);
bool             initAnimations(void);
void             addAnimationToEntityAnimGroup(Entity *e, AnimationGroup *animGroup, char *path);
#endif
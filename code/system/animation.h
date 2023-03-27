#ifndef ANIMATION_H_
#define ANIMATION_H_

uint32_t    getAnimationIndex(char * fileName);
Animation * getAnimation(char * fileName);
bool        initAnimations(void);

#endif
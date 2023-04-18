#ifndef DRAW_H_
#define DRAW_H_

bool       initDraw(void);
void       addEntityToDrawList(Entity * entity, Vec2f p, int thread);
void       addTileToDrawList(MapTile * tile, Vec2f p, int thread);
void       prepareScene(void);
void       presentScene(void);
void       blit(SDL_Texture * texture, int x, int y, bool center);
void       blitSprite(Sprite * sprite, int x, int y, bool center, SDL_RendererFlip flip);
void       debugDrawRect(Vec2f topLeft, float width, float height, int r, int g, int b, int thread);
void       drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
void       drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
void       drawAll(void);
void       resetDraw(void);
void       denselyPackDrawableTiles(void);
void       denselyPackAndSortDrawableEntities(void);
#endif

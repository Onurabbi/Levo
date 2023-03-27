#ifndef TEXT_H_
#define TEXT_H_

bool         initFonts(void);
void         drawText(char *text, int x, int y, int r, int g, int b, int align, int maxWidth);
int          getWrappedTextHeight(char *text, int maxWidth);
void         calcTextDimensions(char *text, int *w, int *h);

#endif
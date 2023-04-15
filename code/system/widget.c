#include "SDL2/SDL_mixer.h"

#include "../common.h"

#include "../json/cJSON.h"

#include "utils.h"
#include "input.h"
#include "draw.h"
#include "sound.h"
#include "text.h"
#include "widget.h"

extern App app;

static Widget widgets[MAX_NUM_WIDGETS];
static uint32_t numWidgets;

static SliderWidget sliderWidgets[MAX_NUM_SLIDER_WIDGETS];
static uint32_t     numSliderWidgets;

static double   sliderDelay;
static double   cursorBlink;
static bool     showCursor;

#define SLIDER_WIDGET_OFFSET_PX 50

static void drawSliderWidget(Widget * w, bool active)
{
    SDL_Color      c;
    SliderWidget * s;

    double          width;

    s = (SliderWidget *)w->data;

    if(active == true)
    {
        c.r = 160;
        c.g = 200;
        c.b = 255;
    }
    else
    {
        c.r = 255;
        c.g = 255;
        c.b = 255;
    }
    
    width = (1.0 * s->value) / 100;
    drawText(w->label, w->rect.x, w->rect.y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
    drawRect(s->rect.x + 2, s->rect.y + 2, (s->rect.w - 4) * width, s->rect.h - 4, c.r, c.g, c.b, 255);
    drawOutlineRect(s->rect.x, s->rect.y, s->rect.w, s->rect.h, 255, 255, 255, 255);
}

static void drawButtonWidget(Widget * w, bool active)
{
    SDL_Color c;

    if(active == true)
    {
        c.r = 160;
        c.g = 200;
        c.b = 255;
    }
    else
    {
        c.r = 255;
        c.g = 255;
        c.b = 255;
    }

    drawText(w->label, w->rect.x, w->rect.y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
}

void drawWidgets(char *groupName)
{
    drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 160);
    app.fontScale = 2;
    for(int i = 0; i < numWidgets; i++)
    {
        Widget * w = &widgets[i];

        if(strcmp(w->groupName, groupName) == 0)
        {
            bool active = (i == app.activeWidgetIndex);

            switch(w->type)
            {
                case WT_BUTTON:
                    drawButtonWidget(w, active);
                    break;
                case WT_SLIDER:
                    drawSliderWidget(w, active);
                    break;
                default:
                    break;
            }
            if ((active == true) && (showCursor == true))
            {
                int h = w->rect.h / 2;
                drawRect(w->rect.x - (h * 2), w->rect.y + h/2, h, h, 160, 200, 255, 255);
            }
        }
    }
    app.fontScale = 1;
}

/*
    TODO: I'll just blindly search the whole widgets array every single time for now.
          Make this a hash table if it's a performance concern
*/
uint32_t getWidgetIndex(char * name, char * groupName)
{
    for(uint32_t i = 0; i < numWidgets; i++)
    {
        Widget * w = widgets + i;

        if((strcmp(w->name, name) == 0) && (strcmp(w->groupName, groupName) == 0))
        {
            return i;
        }
    }
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "No such widget: name='%s', groupName='%s'", name, groupName);
    return MAX_NUM_WIDGETS;
}

static void createSliderWidget(Widget * w, cJSON * root)
{
    SDL_assert(numSliderWidgets < MAX_NUM_SLIDER_WIDGETS);

    SliderWidget * s = &sliderWidgets[numSliderWidgets++];
    w->data = (void *)s;

    s->step = cJSON_GetObjectItem(root, "step")->valueint;
    s->waitOnChange = cJSON_GetObjectItem(root, "waitOnChange")->valueint;

    calcTextDimensions(w->label, &w->rect.w, &w->rect.h);

    s->rect.x = SCREEN_WIDTH/2;
    s->rect.y = w->rect.y;
    s->rect.w = w->rect.w;
    s->rect.h = w->rect.h;
}

static void createButtonWidget(Widget * w)
{
    calcTextDimensions(w->label, &w->rect.w, &w->rect.h);
}

static int getWidgetType(char * type)
{
    int widgetType;

    if(strcmp(type, "WT_BUTTON") == 0)
    {
        widgetType = WT_BUTTON;
    }
    else if(strcmp(type, "WT_SLIDER") == 0)
    {
        widgetType = WT_SLIDER;
    }
    else
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Unknown widget type %s!", type);
        widgetType = WT_MAX;
    }

    return widgetType;
}

static void createWidget(cJSON *root)
{
    SDL_assert(numWidgets < MAX_NUM_WIDGETS);

	Widget *w;
	int     type;

	type = getWidgetType(cJSON_GetObjectItem(root, "type")->valuestring);

	if (type != -1)
	{
		w = &widgets[numWidgets++];

		STRCPY(w->name, cJSON_GetObjectItem(root, "name")->valuestring);
		STRCPY(w->groupName, cJSON_GetObjectItem(root, "groupName")->valuestring);
		STRCPY(w->label, cJSON_GetObjectItem(root, "label")->valuestring);
		w->type = getWidgetType(cJSON_GetObjectItem(root, "type")->valuestring);
		w->rect.x = cJSON_GetObjectItem(root, "x")->valueint;
		w->rect.y = cJSON_GetObjectItem(root, "y")->valueint;

		switch (w->type)
		{
			case WT_BUTTON:
				createButtonWidget(w);
				break;

			case WT_SLIDER:
				createSliderWidget(w, root);
				break;

			default:
				break;
		}
	}
}

static bool loadWidgets(char *fileName)
{
    cJSON *root = NULL;
    cJSON *node = NULL;
    char  *text = NULL;

    app.fontScale = 2;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s ...", fileName);

    text = readFile(fileName);

    if(text == NULL)
    {
        return false;
    }

    root = cJSON_Parse(text);

    if(root == NULL)
    {
        free(text);
        return false;
    }

    for(node = root->child; node != NULL; node = node->next)
    {
        createWidget(node);
    }

    cJSON_Delete(root);

    free(text);

    app.fontScale = 1;

    return true;
}

static void music()
{

}

static void sound()
{

}

static void resume()
{
    clearInput(&app.input);
    app.gameMode = GAME_MODE_DUNGEON;
    playSound(SND_GUI, 0);
}

static void quit()
{
    app.run = false;
}

static void setupWidgets(void)
{
    int      index;
    Widget * w;
    SliderWidget * sw;

    index = getWidgetIndex("resume", "pause");
    app.activeWidgetIndex = index;

    w = &widgets[index];
    w->action = resume;

    index = getWidgetIndex("sound", "pause");
    w = &widgets[index];
    w->action = sound;
    sw = (SliderWidget *)w->data;
    sw->rect.x = w->rect.x + 175;
    sw->rect.y = w->rect.y + 16;
    sw->rect.w = 350;
    sw->rect.h = 32;
    sw->value = ((1.0 * app.soundVolume)/MIX_MAX_VOLUME) * 100;

    index = getWidgetIndex("music", "pause");
    w = &widgets[index];
	w->action = music;
	sw = (SliderWidget *)w->data;
	sw->rect.x = w->rect.x + 175;
	sw->rect.y = w->rect.y + 16;
	sw->rect.w = 350;
	sw->rect.h = 32;
	sw->value = ((1.0 * app.musicVolume) / MIX_MAX_VOLUME) * 100;

    index = getWidgetIndex("quit", "pause");
    w = &widgets[index];
    w->action = quit;
}

bool initWidgets(void)
{
    memset(widgets, 0, sizeof(widgets));
    numWidgets = 0;
    memset(sliderWidgets, 0, sizeof(sliderWidgets));
    numSliderWidgets = 0;

    if(loadWidgets("../data/pause.json") == false)
    {
        return false;
    }

    setupWidgets();

    return true;
}

static inline uint32_t decrementWidgetIndex(int index)
{
    uint32_t result = index - 1;

    if(result < 0)
    {
        result = numWidgets - 1;
    }

    return result;
}

static inline uint32_t incrementWidgetIndex(int index)
{
    uint32_t result = index + 1;

    if (result == numWidgets)
    {
        result = 0;
    }

    return result;
}

void update(void)
{
    if (cursorBlink > CURSOR_BLINK_TIME_SEC)
    {
        showCursor = !showCursor;
        cursorBlink = 0.0;
    }

    if (app.input.keyboardEvents[SDL_SCANCODE_W] == 1)
    {
        app.input.keyboardEvents[SDL_SCANCODE_W] = 0;

        app.activeWidgetIndex = decrementWidgetIndex(app.activeWidgetIndex);

        playSound(SND_GUI, 0);
    }

    if (app.input.keyboardEvents[SDL_SCANCODE_S] == 1)
    {
        app.input.keyboardEvents[SDL_SCANCODE_S] = 0;

        app.activeWidgetIndex = incrementWidgetIndex(app.activeWidgetIndex);

        playSound(SND_GUI, 0);
    }

    if(app.input.keyboardEvents[SDL_SCANCODE_SPACE] == 1)
    {
        widgets[app.activeWidgetIndex].action();
    }

    clearInput(&app.input);
}

void renderMenu(void)
{
    drawWidgets("pause");
}

void updateMenu(double secElapsed)
{
    cursorBlink += secElapsed;
    update();
}


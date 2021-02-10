#ifndef ENTICE_WIN_H
#define ENTICE_WIN_H

typedef struct Entice Entice;

struct Entice
{
    /* window content */
    Evas_Object *conform;
    Evas_Object *layout;
    Evas_Object *event_mouse;
    Evas_Object *event_kbd;
    Eina_Bool focused;

    /* list of opened documents */
    Eina_List *docs;

    /* theme */
    Entice_Config *config;
    char *theme_file;
};

Evas_Object *entice_win_add(void);

#endif /* ENTICE_WIN_H */

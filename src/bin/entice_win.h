/* Entice - small and simple image viewer using the EFL
 * Copyright (C) 2021 Vincent Torri
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ENTICE_WIN_H
#define ENTICE_WIN_H

typedef struct Entice Entice;

struct Entice
{
    /* window content */
    Evas_Object *bg;
    Evas_Object *scroller;
    Evas_Object *layout;
    Evas_Object *image;
    Evas_Object *event_mouse;
    Evas_Object *event_kbd;
    Eina_Bool focused;

    /* list of opened images */
    Eina_List *images;
    /* current displayed image */
    Eina_List *image_current;

    /* gui */
    Evas_Object *prev;      /* previous image */
    Evas_Object *next;      /* next image */
    Evas_Object *rotleft;   /* rotate left (counter clock) */
    Evas_Object *rotright;  /* rotate right (clock) */
    Evas_Object *zoomin;    /* zoom in */
    Evas_Object *zoomval;   /* button for zoom value */
    Evas_Object *zoomout;   /* zoom out */
    Evas_Object *bestfit;   /* checkox for best fit */
    Evas_Object *hover_zoom;

    Evas_Object *menu;      /* menu icon */
    Evas_Object *fullscreen;/* fullscreen icon */
    Evas_Object *close;     /* close icon */
    Evas_Object *hover_menu;
    Ecore_Timer *controls_timer;
    Ecore_Timer *settings_timer;
    Ecore_Timer *exif_timer;
    Eina_Bool settings_created;
    Eina_Bool settings_shown;
    Eina_Bool controls_shown;
    Eina_Bool controls_over;
    Eina_Bool exif_created;
    Eina_Bool exif_shown;

    /* theme */
    Entice_Config *config;
    char *theme_file;
};

Evas_Object *entice_win_add(void);

void entice_win_title_update(Evas_Object *win);

void entice_win_images_set(Evas_Object *win, Eina_List *images);

void entice_win_image_first_set(Evas_Object *win, Eina_List *first);

void entice_win_fullscreen_toggle(Evas_Object *win);

#endif /* ENTICE_WIN_H */

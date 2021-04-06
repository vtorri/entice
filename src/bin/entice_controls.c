/* Entice - small and simple image viewer using the EFL
 * Copyright (C) 2021 Vincent Torro
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

#include <config.h>

#include <Elementary.h>

#include "entice_config.h"
#include "entice_image.h"
#include "entice_settings.h"
#include "entice_win.h"
#include "entice_controls.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

#define CONTROLS(_icon, _action)                                        \
    o = elm_icon_add(win);                                              \
    elm_icon_standard_set(o, _icon);                                    \
    evas_object_show(o);                                                \
    entice->_action = o;                                                \
                                                                        \
    o = elm_button_add(win);                                            \
    elm_object_content_set(o, entice->_action);                         \
    elm_object_focus_allow_set(o, EINA_FALSE);                          \
    elm_object_style_set(o, "overlay");                                 \
    evas_object_show(o);                                                \
    elm_object_part_content_set(entice->layout, "entice." #_action, o); \
                                                                        \
    elm_layout_signal_callback_add(entice->layout,                      \
                                   "image,action," #_action, "entice",  \
                                   _cb_image_##_action, win)

static void
_cb_image_prev(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;
    Eina_List *next;

    entice = evas_object_data_get(win, "entice");
    next = eina_list_prev(entice->image_current);
    if (next)
    {
        entice_image_set(entice->image, next);
    }
}

static void
_cb_image_next(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;
    Eina_List *next;

    entice = evas_object_data_get(win, "entice");
    next = eina_list_next(entice->image_current);
    if (next)
    {
        entice_image_set(entice->image, next);
    }
}

static void
_cb_image_rotleft(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice_image_rotate(entice->image, 3);
}

static void
_cb_image_rotright(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice_image_rotate(entice->image, 1);
}

static void
_cb_image_zoomin(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    //entice_image_rotate(entice->image, 1);
    printf("zoom in \n");
    fflush(stdout);
}

static void
_cb_image_zoomout(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    //entice_image_rotate(entice->image, 1);
    printf("zoom out \n");
    fflush(stdout);
}

static void
_cb_image_zoomorig(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    //entice_image_rotate(entice->image, 1);
    printf("zoom 1:1 \n");
    fflush(stdout);
}

static void
_cb_image_zoomfit(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    //entice_image_rotate(entice->image, 1);
    printf("zoom fit \n");
    fflush(stdout);
}

static void
_cb_image_close(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    evas_object_del(win);
}

static void
_cb_image_fullscreen(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    elm_win_fullscreen_set(win, !elm_win_fullscreen_get(win));
}

static void
_cb_image_settings(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice_settings_init(win);
    if (!entice->settings_shown)
    {
        elm_object_signal_emit(entice->layout, "state,settings,show", "entice");
        entice->settings_shown = EINA_TRUE;
    }
    else
    {
        elm_object_signal_emit(entice->layout, "state,settings,hide", "entice");
        entice->settings_shown = EINA_FALSE;
    }
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

void
entice_controls_init(Evas_Object *win)
{
    Entice *entice;
    Evas_Object *o;

    entice = evas_object_data_get(win, "entice");

    CONTROLS("zoom-fit-best", zoomfit);
    CONTROLS("zoom-original", zoomorig);
    CONTROLS("zoom-out", zoomout);
    CONTROLS("zoom-in", zoomin);
    CONTROLS("object-rotate-left", rotleft);
    CONTROLS("object-rotate-right", rotright);
    CONTROLS("go-previous", prev);
    CONTROLS("go-next", next);

    o = elm_check_add(win);
    elm_object_style_set(o, "default");
    evas_object_show(o);
    entice->zoomcheck = o;

    elm_object_part_content_set(entice->layout, "entice.zoomcheck", entice->zoomcheck);

    /* zoom entry */
    o = elm_entry_add(win);
    elm_scroller_policy_set(o, ELM_SCROLLER_POLICY_OFF,
                            ELM_SCROLLER_POLICY_OFF);
    elm_object_text_set(o, "2000%");
    elm_entry_single_line_set(o, EINA_TRUE);
    //evas_object_smart_callback_add(o, "activated", _entry_activated_cb, NULL);
    evas_object_show(o);
    entice->zoomval = o;

    elm_object_part_content_set(entice->layout, "entice.zoomval", entice->zoomval);

    CONTROLS("preferences-system", settings);
    CONTROLS("view-fullscreen", fullscreen);
    CONTROLS("window-close", close);
}

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

#include "entice_private.h"
#include "entice_config.h"
#include "entice_controls.h"
#include "entice_theme.h"
#include "entice_image.h"
#include "entice_key.h"
#include "entice_win.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

static void
_cb_mouse_down(void *data, Evas *evas_UNUSED, Evas_Object *obj_UNUSED, void *event_info);

static void
_cb_key_down(void *data,
             Evas *evas EINA_UNUSED,
             Evas_Object *obj EINA_UNUSED,
             void *event_info)
{
    EINA_SAFETY_ON_NULL_RETURN(event_info);

    entice_key_handle(data, event_info);
}

static void
_cb_win_del(void *data EINA_UNUSED,
            Evas *_e EINA_UNUSED,
            Evas_Object *win,
            void *_event EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");

    /* FIXME: free images */
    evas_object_data_del(win, "entice");

    evas_object_event_callback_del_full(entice->event_mouse,
                                        EVAS_CALLBACK_MOUSE_DOWN,
                                        _cb_mouse_down, win);

    entice_config_del(entice->config);
    free(entice);
}

static void
_cb_win_resize(void *data EINA_UNUSED,
            Evas *_e EINA_UNUSED,
            Evas_Object *win,
            void *_event EINA_UNUSED)
{
    Entice *entice;
    Entice_Image_Prop *prop;

    entice = evas_object_data_get(win, "entice");
    if (!entice || !entice->image_current)
        return;

    prop = eina_list_data_get(entice->image_current);
    if (!prop)
        return;

    switch(prop->zoom_mode)
    {
        case ENTICE_ZOOM_MODE_NORMAL:
            entice_image_current_zoom(win, 1.0);
            break;
        case ENTICE_ZOOM_MODE_FIT:
            entice_image_current_zoom_fit(win);
            break;
    }
}

static void
_cb_fullscreen(void *data EINA_UNUSED, Evas_Object *win, void *event EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    elm_layout_signal_emit(entice->layout, "state,win,fullscreen", "entice");
    elm_win_noblank_set(win, EINA_TRUE);
}

static void
_cb_unfullscreen(void *data EINA_UNUSED, Evas_Object *win, void *event EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    if (!elm_win_fullscreen_get(win))
    {
        elm_layout_signal_emit(entice->layout, "state,win,normal", "entice");
        elm_win_noblank_set(win, EINA_FALSE);
     }
}

static void
_cb_focused(void *data EINA_UNUSED, Evas_Object *win, void *event EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    elm_layout_signal_emit(entice->layout, "state,win,focused", "entice");
}

static void
_cb_unfocused(void *data EINA_UNUSED, Evas_Object *win, void *event EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    elm_layout_signal_emit(entice->layout, "state,win,unfocused", "entice");
}

static void
_cb_mouse_down(void *win, Evas *evas EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
    Evas_Event_Mouse_Down *ev;

    ev = (Evas_Event_Mouse_Down *)event_info;

    if (ev->button != 1) return;
    if (ev->flags & EVAS_BUTTON_DOUBLE_CLICK)
    {
        elm_win_fullscreen_set(win, !elm_win_fullscreen_get(win));
    }
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

Evas_Object *
entice_win_add(void)
{
    Evas_Object *win;
    Evas_Object *o;
    Entice *entice;

    entice = (Entice *)calloc(1, sizeof(Entice));
    if (!entice)
    {
        ERR(_("could not allocate memory for Entice struct"));
        return NULL;
    }

    entice->config = entice_config_load("config");
    if (!entice->config)
    {
        ERR(_("could not load \"config\" configuration."));
        free(entice);
        return NULL;
    }
    elm_theme_overlay_add(NULL, entice_config_theme_path_default_get(entice->config));
    elm_theme_overlay_add(NULL, entice_config_theme_path_get(entice->config));

    /* main window */
    win = elm_win_add(NULL, "Entice", ELM_WIN_BASIC);
    if (!win)
    {
        ERR(_("could not create elm window."));
        entice_config_del(entice->config);
        free(entice);
        return NULL;
    }

    elm_win_title_set(win, "Entice");
    /* TODO: icon name */
    elm_win_autodel_set(win, EINA_TRUE);
    elm_win_focus_highlight_enabled_set(win, EINA_TRUE);

    evas_object_data_set(win, "entice", entice);

    evas_object_event_callback_add(win, EVAS_CALLBACK_DEL, _cb_win_del, entice);
    evas_object_event_callback_add(win, EVAS_CALLBACK_RESIZE, _cb_win_resize, entice);

    evas_object_smart_callback_add(win, "fullscreen", _cb_fullscreen, NULL);
    evas_object_smart_callback_add(win, "unfullscreen", _cb_unfullscreen, NULL);
    evas_object_smart_callback_add(win, "focused", _cb_focused, entice);
    evas_object_smart_callback_add(win, "unfocused", _cb_unfocused, entice);

    /* conformant */
    o = elm_conformant_add(win);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_fill_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_win_resize_object_add(win, o);
    evas_object_show(o);
    entice->conform = o;

    /* scroller */
    o = elm_scroller_add(win);
    elm_scroller_policy_set(o,
                            ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);
    elm_scroller_bounce_set(o, EINA_TRUE, EINA_TRUE);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(o);
    entice->scroller = o;

    /* table */
    o = elm_table_add(win);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_content_set(entice->scroller, o);
    evas_object_show(o);
    entice->table = o;

    /* gui layout */
    o = elm_layout_add(win);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_fill_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_content_set(entice->conform, o);
    evas_object_repeat_events_set(o, EINA_FALSE);
    evas_object_show(o);
    entice->layout = o;
    if (!entice_theme_apply(win, "entice/core"))
    {
        ERR(_("could not apply the theme."));
        _cb_win_del(NULL, NULL, win, NULL);
        return NULL;
    }
    entice_controls_init(win);

    /* image */
    o = evas_object_image_add(evas_object_evas_get(win));
    evas_object_image_filled_set(o, EINA_TRUE);
    elm_object_part_content_set(entice->layout, "entice.image",
                                entice->scroller);
    elm_table_pack(entice->table, o, 0, 0, 1, 1);
    evas_object_show(o);
    entice->image = o;

    /* dummy button to catch mouse events */
    o = elm_button_add(win);
    elm_object_focus_allow_set(o, EINA_FALSE);
    elm_object_focus_move_policy_set(o, ELM_FOCUS_MOVE_POLICY_CLICK);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win, o);
    evas_object_color_set(o, 0, 0, 0, 0);
    evas_object_repeat_events_set(o, EINA_TRUE);
    evas_object_show(o);
    evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
                                   _cb_mouse_down, win);
    entice->event_mouse = o;

    /* dummy button to catch keyboard events */
    o = elm_button_add(win);
    elm_object_focus_highlight_style_set(o, "blank");
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win, o);
    evas_object_lower(o);
    evas_object_show(o);
    elm_object_focus_set(o, EINA_TRUE);
    evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN,
                                   _cb_key_down, win);
    entice->event_kbd = o;

    return win;
}

void
entice_win_images_set(Evas_Object *win, Eina_List *images)
{
    Entice *entice;

    if (!images)
        return;

    entice = evas_object_data_get(win, "entice");
    entice->images = images;
}

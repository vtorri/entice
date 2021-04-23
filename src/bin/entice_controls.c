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

#include <config.h>

#include <Elementary.h>

#include "entice_config.h"
#include "entice_image.h"
#include "entice_settings.h"
#include "entice_exif.h"
#include "entice_win.h"
#include "entice_controls.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

#define CONTROLS(_icon, _action)                                          \
    o = elm_icon_add(win);                                                \
    elm_icon_standard_set(o, _icon);                                      \
    evas_object_show(o);                                                  \
    entice->_action = o;                                                  \
                                                                          \
    o = elm_button_add(win);                                              \
    elm_object_content_set(o, entice->_action);                           \
    elm_object_focus_allow_set(o, EINA_FALSE);                            \
    elm_object_style_set(o, "overlay");                                   \
    evas_object_show(o);                                                  \
    elm_object_part_content_set(entice->layout, "entice." #_action, o);   \
                                                                          \
    elm_layout_signal_callback_add(entice->layout,                        \
                                   "image,action," #_action, "entice",    \
                                   _entice_ctrl_ ## _action ##_cb, win);  \
                                                                          \
    elm_layout_signal_callback_add(entice->layout,                        \
                                   "image,startfade," #_action, "entice", \
                                   _entice_ctrl_startfade_cb, win);       \
                                                                          \
    elm_layout_signal_callback_add(entice->layout,                        \
                                   "image,stopfade," #_action, "entice",  \
                                   _entice_ctrl_stopfade_cb, win)

#define CONTROL_ZOOM_CB(_zm)                                        \
    static void                                                     \
    _entice_ctrl_zoom_ ## _zm ## _cb(void *data,                    \
                                     Evas_Object *obj,              \
                                     void *event_info EINA_UNUSED)  \
    {                                                               \
        Entice *entice;                                             \
        Elm_Object_Item *list_it;                                   \
        list_it = elm_list_selected_item_get(obj);                  \
        elm_list_item_selected_set(list_it, EINA_FALSE);            \
        entice = (Entice *)data;                                    \
        entice_image_zoom_set(entice->image, _zm);                  \
        entice_image_update(entice->image);                         \
        elm_hover_dismiss(entice->hover_zoom);                      \
    }

static void
_entice_ctrl_zoomout_cb(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice_image_zoom_decrease(entice->image);
    entice_image_update(entice->image);
}

static void
_entice_ctrl_zoomin_cb(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice_image_zoom_increase(entice->image);
    entice_image_update(entice->image);
}

static void
_entice_ctrl_rotleft_cb(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice_image_rotate(entice->image, 3);
}

static void
_entice_ctrl_rotright_cb(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice_image_rotate(entice->image, 1);
}

static void
_entice_ctrl_prev_cb(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
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
_entice_ctrl_next_cb(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
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

CONTROL_ZOOM_CB(2000)
CONTROL_ZOOM_CB(1500)
CONTROL_ZOOM_CB(1000)
CONTROL_ZOOM_CB(500)
CONTROL_ZOOM_CB(200)
CONTROL_ZOOM_CB(133)
CONTROL_ZOOM_CB(100)
CONTROL_ZOOM_CB(50)
CONTROL_ZOOM_CB(33)

static void
_entice_ctrl_zoomcheck_cb(void *win, Evas_Object *obj, void *event_info EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");

    if (elm_check_state_get(obj) == EINA_TRUE)
    {
        entice_image_zoom_mode_set(entice->image, ENTICE_ZOOM_MODE_FIT);
        entice_image_update(entice->image);
    }
    else
    {
        entice_image_zoom_mode_set(entice->image, ENTICE_ZOOM_MODE_NORMAL);
        entice_image_update(entice->image);
    }
}

static void
_entice_ctrl_zoomval_cb(void *win, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
    char buf[16];
    Entice *entice;
    Evas_Object *o;

    printf(" *** zoomval cb\n");
    fflush(stdout);
    entice = evas_object_data_get(win, "entice");

    o = elm_button_add(win);
    elm_object_style_set(o, "overlay");
    snprintf(buf, sizeof(buf), "%d%%", entice_image_zoom_get(entice->image));
    elm_object_text_set(o, buf);
    evas_object_show(o);
    elm_object_part_content_set(entice->hover_zoom, "middle", o);

    evas_object_show(entice->hover_zoom);
}

static void
_entice_ctrl_stopfade_cb(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice->controls_over = EINA_TRUE;
    printf("mouse in\n");
    fflush(stdout);

    if (entice->controls_timer)
    {
        ecore_timer_del(entice->controls_timer);
        entice->controls_timer = NULL;
    }
}

static void
_entice_ctrl_startfade_cb(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice->controls_over = EINA_FALSE;
    //entice_controls_timer_start(win);
    printf("mouse out\n");
    fflush(stdout);
}

static Eina_Bool
_entice_ctrl_hide_cb(void *win)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");

    entice->controls_timer = NULL;

    /* hide controls */
    if (entice->controls_shown)
    {
        elm_object_signal_emit(entice->layout, "state,controls,hide", "entice");
        entice->controls_shown = EINA_FALSE;
    }

    return EINA_FALSE;
}

static void
_entice_ctrl_close_cb(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    evas_object_del(win);
}

static void
_entice_ctrl_fullscreen_cb(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    entice_win_fullscreen_toggle(win);
}

static void
_entice_ctrl_menu_cb(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    evas_object_show(entice->hover_menu);
}

static void
_entice_ctrl_menu_settings_cb(void *win,
                              Evas_Object *obj,
                              void *event_info EINA_UNUSED)
{
    Entice *entice;
    Elm_Object_Item *list_it;

    list_it = elm_list_selected_item_get(obj);
    elm_list_item_selected_set(list_it, EINA_FALSE);

    entice = evas_object_data_get(win, "entice");

    entice_settings_init(win);

    if (entice->exif_shown)
    {
        elm_object_signal_emit(entice->layout, "state,exif,hide", "entice");
        entice->exif_shown = EINA_FALSE;
    }

    if (!entice->settings_shown)
    {
        elm_object_signal_emit(entice->layout, "state,settings,show", "entice");
        entice->settings_shown = EINA_TRUE;
    }

    elm_hover_dismiss(entice->hover_menu);
}

static void
_entice_ctrl_menu_exif_cb(void *win,
                          Evas_Object *obj,
                          void *event_info EINA_UNUSED)
{
    Entice *entice;
    Elm_Object_Item *list_it;

    list_it = elm_list_selected_item_get(obj);
    elm_list_item_selected_set(list_it, EINA_FALSE);

    entice = evas_object_data_get(win, "entice");

    entice_exif_fill(win);

    if (entice->settings_shown)
    {
        elm_object_signal_emit(entice->layout, "state,settings,hide", "entice");
        entice->settings_shown = EINA_FALSE;
    }

    if (!entice->exif_shown)
    {
        elm_object_signal_emit(entice->layout, "state,exif,show", "entice");
        entice->exif_shown = EINA_TRUE;
    }

    elm_hover_dismiss(entice->hover_menu);
}

static void
_entice_ctrl_menu_copy_filename_cb(void *win,
                                   Evas_Object *obj,
                                   void *event_info EINA_UNUSED)
{
    Entice *entice;
    Elm_Object_Item *list_it;

    entice_win_filename_copy(win);

    list_it = elm_list_selected_item_get(obj);
    elm_list_item_selected_set(list_it, EINA_FALSE);

    entice = evas_object_data_get(win, "entice");
    elm_hover_dismiss(entice->hover_menu);
}

static void
_entice_ctrl_menu_copy_file_cb(void *win,
                               Evas_Object *obj,
                               void *event_info EINA_UNUSED)
{
    Entice *entice;
    Elm_Object_Item *list_it;

    entice_win_file_copy(win);

    list_it = elm_list_selected_item_get(obj);
    elm_list_item_selected_set(list_it, EINA_FALSE);

    entice = evas_object_data_get(win, "entice");
    elm_hover_dismiss(entice->hover_menu);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

void
entice_controls_init(Evas_Object *win)
{
    Entice *entice;
    Evas_Object *o;
    Evas_Object *table;
    Evas_Object *list;
    Evas_Object *icon;

    entice = evas_object_data_get(win, "entice");

    CONTROLS("zoom-out", zoomout);
    CONTROLS("zoom-in", zoomin);
    CONTROLS("object-rotate-left", rotleft);
    CONTROLS("object-rotate-right", rotright);
    CONTROLS("go-previous", prev);
    CONTROLS("go-next", next);

    /* zoom button */

    o = elm_table_add(win);
    evas_object_show(o);
    table = o;

    o = elm_button_add(win);
    elm_object_text_set(o, "2000% ");
    elm_table_pack(table, o, 0, 0, 1, 1);

    o = elm_button_add(win);
    elm_object_focus_allow_set(o, EINA_FALSE);
    elm_object_style_set(o, "overlay");
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_table_pack(table, o, 0, 0, 1, 1);
    evas_object_show(o);
    entice->zoomval = o;
    evas_object_smart_callback_add(entice->zoomval, "clicked",
                                   _entice_ctrl_zoomval_cb, win);

    elm_object_part_content_set(entice->layout, "entice.zoomval", table);

    o = elm_hover_add(win);
    elm_hover_parent_set(o, win);
    elm_hover_target_set(o, entice->zoomval);
    elm_object_style_set(o, "popout");
    entice->hover_zoom = o;

    /* list in a table*/
    table = elm_table_add(win);
    evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(table);

    /* rectangle for the size of the list */
    o = evas_object_rectangle_add(evas_object_evas_get(win));
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_min_set(o, ELM_SCALE_SIZE(100), ELM_SCALE_SIZE(160));
    elm_table_pack(table, o, 0, 0, 1, 1);

    o = elm_check_add(win);
    elm_object_style_set(o, "default");
    elm_object_text_set(o, "Best fit");
    evas_object_smart_callback_add(o, "changed",
                                   _entice_ctrl_zoomcheck_cb, win);
    elm_object_focus_allow_set(o, EINA_FALSE);
    evas_object_show(o);
    entice->bestfit = o;

    list = elm_list_add(win);
    evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_list_mode_set(list, ELM_LIST_EXPAND);

#define LIST_APPEND(_zm)                                            \
    o = elm_label_add(win);                                         \
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);     \
    elm_object_text_set(o, #_zm "%");                               \
    elm_list_item_append(list, NULL, o, NULL,                       \
                         _entice_ctrl_zoom_ ## _zm ## _cb, entice)

    LIST_APPEND(2000);
    LIST_APPEND(1500);
    LIST_APPEND(1000);
    LIST_APPEND(500);
    LIST_APPEND(200);
    LIST_APPEND(133);
    LIST_APPEND(100);
    LIST_APPEND(50);
    LIST_APPEND(33);
#undef LIST_APPEND
    elm_list_item_append(list, NULL, entice->bestfit, NULL, NULL, NULL);

    elm_table_pack(table, list, 0, 0, 1, 1);
    evas_object_show(list);

    elm_object_part_content_set(entice->hover_zoom, "top", table);

    /* evas_object_event_callback_add(entice->zoomval, EVAS_CALLBACK_MOUSE_UP, */
    /*                                _entice_ctrl_zoomval_mouse_up_cb, entice->hover_zoom); */

    CONTROLS("window-close", close);
    CONTROLS("view-fullscreen", fullscreen);
    CONTROLS("menu", menu);

    o = elm_hover_add(win);
    elm_hover_parent_set(o, win);
    elm_hover_target_set(o, entice->menu);
    elm_object_style_set(o, "popout");
    entice->hover_menu = o;

    /* list in a table*/
    table = elm_table_add(win);
    evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(table);

    /* rectangle for the size of the list */
    o = evas_object_rectangle_add(evas_object_evas_get(win));
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_min_set(o, ELM_SCALE_SIZE(100), ELM_SCALE_SIZE(64));
    elm_table_pack(table, o, 0, 0, 1, 1);

    list = elm_list_add(win);
    evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_list_mode_set(list, ELM_LIST_EXPAND);

#define LIST_APPEND(_text, _item)                                   \
    o = elm_label_add(win);                                         \
    elm_object_text_set(o, _text);                                  \
    elm_list_item_append(list, NULL, o, NULL,                       \
                         _entice_ctrl_menu_ ## _item ## _cb, win)

    LIST_APPEND("Settings", settings);
    LIST_APPEND("Exif", exif);
    LIST_APPEND("Copy filename", copy_filename);
    LIST_APPEND("Copy file", copy_file);

    elm_table_pack(table, list, 0, 0, 1, 1);
    evas_object_show(list);

    elm_object_part_content_set(entice->hover_menu, "bottom", table);

    o = elm_icon_add(win);
    elm_icon_standard_set(o, "menu");
    evas_object_show(o);
    icon = o;

    o = elm_button_add(win);
    elm_object_content_set(o, icon);
    elm_object_focus_allow_set(o, EINA_FALSE);
    elm_object_style_set(o, "overlay");
    evas_object_show(o);
    elm_object_part_content_set(entice->hover_menu, "middle", o);

    /* error icon when no file is found */
    o = elm_icon_add(win);
    elm_icon_standard_set(o, "dialog-error");
    evas_object_show(o);
    elm_object_part_content_set(entice->layout, "entice.error", o);
}

void
entice_controls_update(Evas_Object *win)
{
    char buf[16];
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    EINA_SAFETY_ON_NULL_RETURN(entice);

    elm_check_state_set(entice->bestfit,
                        entice_image_zoom_mode_get(entice->image));

    snprintf(buf, sizeof(buf), "%d%%", entice_image_zoom_get(entice->image));
    elm_object_text_set(entice->zoomval, buf);
}

void
entice_controls_timer_start(Evas_Object *win)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");

    if (entice->controls_timer)
    {
        ecore_timer_reset(entice->controls_timer);
        return;
    }

    if ((entice->controls_timer) && (entice->controls_over))
    {
        ecore_timer_del(entice->controls_timer);
        entice->controls_timer = NULL;
        return;
    }

    if ((!entice->controls_timer) && (!entice->controls_over))
        entice->controls_timer = ecore_timer_add(entice->config->duration_controls,
                                                 _entice_ctrl_hide_cb, win);

    /* display controls */
    if (!entice->controls_shown)
    {
        elm_object_signal_emit(entice->layout, "state,controls,show", "entice");
        entice->controls_shown = EINA_TRUE;
    }
}

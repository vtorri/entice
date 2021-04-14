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

#define CONTROLS(_icon, _action)                                         \
    o = elm_icon_add(win);                                               \
    elm_icon_standard_set(o, _icon);                                     \
    evas_object_show(o);                                                 \
    entice->_action = o;                                                 \
                                                                         \
    o = elm_button_add(win);                                             \
    elm_object_content_set(o, entice->_action);                          \
    elm_object_focus_allow_set(o, EINA_FALSE);                           \
    elm_object_style_set(o, "overlay");                                  \
    evas_object_show(o);                                                 \
    elm_object_part_content_set(entice->layout, "entice." #_action, o);  \
                                                                         \
    elm_layout_signal_callback_add(entice->layout,                       \
                                   "image,action," #_action, "entice",   \
                                   _cb_image_##_action, win);            \
                                                                         \
    elm_layout_signal_callback_add(entice->layout,                       \
                                   "image,startfade," #_action, "entice",   \
                                   _cb_image_startfade, win);            \
                                                                         \
    elm_layout_signal_callback_add(entice->layout,                       \
                                   "image,stopfade," #_action, "entice", \
                                   _cb_image_stopfade, win)

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
    entice_image_zoom_increase(entice->image);
    entice_image_update(entice->image);
    printf("zoom in \n");
    fflush(stdout);
}

static void
_cb_image_zoomout(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice_image_zoom_decrease(entice->image);
    entice_image_update(entice->image);
    printf("zoom out \n");
    fflush(stdout);
}

static void
_cb_image_zoomorig(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice_image_zoom_set(entice->image, 100);
    entice_image_update(entice->image);
}

static void
_cb_image_zoomfit(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice_image_zoom_mode_set(entice->image, ENTICE_ZOOM_MODE_FIT);
    entice_image_update(entice->image);
}

static void
_cb_image_zoomcheck(void *win, Evas_Object *obj, void *event_info EINA_UNUSED)
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
_cb_image_zoomval(void *win, Evas_Object *obj, void *event_info EINA_UNUSED)
{
    Entice *entice;
    const char *t;
    char *t2;
    size_t l;
    size_t i;
    int z = 0;
    int p = 1;

    entice = evas_object_data_get(win, "entice");

    elm_object_focus_set(entice->event_kbd, EINA_TRUE);

    t = elm_entry_entry_get(obj);
    t2 = strdup(t);

    if (!t2)
        goto update;

    l = strlen(t2);
    if (t2[l - 1] == '%')
    {
        t2[l - 1] = '\0';
        l--;
    }
    if ((l == 0) || (l > 4))
        goto free_t2;

    for (i = 0;  i < l; i++, p *= 10)
    {
        size_t j = l - 1 - i;
        if ((t2[j] < '0') || (t2[j] > '9'))
        goto free_t2;

        z += (t2[j] - '0') * p;
    }

    free(t2);

    if ((z < 2))
        z = 2;

    if (z > 2000)
        z = 2000;

    entice_image_zoom_set(entice->image, z);
    entice_image_update(entice->image);

    return;

  free_t2:
    free(t2);
  update:
    entice_controls_update(win);
}

static void
_cb_image_close(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    evas_object_del(win);
}

static void
_cb_image_fullscreen(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    elm_win_fullscreen_set(win, !elm_win_fullscreen_get(win));
    (elm_win_fullscreen_get(win))
        ? elm_icon_standard_set(entice->fullscreen, "view-fullscreen")
        : elm_icon_standard_set(entice->fullscreen, "view-restore");
}

static void
_cb_image_ctxpopup_dismissed(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   evas_object_del(obj);
}

static void
_cb_image_ctxpopup_settings_cb(void *win, Evas_Object *obj, void *event_info EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");

    entice_settings_init(win);

    if (entice->exif_shown)
    {
        elm_object_signal_emit(entice->layout, "state,exif,hide", "entice");
        elm_object_signal_emit(entice->layout, "state,exifbg,hide", "entice");
        entice->exif_shown = EINA_FALSE;
    }

    if (!entice->settings_shown)
    {
        elm_object_signal_emit(entice->layout, "state,settings,show", "entice");
        elm_object_signal_emit(entice->layout, "state,settingsbg,show", "entice");
        entice->settings_shown = EINA_TRUE;
    }

    evas_object_del(obj);
}

static void
_cb_image_ctxpopup_exif_cb(void *win, Evas_Object *obj, void *event_info EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");

    entice_exif_fill(win);

    if (entice->settings_shown)
    {
        elm_object_signal_emit(entice->layout, "state,settings,hide", "entice");
        elm_object_signal_emit(entice->layout, "state,settingsbg,hide", "entice");
        entice->settings_shown = EINA_FALSE;
    }

    if (!entice->exif_shown)
    {
        elm_object_signal_emit(entice->layout, "state,exif,show", "entice");
        elm_object_signal_emit(entice->layout, "state,exifbg,show", "entice");
        entice->exif_shown = EINA_TRUE;
    }

    evas_object_del(obj);
}

static void
_cb_image_menu(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Evas_Object *ctxpopup;
    Evas_Coord x,y;

    ctxpopup = elm_ctxpopup_add(win);
    elm_ctxpopup_direction_priority_set(ctxpopup,
                                        ELM_CTXPOPUP_DIRECTION_DOWN,
                                        ELM_CTXPOPUP_DIRECTION_LEFT,
                                        ELM_CTXPOPUP_DIRECTION_RIGHT,
                                        ELM_CTXPOPUP_DIRECTION_UP);

    elm_ctxpopup_item_append(ctxpopup, "settings", NULL,
                             _cb_image_ctxpopup_settings_cb, win);
    elm_ctxpopup_item_append(ctxpopup, "exif", NULL,
                             _cb_image_ctxpopup_exif_cb, win);
    evas_pointer_canvas_xy_get(evas_object_evas_get(win), &x, &y);
    evas_object_move(ctxpopup, x, y);
    evas_object_show(ctxpopup);
    evas_object_smart_callback_add(ctxpopup, "dismissed", _cb_image_ctxpopup_dismissed, NULL);
}

static void
_cb_image_stopfade(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
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
_cb_image_startfade(void *win, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    entice->controls_over = EINA_FALSE;
    //entice_controls_timer_start(win);
    printf("mouse out\n");
    fflush(stdout);
}

static Eina_Bool
_cb_controls_hide(void *win)
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

    /* best fit checkbox */
    o = elm_check_add(win);
    elm_object_style_set(o, "default");
    evas_object_smart_callback_add(o, "changed", _cb_image_zoomcheck, win);
    elm_object_focus_allow_set(o, EINA_FALSE);
    evas_object_show(o);
    entice->zoomcheck = o;

    elm_object_part_content_set(entice->layout, "entice.zoomcheck", entice->zoomcheck);

    /* zoom entry */
    o = elm_entry_add(win);
    elm_scroller_policy_set(o, ELM_SCROLLER_POLICY_OFF,
                            ELM_SCROLLER_POLICY_OFF);
    elm_object_text_set(o, "     ");
    elm_entry_single_line_set(o, EINA_TRUE);
    evas_object_smart_callback_add(o, "activated", _cb_image_zoomval, win);
    evas_object_show(o);
    entice->zoomval = o;

    elm_object_part_content_set(entice->layout, "entice.zoomval", entice->zoomval);

    CONTROLS("menu", menu);
    CONTROLS("view-fullscreen", fullscreen);
    CONTROLS("window-close", close);

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

    elm_check_state_set(entice->zoomcheck,
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
                                             _cb_controls_hide, win);

    /* display controls */
    if (eina_list_prev(entice->image_current))
        elm_object_signal_emit(entice->layout, "state,prev,show", "entice");
    else
        elm_object_signal_emit(entice->layout, "state,prev,hide", "entice");
    if (eina_list_next(entice->image_current))
        elm_object_signal_emit(entice->layout, "state,next,show", "entice");
    else
        elm_object_signal_emit(entice->layout, "state,next,hide", "entice");
    if (!entice->controls_shown)
    {
        elm_object_signal_emit(entice->layout, "state,controls,show", "entice");
        entice->controls_shown = EINA_TRUE;
    }
}

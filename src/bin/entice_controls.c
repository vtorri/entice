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
#include "entice_win.h"
#include "entice_controls.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

static void
_cb_image_prev(void *entice, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Eina_List *next;

    next = eina_list_prev(((Entice *)entice)->image_current);
    if (next)
    {
        entice_image_set(((Entice *)entice)->image, next);
    }
}

static void
_cb_image_next(void *entice, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    Eina_List *next;

    next = eina_list_next(((Entice *)entice)->image_current);
    if (next)
    {
        entice_image_set(((Entice *)entice)->image, next);
    }
}

static void
_cb_image_rot_left(void *entice, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    entice_image_rotate(((Entice *)entice)->image, 3);
}

static void
_cb_image_rot_right(void *entice, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    entice_image_rotate(((Entice *)entice)->image, 1);
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

    o = elm_icon_add(win);
    elm_icon_standard_set(o, "go-previous");
    evas_object_show(o);
    entice->prev = o;

    o = elm_button_add(win);
    elm_object_content_set(o, entice->prev);
    elm_object_focus_allow_set(o, EINA_FALSE);
    evas_object_show(o);
    elm_object_part_content_set(entice->layout, "entice.go.prev", o);

    o = elm_icon_add(win);
    elm_icon_standard_set(o, "go-next");
    evas_object_show(o);
    entice->next = o;

    o = elm_button_add(win);
    elm_object_content_set(o, entice->next);
    elm_object_focus_allow_set(o, EINA_FALSE);
    evas_object_show(o);
    elm_object_part_content_set(entice->layout, "entice.go.next", o);

    o = elm_icon_add(win);
    elm_icon_standard_set(o, "object-rotate-left");
    evas_object_show(o);
    entice->rot_left = o;

    o = elm_button_add(win);
    elm_object_content_set(o, entice->rot_left);
    elm_object_focus_allow_set(o, EINA_FALSE);
    evas_object_show(o);
    elm_object_part_content_set(entice->layout, "entice.rotate.left", o);

    o = elm_icon_add(win);
    elm_icon_standard_set(o, "object-rotate-right");
    evas_object_show(o);
    entice->rot_right = o;

    o = elm_button_add(win);
    elm_object_content_set(o, entice->rot_right);
    elm_object_focus_allow_set(o, EINA_FALSE);
    evas_object_show(o);
    elm_object_part_content_set(entice->layout, "entice.rotate.right", o);

    elm_layout_signal_callback_add(entice->layout,
                                   "image,action,prev", "entice",
                                   _cb_image_prev, entice);
    elm_layout_signal_callback_add(entice->layout,
                                   "image,action,next", "entice",
                                   _cb_image_next, entice);
    elm_layout_signal_callback_add(entice->layout,
                                   "image,action,rotleft", "entice",
                                   _cb_image_rot_left, entice);
    elm_layout_signal_callback_add(entice->layout,
                                   "image,action,rotright", "entice",
                                   _cb_image_rot_right, entice);
}

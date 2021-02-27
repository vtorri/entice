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
#include "entice_key.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

void entice_key_handle(Evas_Object *win, Evas_Event_Key_Down *ev)
{
    Entice *entice;
    Eina_Bool ctrl, alt, shift, winm, meta, hyper; /* modifiers */

    ctrl = evas_key_modifier_is_set(ev->modifiers, "Control");
    alt = evas_key_modifier_is_set(ev->modifiers, "Alt");
    shift = evas_key_modifier_is_set(ev->modifiers, "Shift");
    winm = evas_key_modifier_is_set(ev->modifiers, "Super");
    meta =
        evas_key_modifier_is_set(ev->modifiers, "Meta") ||
        evas_key_modifier_is_set(ev->modifiers, "AltGr") ||
        evas_key_modifier_is_set(ev->modifiers, "ISO_Level3_Shift");
    hyper = evas_key_modifier_is_set(ev->modifiers, "Hyper");

    entice = evas_object_data_get(win, "entice");

    /* No modifier */
    if (!ctrl && !alt && !shift && !winm && !meta && !hyper)
    {
        if (!strcmp(ev->key, "F11"))
        {
            elm_win_fullscreen_set(win, !elm_win_fullscreen_get(win));
        }
        else if (!strcmp(ev->key, "space") ||
                 !strcmp(ev->key, "Right"))
        {
            Eina_List *next;

            next = eina_list_next(entice->image_current);
            if (next)
            {
                entice_image_current_set(win, next);
            }
        }
        else if (!strcmp(ev->key, "BackSpace") ||
                 !strcmp(ev->key, "Left"))
        {
            Eina_List *prev;

            prev = eina_list_prev(entice->image_current);
            if (prev)
            {
                entice_image_current_set(win, prev);
            }
        }
        else if (!strcmp(ev->keyname, "f"))
        {
            entice_image_current_zoom_fit(win);
        }
        else if (!strcmp(ev->keyname, "s"))
        {
            entice_settings_init(win);
            if (!entice->settings_shown)
            {
                elm_object_signal_emit(entice->layout, "state,settings,show", "entice");
                entice->settings_shown = EINA_TRUE;
            }
        }
        else if (!strcmp(ev->key, "Escape"))
        {
            fprintf(stderr, "Esc !!!!\n");
            fflush(stderr);
            if (entice->settings_shown)
            {
                elm_object_signal_emit(entice->layout, "state,settings,hide", "entice");
                entice->settings_shown = EINA_FALSE;
            }
        }
    }

    /* Control modifier */
    if (ctrl && !alt && !shift && !winm && !meta && !hyper)
    {
        if (!strcmp(ev->keyname, "q"))
        {
            evas_object_del(win);
        }
        else if (!strcmp(ev->keyname, "r"))
        {
            entice_image_current_rotate(win, 1);
        }
        else if (!strcmp(ev->key, "KP_0") ||
                 !strcmp(ev->key, "0"))
        {
            entice_image_current_zoom(win, 1.0);
        }
    }

    /* Control + Shift modifier */
    if (ctrl && !alt && shift && !winm && !meta && !hyper)
    {
        if (!strcmp(ev->keyname, "r"))
        {
            entice_image_current_rotate(win, 3);
        }
    }

    /* Alt modifier */
    if (!ctrl && alt && !shift && !winm && !meta && !hyper)
    {
        if (!strcmp(ev->key, "Home"))
        {
            entice_image_current_set(win, entice->images);
        }
        else if (!strcmp(ev->key, "End"))
        {
            entice_image_current_set(win, eina_list_last(entice->images));
        }
    }
}

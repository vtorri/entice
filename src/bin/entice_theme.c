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
#include "entice_win.h"
#include "entice_theme.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

static char *entice_theme_default_get(Entice *entice);

static void
_entice_theme_reload_cb(void *data EINA_UNUSED, Evas_Object *obj, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
    void (*func)(void *d);
    void *func_data;
    const char *file;
    const char *group;

   edje_object_file_get(obj, &file, &group);
   edje_object_file_set(obj, file, group);
   func = evas_object_data_get(obj, "theme_reload_func");
   func_data = evas_object_data_get(obj, "theme_reload_func_data");
   if (func) func(func_data);
}

static char *
entice_theme_default_get(Entice *entice)
{
    int size;

    if (entice->theme_file)
        return entice->theme_file;

    size = snprintf(NULL, 0,
                    "%s/entice/themes/default.edj", elm_app_data_dir_get());
    entice->theme_file = (char *)malloc(size + 2);
    if (!entice->theme_file)
        return NULL;

    snprintf(entice->theme_file, size + 1,
             "%s/entice/themes/default.edj", elm_app_data_dir_get());

    return entice->theme_file;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

Eina_Bool
entice_theme_apply(Evas_Object *win, const char *group)
{
    Entice *entice;

    if ((!win) || (!group))
        return EINA_FALSE;

    entice = evas_object_data_get(win, "entice");
    if (!entice)
        return EINA_FALSE;

    if (elm_layout_file_set(entice->layout, entice_theme_default_get(entice), group))
        return EINA_TRUE;

    return EINA_FALSE;
}

void
entice_theme_reload(Evas_Object *win)
{
    Entice *entice;

    if (!win)
        return;

    entice = evas_object_data_get(win, "entice");
    if (!entice)
        return;

    edje_object_signal_callback_add(entice->layout, "edje,change,file", "edje",
                                    _entice_theme_reload_cb, NULL);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

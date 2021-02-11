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
#include "entice_image.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

void
entice_image_current_set(Evas_Object *win, Eina_List *image)
{
    Entice *entice;
    char *filename;

    if (!image)
        return;

    entice = evas_object_data_get(win, "entice");
    entice->image_current = image;

    filename = eina_list_data_get(entice->image_current);
    if (filename)
    {
        int w;
        int h;

        evas_object_image_file_set(entice->image, filename, NULL);
        evas_object_image_size_get(entice->image, &w, &h);
        evas_object_image_size_set(entice->image, w,h);
        evas_object_image_fill_set(entice->image, 0, 0, w, h);
        evas_object_resize(entice->image, w, h);
        evas_object_size_hint_min_set(entice->image, w, h);
    }
}

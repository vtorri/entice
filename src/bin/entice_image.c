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
#include "entice_image.h"
#include "entice_win.h"

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
    Entice_Image_Prop *prop;
    Evas_Load_Error err;
    int win_w;
    int win_h;

    if (!image)
        return;

    entice = evas_object_data_get(win, "entice");
    entice->image_current = image;

    prop = eina_list_data_get(entice->image_current);
    if (!prop->filename || !*prop->filename)
        return;

    evas_object_image_load_orientation_set(entice->image, EINA_TRUE);
    evas_object_image_file_set(entice->image, prop->filename, NULL);
    err = evas_object_image_load_error_get(entice->image);
    if (err != EVAS_LOAD_ERROR_NONE)
    {
        ERR("Could not load image '%s' : \"%s\"\n",
            prop->filename, evas_load_error_str(err));
        return;
    }

    evas_object_image_size_get(entice->image, &prop->width, &prop->height);
    evas_object_geometry_get(win, NULL, NULL, &win_w, &win_h);
    if ((win_w < prop->width) || (win_h < prop->height))
        entice_image_current_zoom_fit(win);
    else
    {
        evas_object_image_size_set(entice->image, prop->width, prop->height);
        evas_object_image_fill_set(entice->image, 0, 0, prop->width, prop->height);
        evas_object_resize(entice->image, prop->width, prop->height);
        evas_object_size_hint_max_set(entice->image, prop->width, prop->height);
        evas_object_size_hint_min_set(entice->image, prop->width, prop->height);
    }

    entice_win_title_update(win, prop);
}

void
entice_image_current_rotate(Evas_Object *win, unsigned int rot)
{
    Entice *entice;
    Entice_Image_Prop *prop;

    entice = evas_object_data_get(win, "entice");
    prop = eina_list_data_get(entice->image_current);
    prop->orient += rot;
    prop->orient = prop->orient & 3;
    evas_object_image_orient_set(entice->image, prop->orient);
}

void
entice_image_current_zoom(Evas_Object *win, double zoom)
{
    Entice *entice;
    Entice_Image_Prop *prop;
    int ox;
    int oy;
    int w;
    int h;
    int x;
    int y;

    entice = evas_object_data_get(win, "entice");
    prop = eina_list_data_get(entice->image_current);
    evas_object_geometry_get(win, &x, &y, &w, &h);

    if (zoom == 1.0)
    {
        ox = ((w - prop->width) / 2.0) + (double)x + 0.5;
        oy = ((h - prop->height) / 2.0) + (double)y + 0.5;
        evas_object_move(entice->image, ox, oy);
        evas_object_resize(entice->image, prop->width, prop->height);
        evas_object_size_hint_max_set(entice->image, prop->width, prop->height);
        evas_object_size_hint_min_set(entice->image, prop->width, prop->height);
    }
    else
    {
    }
   prop->zoom_mode = ENTICE_ZOOM_MODE_NORMAL;
}

void
entice_image_current_zoom_fit(Evas_Object *win)
{
    Entice *entice;
    Entice_Image_Prop *prop;
    int ow;
    int oh;
    int ox;
    int oy;
    int w;
    int h;
    int x;
    int y;

    entice = evas_object_data_get(win, "entice");
    prop = eina_list_data_get(entice->image_current);
    ow = prop->width;
    oh = prop->height;
    evas_object_geometry_get(win, &x, &y, &w, &h);

    if ((w * oh) > (ow * h))
    {
        int ih = oh;
        oh = h;
        ow = (ow * h) / ih;
    }
    else
    {
        int iw = ow;
        ow = w;
        oh = (oh * w) / iw;
    }

    ox = ((w -ow) / 2.0) + (double)x + 0.5;
    oy = ((h -oh) / 2.0) + (double)y + 0.5;

    evas_object_resize(entice->image, ow, oh);
    evas_object_size_hint_max_set(entice->image, ow, oh);
    evas_object_size_hint_min_set(entice->image, ow, oh);

    evas_object_move(entice->image, ox, oy);

    prop->zoom_mode = ENTICE_ZOOM_MODE_FIT;
}

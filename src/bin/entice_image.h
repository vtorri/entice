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

#ifndef ENTICE_IMAGE_H
#define ENTICE_IMAGE_H

typedef enum
{
    ENTICE_ZOOM_MODE_NORMAL,
    ENTICE_ZOOM_MODE_FIT
} Entice_Zoom_Mode;


Evas_Object *entice_image_add(Evas_Object *win);

void entice_image_set(Evas_Object *obj, Eina_List *image);

const char *entice_image_file_get(Evas_Object *obj);

void entice_image_size_get(Evas_Object *obj, int *w, int *h);

void entice_image_rotate(Evas_Object *obj, unsigned int rot);

void entice_image_zoom_mode_set(Evas_Object *obj, Entice_Zoom_Mode zoom_mode);

Entice_Zoom_Mode entice_image_zoom_mode_get(Evas_Object *obj);

void entice_image_zoom_set(Evas_Object *obj, int zoom);

int entice_image_zoom_get(Evas_Object *obj);

void entice_image_zoom_increase(Evas_Object *obj);

void entice_image_zoom_decrease(Evas_Object *obj);

void entice_image_update(Evas_Object *obj);

char *entice_image_title_get(Evas_Object *obj);

#endif /* ENTICE_IMAGE_H */

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

typedef struct Img_ Img;

struct Img_
{
    Evas_Object *bg;
    Evas_Object *img;
    Ecore_Timer *anim;
    double zoom;
    Entice_Zoom_Mode zoom_mode;
    int fr;
    int fr_num;
    int loops;
};

static Evas_Smart *_smart = NULL;
static Evas_Smart_Class _parent_sc = EVAS_SMART_CLASS_INIT_NULL;

static Eina_Bool
_entice_image_anim_cb(void *obj)
{
    Img *sd;
    double t;
    int fr;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN_VAL(sd, EINA_FALSE);

    sd->fr++;
    fr = ((sd->fr - 1) % (sd->fr_num)) + 1;
    if ((sd->fr >= sd->fr_num) && (fr == 1))
    {
        int loops;

        if (evas_object_image_animated_loop_type_get(sd->img) ==
            EVAS_IMAGE_ANIMATED_HINT_NONE)
        {
            sd->anim = NULL;
            return EINA_FALSE;
        }
        sd->loops++;
        loops = evas_object_image_animated_loop_count_get(sd->img);
        if (loops != 0) // loop == 0 -> loop forever
        {
            if (loops < sd->loops)
            {
                sd->anim = NULL;
                return EINA_FALSE;
            }
        }
     }

    evas_object_image_animated_frame_set(sd->img, fr);
    t = evas_object_image_animated_frame_duration_get(sd->img, fr, 0);
    ecore_timer_interval_set(sd->anim, t);

   return EINA_TRUE;
}

static void
_entice_image_anim_handle(Evas_Object *obj)
{
    Img *sd;
    double t;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);


    if (!evas_object_image_animated_get(sd->img))
        return;

    sd->fr = 1;
    sd->fr_num = evas_object_image_animated_frame_count_get(sd->img);
    if (sd->fr_num < 2)
        return;

    t = evas_object_image_animated_frame_duration_get(sd->img, sd->fr, 0);
    sd->anim = ecore_timer_add(t, _entice_image_anim_cb, obj);
}

static void
_smart_add(Evas_Object *obj)
{
   Img *sd;

   sd = calloc(1, sizeof(Img));
   EINA_SAFETY_ON_NULL_RETURN(sd);

   evas_object_smart_data_set(obj, sd);

   _parent_sc.add(obj);

   sd->img = evas_object_image_filled_add(evas_object_evas_get(obj));
   evas_object_smart_member_add(sd->img, obj);
   evas_object_image_load_orientation_set(sd->img, EINA_TRUE);
   sd->zoom_mode = ENTICE_ZOOM_MODE_NORMAL;
   sd->zoom = 1.0;
}

static void
_smart_del(Evas_Object *obj)
{
    Img *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_data_del(obj, "win");
    if (sd->img)
        evas_object_del(sd->img);

    _parent_sc.del(obj);

    free(sd);
}

static void
_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
    Img *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_resize(sd->img, w, h);
}

static void
_smart_show(Evas_Object *obj)
{
    Img *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_show(sd->img);
}

static void
_smart_hide(Evas_Object *obj)
{
    Img *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_hide(sd->img);
}

static void
_smart_calculate(Evas_Object *obj)
{
    Img *sd;
    Evas_Coord ox;
    Evas_Coord oy;
    Evas_Coord ow;
    Evas_Coord oh;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_geometry_get(obj, &ox, &oy, &ow, &oh);

    evas_object_move(sd->img, ox, oy);
    evas_object_resize(sd->img, ow, oh);
}

static void
_smart_init(void)
{
    static Evas_Smart_Class sc;

    evas_object_smart_clipped_smart_set(&_parent_sc);
    sc           = _parent_sc;
    sc.name      = "image";
    sc.version   = EVAS_SMART_CLASS_VERSION;
    sc.add       = _smart_add;
    sc.del       = _smart_del;
    sc.resize    = _smart_resize;
    sc.show      = _smart_show;
    sc.hide      = _smart_hide;
    sc.calculate = _smart_calculate;
    _smart = evas_smart_class_new(&sc);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

Evas_Object *
entice_image_add(Evas_Object *win)
{
    Evas *evas;
    Evas_Object *obj;

    EINA_SAFETY_ON_NULL_RETURN_VAL(win, NULL);
    evas = evas_object_evas_get(win);
    if (!evas)
    {
        ERR("can not get evas");
        return NULL;
    }

    if (!_smart) _smart_init();
    obj = evas_object_smart_add(evas, _smart);

    evas_object_data_set(obj, "win", win);

    return obj;
}

void
entice_image_set(Evas_Object *obj, Eina_List *image)
{
    Entice *entice;
    Img *sd;
    Evas_Object *win;
    Evas_Load_Error err;
    const char *filename;
    Evas_Coord iw;
    Evas_Coord ih;
    Evas_Coord w;
    Evas_Coord h;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    win = evas_object_data_get(obj, "win");
    entice = evas_object_data_get(win, "entice");
    if (image == entice->image_current)
        return;

    entice->image_current = image;
    filename = eina_list_data_get(entice->image_current);

    evas_object_image_load_orientation_set(sd->img, EINA_TRUE);

    evas_object_image_file_set(sd->img, filename, NULL);
    err = evas_object_image_load_error_get(sd->img);
    if (err != EVAS_LOAD_ERROR_NONE)
    {
        ERR("Could not load image '%s' : \"%s\"\n",
            filename, evas_load_error_str(err));
        return;
    }

    evas_object_image_size_get(sd->img, &iw, &ih);
    evas_object_geometry_get(win, NULL, NULL, &w, &h);

    if ((w < iw) || (h < ih))
        entice_image_zoom_fit(obj);
    else
        entice_image_zoom(obj, sd->zoom);

    // FIXME : with settings
    _entice_image_anim_handle(obj);

    entice_win_title_update(win);
}

void
entice_image_size_get(Evas_Object *obj, int *w, int *h)
{
    Img *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_image_size_get(sd->img, w, h);
}

void
entice_image_rotate(Evas_Object *obj, unsigned int rot)
{
    Img *sd;
    Evas_Image_Orient orient;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    orient = evas_object_image_orient_get(sd->img) + rot;
    evas_object_image_orient_set(sd->img, orient & 3);
    if (sd->zoom_mode == ENTICE_ZOOM_MODE_FIT)
        entice_image_zoom_fit(obj);
}

Entice_Zoom_Mode
entice_image_zoom_mode_get(Evas_Object *obj)
{
    Img *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN_VAL(sd, ENTICE_ZOOM_MODE_NORMAL);

    return sd->zoom_mode;
}

void
entice_image_zoom(Evas_Object *obj, double zoom)
{
    Evas_Object *win;
    Entice *entice;
    Img *sd;
    Evas_Coord w;
    Evas_Coord h;
    Evas_Coord iw;
    Evas_Coord ih;
    Evas_Coord x;
    Evas_Coord y;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    win = evas_object_data_get(obj, "win");
    evas_object_geometry_get(win, NULL, NULL, &w, &h);
    evas_object_image_size_get(sd->img, &iw, &ih);

    if (zoom == 1.0)
    {
        x = ((w - iw) / 2.0);
        y = ((h - ih) / 2.0);

        evas_object_resize(obj, iw, ih);
        evas_object_size_hint_max_set(obj, iw, ih);
        evas_object_size_hint_min_set(obj, iw, ih);

        evas_object_move(sd->img, x, y);
    }
    else
    {
    }

    sd->zoom_mode = ENTICE_ZOOM_MODE_NORMAL;

    entice = evas_object_data_get(win, "entice");
    elm_scroller_policy_set(entice->scroller,
                            ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);
}

void
entice_image_zoom_fit(Evas_Object *obj)
{
    Evas_Object *win;
    Entice *entice;
    Img *sd;
    Evas_Coord w;
    Evas_Coord h;
    Evas_Coord iw;
    Evas_Coord ih;
    Evas_Coord x;
    Evas_Coord y;
    Evas_Image_Orient orient;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    win = evas_object_data_get(obj, "win");
    evas_object_geometry_get(win, NULL, NULL, &w, &h);
    orient = evas_object_image_orient_get(sd->img);
    evas_object_image_size_get(sd->img, &iw, &ih);

    if ((w * ih) > (iw * h))
    {
        int tmp = ih;
        ih = h;
        iw = (iw * h) / tmp;
    }
    else
    {
        int tmp = iw;
        iw = w;
        ih = (ih * w) / tmp;
    }

    x = ((w -iw) / 2.0);
    y = ((h -ih) / 2.0);

    evas_object_resize(obj, iw, ih);
    evas_object_size_hint_max_set(obj, iw, ih);
    evas_object_size_hint_min_set(obj, iw, ih);

    evas_object_move(sd->img, x, y);

    sd->zoom_mode = ENTICE_ZOOM_MODE_FIT;

    entice = evas_object_data_get(win, "entice");
    elm_scroller_policy_set(entice->scroller,
                            ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
}

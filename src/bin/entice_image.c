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

#include "entice_private.h"
#include "entice_config.h"
#include "entice_controls.h"
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
    int zoom;
    Entice_Zoom_Mode zoom_mode;
    int fr;
    int fr_num;
    int loops;
};

static Evas_Smart *_smart = NULL;
static Evas_Smart_Class _parent_sc = EVAS_SMART_CLASS_INIT_NULL;

static int _zoom_levels[] =
{
    2,
    5,
    10,
    20,
    33,
    50,
    67,
    100,
    133,
    200,
    300,
    400,
    500,
    600,
    700,
    800,
    900,
    1000,
    1100,
    1200,
    1300,
    1400,
    1500,
    1600,
    1700,
    1800,
    1900,
    2000
};

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
    sd->zoom_mode = ENTICE_ZOOM_MODE_NORMAL;
    sd->zoom = 100;
}

static void
_smart_del(Evas_Object *obj)
{
    Img *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    evas_object_data_del(obj, "entice");
    evas_object_data_del(obj, "win");
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

static void
_entice_image_size_zoom_get(int zoom, int in_w, int in_h, int img_w, int img_h,
                            int *out_x, int *out_y, int *out_w, int *out_h)
{
    printf("nor window : %d %d\n", in_w, in_h);
    printf("nor image  : %d %d\n", img_w, img_h);
    printf("nor zoom   : %d\n", zoom);
    *out_w = (zoom * img_w) / 100;
    *out_h = (zoom * img_h) / 100;

    if (*out_w >= in_w)
        *out_x = 0;
    else
        *out_x = ((in_w - *out_w) / 2);

    if (*out_h >= in_h)
        *out_y = 0;
    else
        *out_y = ((in_h - *out_h) / 2);
    printf("fit out : %d %d  %d %d  \n", *out_x, *out_y, *out_w, *out_h);
    fflush(stdout);
}

static void
_entice_image_size_fit_get(int in_w, int in_h, int img_w, int img_h,
                           int *out_x, int *out_y, int *out_w, int *out_h,
                           int *zoom)
{
    printf("fit window : %d %d\n", in_w, in_h);
    printf("fit image  : %d %d\n", img_w, img_h);

    if ((in_w * img_h) > (img_w * in_h))
    {
        *out_h = in_h;
        *out_w = (img_w * in_h) / img_h;
        *zoom = (100 * *out_w) / img_w;
    }
    else
    {
        *out_w = in_w;
        *out_h = (img_h * in_w) / img_w;
        *zoom = (100 * *out_h) / img_h;
    }

    *out_x = ((in_w - *out_w) / 2.0);
    *out_y = ((in_h - *out_h) / 2.0);
    printf("fit out : %d %d  %d %d  %d%%\n", *out_x, *out_y, *out_w, *out_h, *zoom);
    fflush(stdout);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

Evas_Object *
entice_image_add(Evas_Object *win)
{
    Evas *evas;
    Evas_Object *obj;
    Entice *entice;

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

    entice = evas_object_data_get(win, "entice");
    EINA_SAFETY_ON_NULL_RETURN_VAL(entice, NULL);

    evas_object_data_set(obj, "entice", entice);

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

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    win = evas_object_data_get(obj, "win");
    EINA_SAFETY_ON_NULL_RETURN(win);

    entice = evas_object_data_get(obj, "entice");
    EINA_SAFETY_ON_NULL_RETURN(entice);

    if (!image)
    {
        elm_object_signal_emit(entice->layout, "state,error,show", "entice");
        elm_win_title_set(win, "Entice");
        return;
    }

    if (image == entice->image_current)
        return;

    entice->image_current = image;
    filename = eina_list_data_get(entice->image_current);

    evas_object_image_load_orientation_set(sd->img,
                                           entice->config->automatic_orientation);

    evas_object_image_file_set(sd->img, filename, NULL);
    err = evas_object_image_load_error_get(sd->img);
    if (err != EVAS_LOAD_ERROR_NONE)
    {
        ERR("Could not load image '%s' : \"%s\"\n",
            filename, evas_load_error_str(err));
        ERR("Verify that the Evas loader is available for this image if you think it is a valid image");
        return;
    }

    if (entice->config->best_fit_startup)
        sd->zoom_mode = ENTICE_ZOOM_MODE_FIT;

    if (entice->config->play_animated)
        _entice_image_anim_handle(obj);

    entice_image_update(obj);
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
        entice_image_update(obj);
}

void
entice_image_zoom_mode_set(Evas_Object *obj, Entice_Zoom_Mode zoom_mode)
{
    Img *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    sd->zoom_mode = zoom_mode;
}

Entice_Zoom_Mode
entice_image_zoom_mode_get(Evas_Object *obj)
{
    Img *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN_VAL(sd, ENTICE_ZOOM_MODE_NORMAL);

    return sd->zoom_mode;
}

void entice_image_zoom_set(Evas_Object *obj, int zoom)
{
    Img *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    sd->zoom_mode = ENTICE_ZOOM_MODE_NORMAL;
    sd->zoom = zoom;
    printf("zoom set : %d\n", sd->zoom);
    fflush(stdout);
}

int entice_image_zoom_get(Evas_Object *obj)
{
    Img *sd;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN_VAL(sd, 100);

    return sd->zoom;
}

void
entice_image_zoom_increase(Evas_Object *obj)
{
    Img *sd;
    size_t l;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    l = sizeof(_zoom_levels) / sizeof(int);
    for (size_t i = 0; i < (l - 1); i++)
    {
        if ((_zoom_levels[i] <= sd->zoom) && (sd->zoom < _zoom_levels[i + 1]))
        {
            entice_image_zoom_set(obj, _zoom_levels[i + 1]);
            break;
        }
    }
}

void
entice_image_zoom_decrease(Evas_Object *obj)
{
    Img *sd;
    size_t l;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    l = sizeof(_zoom_levels) / sizeof(int);
    for (size_t i = (l - 1); i > 0; i--)
    {
        if ((_zoom_levels[i - 1] < sd->zoom) && (sd->zoom <= _zoom_levels[i]))
        {
            entice_image_zoom_set(obj, _zoom_levels[i - 1]);
            break;
        }
    }
}

void
entice_image_update(Evas_Object *obj)
{
    Evas_Object *win;
    Entice *entice;
    Img *sd;
    Evas_Coord win_w;
    Evas_Coord win_h;
    Evas_Coord img_w;
    Evas_Coord img_h;
    Evas_Coord out_x;
    Evas_Coord out_y;
    Evas_Coord out_w;
    Evas_Coord out_h;
    int zoom;

    sd = evas_object_smart_data_get(obj);
    EINA_SAFETY_ON_NULL_RETURN(sd);

    win = evas_object_data_get(obj, "win");
    EINA_SAFETY_ON_NULL_RETURN(win);

    entice = evas_object_data_get(win, "entice");
    EINA_SAFETY_ON_NULL_RETURN(entice);

    evas_object_geometry_get(win, NULL, NULL, &win_w, &win_h);
    evas_object_image_size_get(sd->img, &img_w, &img_h);

    switch (sd->zoom_mode)
    {
        case ENTICE_ZOOM_MODE_NORMAL:
            _entice_image_size_zoom_get(sd->zoom, win_w, win_h, img_w, img_h,
                                        &out_x, &out_y,  &out_w, &out_h);
            elm_scroller_policy_set(entice->scroller,
                                    ELM_SCROLLER_POLICY_AUTO,
                                    ELM_SCROLLER_POLICY_AUTO);
            break;
        case ENTICE_ZOOM_MODE_FIT:
            _entice_image_size_fit_get(win_w, win_h, img_w, img_h,
                                       &out_x, &out_y, &out_w, &out_h, &zoom);
            sd->zoom = zoom;
            elm_scroller_policy_set(entice->scroller,
                                    ELM_SCROLLER_POLICY_OFF,
                                    ELM_SCROLLER_POLICY_OFF);
            break;
    }

    evas_object_resize(obj, out_w, out_h);
    evas_object_size_hint_max_set(obj, out_w, out_h);
    evas_object_size_hint_min_set(obj, out_w, out_h);

    evas_object_move(sd->img, out_x, out_y);

    entice_win_title_update(win);
    entice_controls_update(win);
}

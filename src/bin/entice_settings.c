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
#include "entice_key.h"
#include "entice_image.h"
#include "entice_win.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

typedef struct
{
    Entice *entice;
    Entice_Config *config;
    Evas_Object *op_w;
    Evas_Object *op_h;
    Evas_Object *op_wh_current;
    Evas_Object *op_duration_controls;
} Settings_Ctx;

#define OPTIONS_CB(_cfg_name, _inv)                         \
    static void                                             \
    _ent_st_##_cfg_name##_cb(void *data, Evas_Object *obj,  \
                             void *_event EINA_UNUSED)      \
    {                                                       \
        Settings_Ctx *ctx = data;                           \
        Entice_Config *config = ctx->config;                \
        if (_inv)                                           \
            config->_cfg_name = !elm_check_state_get(obj);  \
        else                                                \
            config->_cfg_name = elm_check_state_get(obj);   \
        entice_config_save(config);                         \
    }

#define SETTINGS_CX(_lbl, _cfg_name, _inv)                             \
    do                                                                 \
    {                                                                  \
        o = elm_check_add(box);                                        \
        evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);    \
        evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);       \
        elm_object_text_set(o, _lbl);                                  \
        elm_check_state_set(o, _inv ? !ctx->config->_cfg_name          \
                            : ctx->config->_cfg_name);                 \
        elm_box_pack_end(box, o);                                      \
        evas_object_show(o);                                           \
        evas_object_smart_callback_add(o, "changed",                   \
                                       _ent_st_##_cfg_name##_cb, ctx); \
    } while (0)

#define SETTINGS_SEPARATOR                                          \
    do                                                              \
    {                                                               \
        o = elm_separator_add(box);                                 \
        evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0); \
        evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);    \
        elm_separator_horizontal_set(o, EINA_TRUE);                 \
        elm_box_pack_end(box, o);                                   \
        evas_object_show(o);                                        \
    } while (0)


static void
_ent_st_custom_geometry_cb(void *data,
                           Evas_Object *obj,
                           void *_event EINA_UNUSED)
{
    Settings_Ctx *ctx = data;
    Entice_Config *config = ctx->config;

    config->custom_geometry = elm_check_state_get(obj);
    if (config->custom_geometry)
    {
        config->cg_width = (int) elm_spinner_value_get(ctx->op_w);
        config->cg_height = (int) elm_spinner_value_get(ctx->op_h);
    }
    entice_config_save(config);

    elm_object_disabled_set(ctx->op_w, !config->custom_geometry);
    elm_object_disabled_set(ctx->op_h, !config->custom_geometry);
    elm_object_disabled_set(ctx->op_wh_current, !config->custom_geometry);
}

static void
_entice_settings_custom_geometry_current_set_cb(void *data,
                                                Evas_Object *obj EINA_UNUSED,
                                                void *_event EINA_UNUSED)
{
    Settings_Ctx *ctx = data;
    Entice_Config *config = ctx->config;

    if (config->custom_geometry)
    {
        elm_spinner_value_set(ctx->op_w, config->cg_width);
        elm_spinner_value_set(ctx->op_h, config->cg_height);
    }
    entice_config_save(config);
}

static void
_entice_settings_cg_width_cb(void *data,
                             Evas_Object *obj,
                             void *_event EINA_UNUSED)
{
    Settings_Ctx *ctx = data;
    Entice_Config *config = ctx->config;

    if (config->custom_geometry)
    {
        config->cg_width = (int) elm_spinner_value_get(obj);
        entice_config_save(config);
    }
}

static void
_entice_settings_cg_height_cb(void *data,
                              Evas_Object *obj,
                              void *_event EINA_UNUSED)
{
    Settings_Ctx *ctx = data;
    Entice_Config *config = ctx->config;

    if (config->custom_geometry)
    {
        config->cg_height = (int) elm_spinner_value_get(obj);
        entice_config_save(config);
    }
}

static void
_entice_settings_duration_controls_cb(void *data,
                                      Evas_Object *obj,
                                      void *_event EINA_UNUSED)
{
    Settings_Ctx *ctx = data;
    Entice_Config *config = ctx->config;

    config->duration_controls = (int) elm_spinner_value_get(obj);
    entice_config_save(config);
}

static void
_entice_settings_order_cb(void *data,
                          Evas_Object *obj,
                          void *event_info EINA_UNUSED)
{
    Settings_Ctx *ctx = data;
    Entice_Config *config = ctx->config;

    printf("radio group value : %d\n", elm_radio_value_get(obj));
    fflush(stdout);
    config->order = elm_radio_value_get(obj);
    entice_config_save(config);
}

OPTIONS_CB(fullscreen_startup, 0);
OPTIONS_CB(automatic_orientation, 0);
OPTIONS_CB(best_fit_startup, 0);
OPTIONS_CB(play_animated, 0);

static Eina_Bool
_entice_settings_focus_set_cb(void *win)
{
    Entice *entice = evas_object_data_get(win, "entice");

    entice->settings_timer = NULL;
    elm_object_focus_set(entice->event_kbd, EINA_TRUE);
    return EINA_FALSE;
}

static void
_entice_settings_key_down_cb(void *win,
                         Evas *evas EINA_UNUSED,
                         Evas_Object *obj EINA_UNUSED,
                         void *event_info)
{
    Entice *entice;
    Evas_Event_Key_Down *ev;
    Eina_Bool ctrl, alt, shift, winm, meta, hyper; /* modifiers */

    EINA_SAFETY_ON_NULL_RETURN(event_info);

    ev = (Evas_Event_Key_Down *)event_info;

    ENTICE_MODIFIERS_GET(ev->modifiers);

    entice = evas_object_data_get(win, "entice");

    /* No modifier */
    if (!ctrl && !alt && !shift && !winm && !meta && !hyper)
    {
        if (!strcmp(ev->key, "Escape"))
        {
            elm_object_signal_emit(entice->layout, "state,settings,hide", "entice");
            entice->settings_shown = EINA_FALSE;
        }
    }

    entice->settings_timer = ecore_timer_add(0.1, _entice_settings_focus_set_cb, win);
}

static void
_entice_settings_close_cb(void *win,
                          Evas_Object *obj EINA_UNUSED,
                          void *_event EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    elm_object_signal_emit(entice->layout, "state,settings,hide", "entice");
    elm_object_focus_set(entice->event_kbd, EINA_TRUE);
    entice->settings_shown = EINA_FALSE;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

void
entice_settings_init(Evas_Object *win)
{
    Entice *entice;
    Settings_Ctx *ctx;
    Evas_Object *o;
    Evas_Object *frame;
    Evas_Object *scroller;
    Evas_Object *box;
    Evas_Object *hbox2;
    Evas_Object *rdg;
    int w;
    int h;

    entice = evas_object_data_get(win, "entice");
    if (entice->settings_created)
        return;

    ctx = calloc(1, sizeof(Settings_Ctx));
    if (!ctx)
        return;

    ctx->entice = entice;
    ctx->config = entice->config;
    evas_object_geometry_get(win, NULL, NULL, &w, &h);

    o = elm_frame_add(win);
    elm_object_style_set(o, "border");
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_text_set(o, "Settings");
    evas_object_show(o);
    frame = o;
    evas_object_smart_callback_add(frame, "close",
                                   _entice_settings_close_cb, win);

    evas_object_event_callback_add(frame, EVAS_CALLBACK_KEY_DOWN,
                                   _entice_settings_key_down_cb, win);


    o = elm_scroller_add(frame);
    elm_scroller_content_min_limit(o, EINA_TRUE, EINA_FALSE);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_content_set(frame, o);
    evas_object_show(o);
    scroller = o;

    o = elm_box_add(scroller);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.0);
    elm_object_content_set(scroller, o);
    evas_object_show(o);
    box = o;

    SETTINGS_CX("Always open at size", custom_geometry, 0);

    o = elm_button_add(box);
    evas_object_size_hint_weight_set(o, 0.0, 0.0);
    evas_object_size_hint_align_set(o, 0.0, 0.5);
    elm_object_text_set(o, "Set Current:");
    elm_box_pack_end(box, o);
    evas_object_show(o);
    ctx->op_wh_current = o;
    elm_object_disabled_set(o, !ctx->config->custom_geometry);
    evas_object_smart_callback_add(o, "clicked",
                                   _entice_settings_custom_geometry_current_set_cb,
                                   ctx);

    o = elm_label_add(box);
    evas_object_size_hint_weight_set(o, 0.0, 0.0);
    evas_object_size_hint_align_set(o, 0.0, 0.5);
    elm_object_text_set(o, "Width:");
    elm_box_pack_end(box, o);
    evas_object_show(o);

    o = elm_spinner_add(box);
    elm_spinner_editable_set(o, EINA_TRUE);
    elm_spinner_min_max_set(o, 2.0, 960.0);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
    if (ctx->config->custom_geometry)
        elm_spinner_value_set(o, ctx->config->cg_width);
    else
        elm_spinner_value_set(o, w);
    elm_object_disabled_set(o, !ctx->config->custom_geometry);
    elm_box_pack_end(box, o);
    evas_object_show(o);
    ctx->op_w = o;
    evas_object_smart_callback_add(o, "changed",
                                   _entice_settings_cg_width_cb, ctx);

    o = elm_label_add(box);
    evas_object_size_hint_weight_set(o, 0.0, 0.0);
    evas_object_size_hint_align_set(o, 0.0, 0.5);
    elm_object_text_set(o, "Height:");
    elm_box_pack_end(box, o);
    evas_object_show(o);

    o = elm_spinner_add(box);
    elm_spinner_editable_set(o, EINA_TRUE);
    elm_spinner_min_max_set(o, 1.0, 540.0);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
    if (ctx->config->custom_geometry)
        elm_spinner_value_set(o, ctx->config->cg_height);
    else
        elm_spinner_value_set(o, h);
    elm_object_disabled_set(o, !ctx->config->custom_geometry);
    elm_box_pack_end(box, o);
    evas_object_show(o);
    ctx->op_h = o;
    evas_object_smart_callback_add(o, "changed",
                                   _entice_settings_cg_height_cb, ctx);

    SETTINGS_CX("Fullscreen at startup", fullscreen_startup, 0);

    SETTINGS_SEPARATOR;

    SETTINGS_CX("Automatic orientation of images", automatic_orientation, 0);
    SETTINGS_CX("Always best fit", best_fit_startup, 0);
    SETTINGS_CX("Play animated images", play_animated, 0);

    o = elm_label_add(box);
    evas_object_size_hint_weight_set(o, 0.0, 0.0);
    evas_object_size_hint_align_set(o, 0.0, 0.5);
    elm_object_text_set(o, "Display duration of controls:");
    elm_box_pack_end(box, o);
    evas_object_show(o);

    o = elm_spinner_add(box);
    elm_spinner_editable_set(o, EINA_TRUE);
    elm_spinner_min_max_set(o, 1.0, 20.0);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
    elm_spinner_value_set(o, (int) ctx->config->duration_controls);
    elm_box_pack_end(box, o);
    evas_object_show(o);
    ctx->op_duration_controls = o;
    evas_object_smart_callback_add(o, "changed",
                                   _entice_settings_duration_controls_cb, ctx);

    o = elm_box_add(box);
    evas_object_size_hint_weight_set(o, 0.0, 0.0);
    evas_object_size_hint_align_set(o, 0.0, 0.0);
    elm_box_horizontal_set(o, EINA_TRUE);
    elm_box_pack_end(box, o);
    evas_object_show(o);
    hbox2 = o;

    o = elm_label_add(box);
    evas_object_size_hint_weight_set(o, 0.0, 0.0);
    evas_object_size_hint_align_set(o, 0.0, 0.5);
    elm_object_text_set(o, "Image order:");
    elm_box_pack_end(hbox2, o);
    evas_object_show(o);

    o = elm_radio_add(box);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
    elm_object_text_set(o, "alphabetic");
    elm_radio_state_value_set(o, 0);
    elm_box_pack_end(hbox2, o);
    evas_object_show(o);
    evas_object_smart_callback_add(o, "changed",
                                   _entice_settings_order_cb, ctx);

    rdg = o;

    o = elm_radio_add(box);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
    elm_object_text_set(o, "date");
    elm_radio_state_value_set(o, 1);
    elm_radio_group_add(o, rdg);
    elm_box_pack_end(hbox2, o);
    evas_object_show(o);
    evas_object_smart_callback_add(o, "changed",
                                   _entice_settings_order_cb, ctx);

    elm_radio_value_set(rdg, ctx->config->order);

    elm_object_part_content_set(entice->layout, "entice.settings.panel", frame);

    entice->settings_created = EINA_TRUE;
}

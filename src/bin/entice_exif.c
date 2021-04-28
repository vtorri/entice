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

#include <libexif/exif-data.h>

#include <Elementary.h>

#include "entice_private.h"
#include "entice_config.h"
#include "entice_win.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

typedef struct
{
    Evas_Object *button;
    const char *name;
    const char *tag;
} Entice_Exif_Entry;

static Entice_Exif_Entry entice_exif_entries[] =
{
    { NULL, "Width:",         "PixelXDimension"  },
    { NULL, "Height:",        "PixelYDimension"  },
    { NULL, "Exposure time:", "ExposureTime"     },
    { NULL, "Date time:",     "DateTimeOriginal" },
    { NULL, "Focal length::", "FocalLength"      },
    { NULL, "Exposure time",  "ColorSpace"       },
};

static Entice_Exif_Entry entice_exif_gps_entries[] =
{
    { NULL, "Latitude Ref:",    "GPSLatitudeRef"     },
    { NULL, "Latitude:",        "GPSLatitude"        },
    { NULL, "Longiture Ref:",   "GPSLongitudeRef"    },
    { NULL, "Longiture:",       "GPSLongitude"       },
    { NULL, "Altitude Ref:",    "GPSAltitudeRef"     },
    { NULL, "Altitude:",        "GPSAltitude"     },
    { NULL, "Timestamp:",       "GPSTimeStamp"       },
    { NULL, "Satellites:",      "GPSSatellites"      },
    { NULL, "ImgDirectionRef:", "GPSImgDirectionRef" },
    { NULL, "Map Datum:",       "GPSMapDatum"        },
    { NULL, "DateStamp:",       "GPSDateStamp"       },
};

static Eina_Bool
_entice_exif_focus_set_cb(void *win)
{
    Entice *entice = evas_object_data_get(win, "entice");

    entice->exif_timer = NULL;
    elm_object_focus_set(entice->event_kbd, EINA_TRUE);
    return EINA_FALSE;
}

static void
_entice_exif_key_down_cb(void *win,
                         Evas *evas EINA_UNUSED,
                         Evas_Object *obj EINA_UNUSED,
                         void *event_info)
{
    Entice *entice;
    Evas_Event_Key_Down *ev;
    Eina_Bool ctrl, alt, shift, winm, meta, hyper; /* modifiers */

    EINA_SAFETY_ON_NULL_RETURN(event_info);

    ev = (Evas_Event_Key_Down *)event_info;

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
        if (!strcmp(ev->key, "Escape"))
        {
            ERR("exif cb");
            elm_object_signal_emit(entice->layout, "state,exif,hide", "entice");
            entice->exif_shown = EINA_FALSE;
        }
    }

    entice->exif_timer = ecore_timer_add(0.1, _entice_exif_focus_set_cb, win);
}

static void
_entice_exif_close_cb(void *win,
                      Evas_Object *obj,
                      void *_event EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    elm_object_signal_emit(entice->layout, "state,exif,hide", "entice");
    elm_object_focus_set(entice->event_kbd, EINA_TRUE);
    entice->exif_shown = EINA_FALSE;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

void
entice_exif_init(Evas_Object *win)
{
    Entice *entice;
    Evas_Object *o;
    Evas_Object *scroller;
    Evas_Object *box;
    Evas_Object *table;
    size_t i;

    entice = evas_object_data_get(win, "entice");
    if (entice->exif_created)
        return;

    o = elm_frame_add(win);
    elm_object_style_set(o, "border");
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_text_set(o, "EXIF");
    evas_object_show(o);
    entice->frame_exif = o;
    evas_object_smart_callback_add(entice->frame_exif, "close",
                                   _entice_exif_close_cb, win);

    o = elm_scroller_add(win);
    elm_scroller_content_min_limit(o, EINA_TRUE, EINA_FALSE);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_content_set(entice->frame_exif, o);
    evas_object_show(o);
    scroller = o;

    o = elm_box_add(scroller);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.0);
    elm_object_content_set(scroller, o);
    evas_object_show(o);
    box = o;

    /* EXIF informations */

    o = elm_table_add(box);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_box_pack_end(box, o);
    evas_object_show(o);
    table = o;

    for (i = 0; i < sizeof(entice_exif_entries) / sizeof(Entice_Exif_Entry); i++)
    {
        o = elm_label_add(table);
        elm_object_text_set(o, entice_exif_entries[i].name);
        evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(o, 1.0, 0.5);
        elm_table_pack(table, o, 0, i, 1, 1);
        evas_object_show(o);

        o = elm_entry_add(table);
        elm_entry_single_line_set(o, EINA_TRUE);
        elm_entry_scrollable_set(o, EINA_TRUE);
        elm_object_part_text_set(o, "text", "Unknown");
        evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
        elm_table_pack(table, o, 1, i, 1, 1);
        evas_object_show(o);
        entice_exif_entries[i].button = o;
    }

    /* GPS informations */

    o = elm_separator_add(box);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
    elm_separator_horizontal_set(o, EINA_TRUE);
    elm_box_pack_end(box, o);
    evas_object_show(o);

    o = elm_table_add(box);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_box_pack_end(box, o);
    evas_object_show(o);
    table = o;

    for (i = 0; i < sizeof(entice_exif_gps_entries) / sizeof(Entice_Exif_Entry); i++)
    {
        o = elm_label_add(table);
        elm_object_text_set(o, entice_exif_gps_entries[i].name);
        evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(o, 1.0, 0.5);
        elm_table_pack(table, o, 0, i, 1, 1);
        evas_object_show(o);

        o = elm_entry_add(table);
        elm_entry_single_line_set(o, EINA_TRUE);
        elm_entry_scrollable_set(o, EINA_TRUE);
        elm_object_part_text_set(o, "text", "Unknown");
        evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
        elm_table_pack(table, o, 1, i, 1, 1);
        evas_object_show(o);
        entice_exif_gps_entries[i].button = o;
    }

    elm_object_part_content_set(entice->layout,
                                "entice.exif.panel", entice->frame_exif);

    entice->exif_created = EINA_TRUE;
}

void
entice_exif_fill(Evas_Object *win)
{
    char value[1024];
    Entice *entice;
    const Entice_List_Data *data;
    ExifData *ed;
    ExifContent *ec;
    ExifEntry *ee;
    unsigned int i;

    entice = evas_object_data_get(win, "entice");

    evas_object_event_callback_add(entice->frame_exif, EVAS_CALLBACK_KEY_DOWN,
                                   _entice_exif_key_down_cb, win);

    data = (const Entice_List_Data *)eina_list_data_get(entice->image_current);
    ed = exif_data_new_from_file(data->path);
    if (!ed)
    {
        WRN("can not get exif data from %s", data->path);
        return;
    }

    ec  = ed->ifd[EXIF_IFD_EXIF];
    if (ec && ec->count)
    {
        printf("IFD '%s'\n", exif_ifd_get_name(EXIF_IFD_EXIF));
        for (i = 0; i < ec->count; i++)
        {
            const char *tag_name;
            size_t j;

            ee = ec->entries[i];
            tag_name = exif_tag_get_name_in_ifd(ee->tag,
                                                exif_entry_get_ifd(ee));
            //printf("  tag name : %s\n", tag_name);

            for (j = 0; j < sizeof(entice_exif_entries) / sizeof(Entice_Exif_Entry); j++)
            {
                //printf("  tag entice : %s\n",entice_exif_entries[j].tag);
                //fflush(stdout);
                if (strcmp(entice_exif_entries[j].tag, tag_name) == 0)
                {
                    elm_object_text_set(entice_exif_entries[j].button,
                                        exif_entry_get_value(ee, value, sizeof(value)));
                    break;
                }
            }
        }
    }

    ec  = ed->ifd[EXIF_IFD_GPS];
    if (ec && ec->count)
    {
        printf("IFD '%s'\n", exif_ifd_get_name(EXIF_IFD_GPS));
        for (i = 0; i < ec->count; i++)
        {
            const char *tag_name;
            size_t j;

            ee = ec->entries[i];
            tag_name = exif_tag_get_name_in_ifd(ee->tag,
                                                exif_entry_get_ifd(ee));
            //printf("  tag name : %s\n", tag_name);

            for (j = 0; j < sizeof(entice_exif_gps_entries) / sizeof(Entice_Exif_Entry); j++)
            {
                //printf("  tag entice : %s\n",entice_exif_gps_entries[j].tag);
                //fflush(stdout);
                if (strcmp(entice_exif_gps_entries[j].tag, tag_name) == 0)
                {
                    elm_object_text_set(entice_exif_gps_entries[j].button,
                                        exif_entry_get_value(ee, value, sizeof(value)));
                    break;
                }
            }
        }
    }
}

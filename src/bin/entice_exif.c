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
    { NULL, "Focal length::",  "FocalLength"      },
    { NULL, "Exposure time", "ColorSpace"       },
};

static Entice_Exif_Entry entice_exif_gps_entries[] =
{
    { NULL, "Latitude Ref:",    "GPSLatitudeRef"     },
    { NULL, "Latitude:",        "GPSLatitude"        },
    { NULL, "Longiture Ref:",   "GPSLongitudeRef"    },
    { NULL, "Longiture:",       "GPSLongitude"       },
    { NULL, "Altitude Ref:",    "GPSAltitudeRef"     },
    { NULL, "Timestamp:",       "GPSTimeStamp"       },
    { NULL, "Satellites:",      "GPSSatellites"      },
    { NULL, "ImgDirectionRef:", "GPSImgDirectionRef" },
    { NULL, "Map Datum:",       "GPSMapDatum"        },
    { NULL, "DateStamp:",       "GPSDateStamp"       },
};

static void
_cb_op_exif_close(void *win,
                      Evas_Object *obj,
                      void *_event EINA_UNUSED)
{
    Entice *entice;

    entice = evas_object_data_get(win, "entice");
    elm_object_signal_emit(entice->layout, "state,exif,hide", "entice");
    elm_object_signal_emit(entice->layout, "state,exifbg,hide", "entice");
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
    Evas_Object *vbox;
    Evas_Object *hbox;
    Evas_Object *box;
    Evas_Object *icon;
    Evas_Object *frame;
    Evas_Object *table;
    size_t i;

    entice = evas_object_data_get(win, "entice");
    if (entice->exif_created)
        return;

    o = elm_box_add(win);
    evas_object_size_hint_weight_set(o, 0.0, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, 0.0, EVAS_HINT_FILL);
    evas_object_show(o);
    vbox = o;

    o = elm_box_add(vbox);
    evas_object_size_hint_weight_set(o, 0.0, 0.0);
    evas_object_size_hint_align_set(o, 1.0, 0.0);
    elm_box_horizontal_set(o, EINA_TRUE);
    elm_box_pack_end(vbox, o);
    evas_object_show(o);
    hbox = o;

    o = elm_icon_add(win);
    evas_object_size_hint_align_set(o, 1.0, 0.0);
    elm_icon_standard_set(o, "window-close");
    evas_object_show(o);
    icon = o;

    o = elm_button_add(vbox);
    elm_object_content_set(o, icon);
    evas_object_size_hint_align_set(o, 1.0, 0.0);
    elm_box_pack_end(hbox, o);
    evas_object_show(o);
    evas_object_smart_callback_add(o, "clicked",
                                   _cb_op_exif_close,
                                   win);

    o = elm_scroller_add(win);
    elm_scroller_content_min_limit(o, EINA_TRUE, EINA_FALSE);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);

    elm_box_pack_end(vbox, o);
    evas_object_show(o);
    scroller = o;

    o = elm_box_add(scroller);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.0);
    elm_object_content_set(scroller, o);
    evas_object_show(o);
    box = o;

    /* EXIF informations */

    o = elm_frame_add(box);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_focus_allow_set(o, EINA_FALSE);
    elm_object_text_set(o, "EXIF");
    elm_box_pack_end(box, o);
    evas_object_show(o);
    frame = o;

    o = elm_table_add(frame);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_content_set(frame, o);
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

    o = elm_frame_add(box);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_focus_allow_set(o, EINA_FALSE);
    elm_object_text_set(o, "GPS");
    elm_box_pack_end(box, o);
    evas_object_show(o);
    frame = o;

    o = elm_table_add(frame);
    evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_content_set(frame, o);
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

    elm_object_part_content_set(entice->layout, "entice.exif.panel", vbox);

    entice->exif_created = EINA_TRUE;
}

void
entice_exif_fill(Evas_Object *win)
{
    char value[1024];
    Entice *entice;
    ExifData *ed;
    ExifContent *ec;
    ExifEntry *ee;
    unsigned int i;

    entice = evas_object_data_get(win, "entice");

    ed = exif_data_new_from_file(eina_list_data_get(entice->image_current));
    if (!ed)
    {
      printf("can not get exif data from %s\n",
             (char *)eina_list_data_get(entice->image_current));
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
            printf("  tag name : %s\n", tag_name);

            for (j = 0; j < sizeof(entice_exif_entries) / sizeof(Entice_Exif_Entry); j++)
            {
                printf("  tag entice : %s\n",entice_exif_entries[j].tag);
                fflush(stdout);
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
            printf("  tag name : %s\n", tag_name);

            for (j = 0; j < sizeof(entice_exif_gps_entries) / sizeof(Entice_Exif_Entry); j++)
            {
                printf("  tag entice : %s\n",entice_exif_gps_entries[j].tag);
                fflush(stdout);
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

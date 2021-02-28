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

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

#define CONFIG_KEY "config"

#define ENTICE_CONFIG_EDD_KEY_ADD(_s, _m, _t) \
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd_keys, Entice_Config_Keys, _s, _m, _t)

#define ADD_KEYS(Name, Ctrl, Alt, Shift, Win, Cb) \
    kb = calloc(1, sizeof(Entice_Config_Keys)); \
    if (!kb) return; \
    kb->keyname = eina_stringshare_add_length(Name, strlen(Name)); \
    kb->ctrl = Ctrl; \
    kb->alt = Alt; \
    kb->shift = Shift; \
    kb->win = Win; \
    kb->cb = eina_stringshare_add_length(Cb, strlen(Cb)); \
    config->keys = eina_list_append(config->keys, kb)

static Eet_Data_Descriptor *edd_base = NULL;
static Eet_Data_Descriptor *edd_keys = NULL;

static void
_entice_default_keys_add(Entice_Config *config)
{
    Entice_Config_Keys *kb;

    ADD_KEYS("F11", 0, 0, 0, 0, "win_fullscreen");
}

static Entice_Config *
_entice_config_new(void)
{
    Entice_Config *config;

    config = (Entice_Config *)calloc(1, sizeof(Entice_Config));
    if (!config)
        return NULL;

    config->theme = eina_stringshare_add("default.edj");
    config->cg_width = 960;
    config->cg_height = 540;
    config->custom_geometry = EINA_FALSE;
    config->fullscreen_startup = EINA_FALSE;
    _entice_default_keys_add(config);

    return config;
}

static const char *
_entice_config_theme_path_get(const char *name)
{
    static char path1[PATH_MAX];
    static char path2[PATH_MAX];
    struct stat s;

    snprintf(path2, sizeof(path2) - 1, "%s/.config/entice/themes/%s",
             eina_environment_home_get(), name);
    if (stat(path2, &s) == 0)
        return path2;
    snprintf(path1, sizeof(path1) - 1, "%s/config/entice/themes/%s",
             elm_app_data_dir_get(), name);
    return path1;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

void
entice_config_init(void)
{
    char path[PATH_MAX];
    Eet_Data_Descriptor_Class eddc;
    Eet_Data_Descriptor_Class eddkc;

    snprintf(path, sizeof(path), "%s/.config/entice/themes",
             eina_environment_home_get());
    ecore_file_mkpath(path);

    eet_eina_stream_data_descriptor_class_set(&eddc, sizeof(eddc),
                                              "Config", sizeof(Entice_Config));
    edd_base = eet_data_descriptor_stream_new(&eddc);

    eet_eina_stream_data_descriptor_class_set(&eddkc, sizeof(eddkc),
                                              "Config_Keys", sizeof(Entice_Config_Keys));
    edd_keys = eet_data_descriptor_stream_new(&eddkc);

    ENTICE_CONFIG_EDD_KEY_ADD("keyname", keyname, EET_T_STRING);
    ENTICE_CONFIG_EDD_KEY_ADD("ctrl", ctrl, EET_T_UCHAR);
    ENTICE_CONFIG_EDD_KEY_ADD("alt", alt, EET_T_UCHAR);
    ENTICE_CONFIG_EDD_KEY_ADD("shift", shift, EET_T_UCHAR);
    ENTICE_CONFIG_EDD_KEY_ADD("win", win, EET_T_UCHAR);
    ENTICE_CONFIG_EDD_KEY_ADD("meta", meta, EET_T_UCHAR);
    ENTICE_CONFIG_EDD_KEY_ADD("hyper", hyper, EET_T_UCHAR);
    ENTICE_CONFIG_EDD_KEY_ADD("cb", cb, EET_T_STRING);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd_base, Entice_Config, "theme", theme, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd_base, Entice_Config, "cg_width", cg_width, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd_base, Entice_Config, "cg_height", cg_height, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd_base, Entice_Config, "custom_geometry", custom_geometry, EET_T_UCHAR);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd_base, Entice_Config, "fullscreen_startup", fullscreen_startup, EET_T_UCHAR);
    EET_DATA_DESCRIPTOR_ADD_LIST(edd_base, Entice_Config, "keys", keys, edd_keys);
}

void
entice_config_shutdown(void)
{
    eet_data_descriptor_free(edd_base);
    eet_data_descriptor_free(edd_keys);
}

Entice_Config *
entice_config_load(void)
{
    char buf[PATH_MAX];
    Eet_File *ef;
    Entice_Config *config = NULL;

    snprintf(buf, sizeof(buf), "%s/.config/entice/config/base.cfg",
             eina_environment_home_get());
    buf[sizeof(buf) - 1] = '\0';
    ef = eet_open(buf, EET_FILE_MODE_READ);
    if (ef)
    {
        config = eet_data_read(ef, edd_base, CONFIG_KEY);
        eet_close(ef);
        if (eina_list_count(config->keys) == 0)
        {
            _entice_default_keys_add(config);
        }
    }

    if (!config)
        config = _entice_config_new();

    return config;
}

void
entice_config_del(Entice_Config *config)
{
    Entice_Config_Keys *key;

    if (!config)
        return;

    eina_stringshare_del(config->theme);

    EINA_LIST_FREE(config->keys, key)
    {
        eina_stringshare_del(key->keyname);
        eina_stringshare_del(key->cb);
        free(key);
     }

    free(config);
}

void
entice_config_save(Entice_Config *config)
{
    char buf[PATH_MAX];
    char buf2[PATH_MAX];
    Eet_File *ef;

    EINA_SAFETY_ON_NULL_RETURN(config);

    snprintf(buf, sizeof(buf), "%s/.config/entice/config",
             eina_environment_home_get());
    ecore_file_mkpath(buf);
    snprintf(buf, sizeof(buf), "%s/.config/entice/config/base.cfg.tmp",
             eina_environment_home_get());
    snprintf(buf2, sizeof(buf2), "%s/.config/entice/config/base.cfg",
             eina_environment_home_get());
    ef = eet_open(buf, EET_FILE_MODE_WRITE);
    if (ef)
    {
        int ok;

        ok = eet_data_write(ef, edd_base, CONFIG_KEY, config, 1);
        eet_close(ef);
        if (ok) ecore_file_mv(buf, buf2);
    }
}

const char *
entice_config_theme_path_get(const Entice_Config *config)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(config, NULL);
    EINA_SAFETY_ON_NULL_RETURN_VAL(config->theme, NULL);

    if (strchr(config->theme, '/'))
        return config->theme;

   return _entice_config_theme_path_get(config->theme);
}

const char *
entice_config_theme_path_default_get(const Entice_Config *config)
{
    static char path[PATH_MAX];

   EINA_SAFETY_ON_NULL_RETURN_VAL(config, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(config->theme, NULL);

   *path = '\0';
   if (path[0]) return path;

   snprintf(path, sizeof(path), "%s/config/entice/themes/default.edj",
            elm_app_data_dir_get());

   return path;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

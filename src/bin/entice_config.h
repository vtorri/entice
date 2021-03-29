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

#ifndef ENTICE_CONFIG_H
#define ENTICE_CONFIG_H

typedef struct Entice_Config_Keys_ Entice_Config_Keys;
typedef struct Entice_Config_ Entice_Config;

struct Entice_Config_Keys_
{
    const char *keyname;
    Eina_Bool   ctrl;
    Eina_Bool   alt;
    Eina_Bool   shift;
    Eina_Bool   win;
    Eina_Bool   meta;
    Eina_Bool   hyper;
    const char *cb;
};

struct Entice_Config_
{
    const char *theme;
    int         cg_width;
    int         cg_height;
    int         duration_controls;
    Eina_Bool   custom_geometry;
    Eina_Bool   fullscreen_startup;
    Eina_Bool   automatic_orientation;
    Eina_Bool   best_fit_startup;
    Eina_Bool   play_animated;
    Eina_List  *keys;
};

void entice_config_init(void);
void entice_config_shutdown(void);
Entice_Config *entice_config_load(void);
void entice_config_del(Entice_Config *config);
void entice_config_save(Entice_Config *config);

const char *entice_config_theme_path_get(const Entice_Config *config);
const char *entice_config_theme_path_default_get(const Entice_Config *config);

#endif /* ENTICE_CONFIG_H */

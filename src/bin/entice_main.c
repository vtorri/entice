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
#include <Ecore_Getopt.h>

#include "entice_private.h"
#include "entice_config.h"
#include "entice_image.h"
#include "entice_win.h"

#define ENTICE_COPYRIGHT "(C) 2021 Vincent Torri and others"

static Ecore_Getopt options = {
    PACKAGE_NAME,
    "%prog [options] [filename]",
    PACKAGE_VERSION,
    gettext_noop(ENTICE_COPYRIGHT),
    "BSD 2-Clause License",
    gettext_noop("Simple image viewer written with Enlightenment Foundation Libraries."),
    EINA_TRUE,
    {
        ECORE_GETOPT_STORE_TRUE('f', "fullscreen",
                                gettext_noop("Go into the fullscreen mode from start.")),

        ECORE_GETOPT_VERSION   ('V', "version"),
        ECORE_GETOPT_COPYRIGHT ('C', "copyright"),
        ECORE_GETOPT_LICENSE   ('L', "license"),
        ECORE_GETOPT_HELP      ('h', "help"),
        ECORE_GETOPT_SENTINEL
    }
};

int entice_app_log_dom_global = 1;

#if ENABLE_NLS
static void
_entice_translate_options(void)
{
    Ecore_Getopt_Desc *desc;

    options.copyright = eina_stringshare_printf(_(options.copyright),
                                                2020);

    desc = (Ecore_Getopt_Desc *)options.descs;
    while ((desc->shortname != '\0') ||
           (desc->longname) ||
           (desc->action == ECORE_GETOPT_ACTION_CATEGORY))
    {
        if (desc->help)
        {
            switch (desc->action)
            {
                case ECORE_GETOPT_ACTION_VERSION:
                    desc->help = _("show program version.");
                    break;
                case ECORE_GETOPT_ACTION_COPYRIGHT:
                    desc->help = _("show copyright.");
                    break;
                case ECORE_GETOPT_ACTION_LICENSE:
                    desc->help = _("show license.");
                    break;
                case ECORE_GETOPT_ACTION_HELP:
                    desc->help = _("show this message.");
                    break;
                default:
                    desc->help = _(desc->help);
            }
        }
        desc++;
    }
}
#endif

static Eina_List *
_file_list_append(Eina_List *list, const char *path)
{
    const Eina_List *l;
    const char *mime;
    const char *data;
    Eina_Bool found;

    mime = efreet_mime_type_get(path);
    if (mime &&
        (strncmp(mime, "image/", strlen("image/")) == 0))
    {
        found = EINA_FALSE;
        EINA_LIST_FOREACH(list, l, data)
        {
            if (strcmp(data, path) == 0)
            {
                found = EINA_TRUE;
                break;
            }
        }
        if (!found)
            list = eina_list_append(list, eina_stringshare_add(path));
    }

    return list;
}


static Eina_List *
_dir_parse(Eina_List *list, const char *path)
{
    Eina_Iterator *it;
    Eina_File_Direct_Info *info;

    if (!path || !*path)
        return list;

    it = eina_file_stat_ls(path);
    EINA_ITERATOR_FOREACH(it, info)
    {
        if (info->type == EINA_FILE_REG)
        {
            list = _file_list_append(list, info->path);
        }
    }
    fflush(stdout);

    return list;
}

static EAPI_MAIN int
elm_main(int argc, char **argv)
{
    Evas_Object *win;
    Eina_List *list;
    Eina_Bool fullscreen = EINA_FALSE;
    Eina_Bool quit_option = EINA_FALSE;
    Ecore_Getopt_Value values[] = {
        ECORE_GETOPT_VALUE_BOOL(fullscreen),

        ECORE_GETOPT_VALUE_BOOL(quit_option),
        ECORE_GETOPT_VALUE_BOOL(quit_option),
        ECORE_GETOPT_VALUE_BOOL(quit_option),
        ECORE_GETOPT_VALUE_BOOL(quit_option),

        ECORE_GETOPT_VALUE_NONE
    };
    int args;

    elm_need_efreet();

    entice_app_log_dom_global = eina_log_domain_register(PACKAGE_NAME, NULL);
    if (entice_app_log_dom_global < 0)
    {
        EINA_LOG_CRIT(_("could not create log domain '" PACKAGE_NAME "'."));
        goto end;
    }

    args = ecore_getopt_parse(&options, values, argc, argv);
    if (args < 0)
    {
        ERR(_("Could not parse command line options."));
        goto end;
    }

    if (quit_option)
        goto end;

    list = NULL;
    if (args == argc)
    {
        list = _dir_parse(list, efreet_pictures_dir_get());
    }
    else
    {
        int i;
        char *realpath;

        for (i = args; i < argc; i++)
        {
            realpath = strdup(argv[i]);
            if (!strncasecmp(realpath, "file://", strlen("file://")))
            {
                Efreet_Uri *uri = efreet_uri_decode(realpath);
                if (uri)
                {
                    free(realpath);
                    realpath = ecore_file_realpath(uri->path);
                    efreet_uri_free(uri);
                }
            }
            if (ecore_file_is_dir(realpath))
                list = _dir_parse(list, realpath);
            else
            {
                list = _file_list_append(list, realpath);
                free(realpath);
            }
        }
    }
    {
        Eina_List *l;
        char *data;

        EINA_LIST_FOREACH(list, l, data)
        {
            printf(" ** %s\n", data);
        }
    }

    entice_config_init();
    /* FIXME key binding */

    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
    elm_app_compile_bin_dir_set(PACKAGE_BIN_DIR);
    elm_app_compile_lib_dir_set(PACKAGE_LIB_DIR);
    elm_app_compile_data_dir_set(PACKAGE_DATA_DIR);
#if ENABLE_NLS
    elm_app_compile_locale_set(LOCALEDIR);
#endif
    elm_app_name_set(PACKAGE_NAME);
    elm_app_info_set(elm_main, PACKAGE_NAME, "themes/default.edj");

#if ENABLE_NLS
    bindtextdomain(PACKAGE_NAME, elm_app_locale_dir_get());
    textdomain(PACKAGE_NAME);
    _entice_translate_options();
#else
    options.copyright = ENTICE_COPYRIGHT;
#endif

    win = entice_win_add();
    if (!win)
    {
        ERR(_("could not create main window."));
        goto shutdown_config;
    }
    evas_object_resize(win,
                       562 * elm_config_scale_get(),
                       800 * elm_config_scale_get());

    evas_object_show(win);

    entice_win_images_set(win, list);

    if (fullscreen) elm_win_fullscreen_set(win, EINA_TRUE);

    /* once the images are in the list, display the first and current one */
    entice_image_set(win);

    elm_run();

    return 0;

 shutdown_config:
    entice_config_shutdown();
    eina_log_domain_unregister(entice_app_log_dom_global);
    entice_app_log_dom_global = -1;
  end:
    return 1;
}
ELM_MAIN()

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
    Eina_Bool   custom_geometry;
    Eina_Bool   disable_visual_bell;
    Eina_Bool   bell_rings;
    Eina_List  *keys;

    const char *config_key;
};

void entice_config_init(void);
void entice_config_shutdown(void);
Entice_Config *entice_config_load(const char *key);
void entice_config_del(Entice_Config *config);
void entice_config_save(Entice_Config *config, const char *key);

const char *entice_config_theme_path_get(const Entice_Config *config);
const char *entice_config_theme_path_default_get(const Entice_Config *config);

#endif /* ENTICE_CONFIG_H */

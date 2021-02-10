#ifndef ENTICE_PRIVATE_H
#define ENTICE_PRIVATE_H

#if ENABLE_NLS
# include <libintl.h>
# define _(string) gettext(string)
#else
# define _(string) (string)
#endif
#define gettext_noop(String) String

extern int entice_app_log_dom_global;

#ifdef ENTICE_DEFAULT_LOG_COLOR
# undef ENTICE_DEFAULT_LOG_COLOR
#endif
#define ENTICE_DEFAULT_LOG_COLOR EINA_COLOR_CYAN

#ifdef ERR
# undef ERR
#endif
#define ERR(...)  EINA_LOG_DOM_ERR(entice_app_log_dom_global, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...)  EINA_LOG_DOM_DBG(entice_app_log_dom_global, __VA_ARGS__)

#ifdef INF
# undef INF
#endif
#define INF(...)  EINA_LOG_DOM_INFO(entice_app_log_dom_global, __VA_ARGS__)

#ifdef WRN
# undef WRN
#endif
#define WRN(...)  EINA_LOG_DOM_WARN(entice_app_log_dom_global, __VA_ARGS__)

#ifdef CRIT
# undef CRIT
#endif
#define CRIT(...) EINA_LOG_DOM_CRIT(entice_app_log_dom_global, __VA_ARGS__)

#endif /* ENTICE_PRIVATE_H */

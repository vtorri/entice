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

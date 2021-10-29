#ifndef SRC_UTILS_GETTEXT_UTIL_H
#define SRC_UTILS_GETTEXT_UTIL_H
#include <libintl.h>
#define _(STRING) gettext(STRING)
void gettextUtil_init();
#endif
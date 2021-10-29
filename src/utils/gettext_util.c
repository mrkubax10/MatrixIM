#include "utils/gettext_util.h"
#include <locale.h>
void gettextUtil_init(){
    bindtextdomain("messages","./locale");
    setlocale(LC_ALL,"pl_PL.UTF-8");
    textdomain("messages");
}
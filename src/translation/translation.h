#ifndef SRC_TRANSLATION_TRANSLATION_H
#define SRC_TRANSLATION_TRANSLATION_H

extern char* translation_lang;
void translation_setLanguage(char* lang);
char* translation_translate(char* msgid);
#define _(msgid) translation_translate(msgid)

#endif

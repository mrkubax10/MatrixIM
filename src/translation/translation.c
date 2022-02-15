#include "translation/translation.h"

#include <stdlib.h>
#include <stdio.h>
#include <cjson/cJSON.h>
#include <string.h>

#include "utils/filesystem.h"

char* translation_lang;
static char* translation_data=0;
static unsigned int translation_dataEntryCount;
void translation_setLanguage(char* lang){
    if(translation_data)
        free(translation_data);
    translation_data=0;
    translation_dataEntryCount=0;
    char filePath[512];
    snprintf(filePath,512,"locale/%s.json",lang);
    char* fileData=loadFullFile(filePath);
    if(!fileData)
        return;
    const cJSON* jsonData=cJSON_Parse(fileData);
    free(fileData);
    unsigned int dataLength=0;
    if(cJSON_IsArray(jsonData)){
        for(int i=0; i<cJSON_GetArraySize(jsonData); i++){
            cJSON* arrayItem=cJSON_GetArrayItem(jsonData,i);
            if(cJSON_IsObject(arrayItem)){
                cJSON* msgid=cJSON_GetObjectItemCaseSensitive(arrayItem,"msgid");
                cJSON* msgstr=cJSON_GetObjectItemCaseSensitive(arrayItem,"msgstr");
                if(cJSON_IsString(msgid) && cJSON_IsString(msgstr)){
                    translation_dataEntryCount++;
                    translation_data=realloc(translation_data,dataLength+strlen(msgid->valuestring)+strlen(msgstr->valuestring)+2);
                    strcpy(translation_data+dataLength,msgid->valuestring);
                    strcpy(translation_data+dataLength+strlen(msgid->valuestring)+1,msgstr->valuestring);
                    dataLength+=strlen(msgid->valuestring)+strlen(msgstr->valuestring)+2;
                }
            }
        }
    }
    cJSON_free((void*)jsonData);
}
char* translation_translate(char* msgid){
    unsigned int offset=0;
    for(unsigned int i=0; i<translation_dataEntryCount; i++){
        if(strcmp(translation_data+offset,msgid)==0)
            return translation_data+offset+strlen(translation_data+offset)+1;
        offset+=strlen(translation_data+offset)+1;
        offset+=strlen(translation_data+offset)+1;
    }
    return msgid;
}
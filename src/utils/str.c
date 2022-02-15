// MatrixIM
// Copyright (C) 2022 mrkubax10

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "utils/str.h"

#include <stdbool.h>
char** split(char* str,char ch,int* len){
    char** output=0;
    int elements=0,tempLength=0;
    bool hasData=false;
    char* temp=0;
    for(int i=0; i<strlen(str); i++){
        if(str[i]==ch){
            if(!hasData)
                continue;
            elements++;
            output=realloc(output,elements*sizeof(char*));
            output[elements-1]=malloc(tempLength+1);
            memcpy(output[elements-1],temp,tempLength);
            output[elements-1][tempLength]=0;
            free(temp);
            tempLength=0;
            temp=0;
            hasData=false;
        }
        else{
            tempLength++;
            temp=realloc(temp,tempLength*sizeof(char));
            temp[tempLength-1]=str[i];
            hasData=true;
        }
    }
    if(tempLength>0){
        elements++;
        output=realloc(output,elements*sizeof(char*));
        output[elements-1]=malloc(tempLength+1);
        memcpy(output[elements-1],temp,tempLength);
        output[elements-1][tempLength]=0;
        free(temp);
    }
    *len=elements;
    return output;
}
char** splitByString(char* str,char* str2,int* len){
    char** output=0;
    if(strlen(str2)>=strlen(str)){
        *len=1;
        output=realloc(output,1*sizeof(char*));
        output[0]=malloc(strlen(str)+1);
        strcpy(output[0],str);
        return output;
    }
    int elements=0,tempLength=0;
    bool hasData=false;
    char* temp=0;
    for(int i=0; i<strlen(str); i++){
        if(strlen(str2)+i-1<strlen(str) && memcmp((void*)(str+i),(void*)str2,strlen(str2))==0){
            if(!hasData)
                continue;
            elements++;
            output=realloc(output,elements*sizeof(char*));
            output[elements-1]=malloc(tempLength+1);
            memcpy(output[elements-1],temp,tempLength);
            output[elements-1][tempLength]=0;
            free(temp);
            temp=0;
            tempLength=0;
            hasData=false;
            i+=strlen(str2)-1;
        }
        else{
            tempLength++;
            temp=realloc(temp,tempLength*sizeof(char));
            temp[tempLength-1]=str[i];
            hasData=true;
        }
    }
    if(tempLength>0){
        elements++;
        output=realloc(output,elements*sizeof(char*));
        output[elements-1]=malloc(tempLength+1);
        memcpy(output[elements-1],temp,tempLength);
        output[elements-1][tempLength]=0;
        free(temp);
    }
    *len=elements;
    return output;
}
char* intToString(int num){
    int length=snprintf(0,0,"%d",num);
    char* output=(char*)malloc(length+1);
    snprintf(output,length+1,"%d",num);
    return output;
}
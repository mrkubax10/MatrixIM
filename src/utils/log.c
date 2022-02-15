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

#include "utils/log.h"

#include <stdarg.h>
#include <stdio.h>
void log_info(const char* component,const char* fmt,...){
    va_list args;
    va_start(args,fmt);
    printf("[Log] %s: ",component);
    vprintf(fmt,args);
    va_end(args);
    printf("\n");
}
void log_warning(const char* component,const char* fmt,...){
    va_list args;
    va_start(args,fmt);
    printf("[Warning] %s: ",component);
    vprintf(fmt,args);
    va_end(args);
    printf("\n");
}
void log_error(const char* component,const char* fmt,...){
    va_list args;
    va_start(args,fmt);
    printf("[Error] %s: ",component);
    vprintf(fmt,args);
    va_end(args);
    printf("\n");
}
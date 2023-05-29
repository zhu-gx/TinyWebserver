#pragma once
#include<stdio.h>
#include<stdlib.h>

//异常退出
void errif(bool condition,const char* errmsg){
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}
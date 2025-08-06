#include <stdlib.h>  // For malloc, free, realloc
#include <stdint.h>  // For int64_t
#include <stdio.h>
#include <string.h>

#include "../include/array_int64.h"

void init_int64_list(list_64t* list, int max_size){
    list->data = malloc(sizeof(int64_t)*max_size);
    if(list->data == NULL){
        printf("Malloc failed to initalise array");
        return;
    }
    list->size = 0;
    list->max_size = max_size;
}

void free_int64_list(list_64t* list){
    free(list->data);
    list->data = NULL;
    free(list);
}

int resize_int64(list_64t* list,int new_size){
    int64_t* temp = realloc(list->data,sizeof(int64_t)*new_size);
    if(temp == NULL){
        printf("resize failed no space left!");
        return 0;
    }
    if (new_size < list->size){
        list->size = new_size;
    }
    list->max_size = new_size;
    list->data = temp;
    return 1;
}

void print_int64_array(list_64t* list) {

    for(int i = 0; i < list->size; i++) {
        printf("%ld ", list->data[i]);
    }
    printf("\n");
}


void insert_int64(list_64t* list, int index, int64_t value){
    int size = list->size;
    if(size < index || index<0){
        printf("insertion index out of range\n");
        return;
    }
    if(size >= list->max_size){
        if(!resize_int64(list,list->max_size*2)){
            return;
        }
    }
    for(int i= size; i > index;i--){
        list->data[i] = list->data[i-1];
    }
    list->data[index] = value;
    list->size++;
    return;
}






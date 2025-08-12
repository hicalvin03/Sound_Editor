#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "../libs/chunk_list.h"


chunk* chunk_init(int16_t* data,int start_index, size_t len){ //create and setup a new chunk
    chunk* node = malloc(sizeof(chunk));
    if(node == NULL){
        perror("malloc failed!");
    }
    node->data = data;
    node->start_index = start_index;
    node->len = len;
    node->start_array = data;
    node->next = NULL;
    return node;
}

void free_chunk(chunk* node){
    free(node->data);
    //node->next = NULL;
    free(node);
}

void init_chunk_list(chunk_list* list, int list_max_size) { // only initalise memory for chunk_ptrs since this is only a list of pts to parents.
    list->data = malloc(sizeof(chunk*) * list_max_size);
    if (!list->data) {
        printf("Failed to allocate list of parents!\n");
        return;
    }
    list->list_size = 0;
    list->list_max_size = list_max_size;
}

void add_chunk(chunk_list* list, chunk* data_member) { // combined insert_at and resize from chunk_int implem since we can not decrease this list.
    if (list->list_size >= list->list_max_size) {
        list->list_max_size *= 2;
        chunk** temp = realloc(list->data, sizeof(chunk*) * list->list_max_size);
        if (!temp) {
            printf("Unable to resize parent list!\n");
            return;
        }
        list->data = temp;
    }
    list->data[list->list_size] = data_member;
    list->list_size++;

}

void free_chunk_list(chunk_list* list){
    free(list->data); // these pointers are owned by someone else so we're not leaking any memory.
    free(list); //asume chunk_list is dynamically allocated as well!
}

chunk_list* shallow_copy_chunk_list(chunk_list* original) {
    
    // Allocate memory for the new chunk_list
    chunk_list* copy = malloc(sizeof(chunk_list));
    if (!copy) {
        printf("Failed to allocate shallow copy of chunk_list\n");
        return NULL;
    }

    copy->data = malloc(sizeof(chunk*) * original->list_max_size);
    if (!copy->data) {
        printf("Failed to allocate shallow data array pointers\n");
        free(copy);
        return NULL;
    }

    // Copy over the pointers shallow copy
    memcpy(copy->data, original->data, sizeof(chunk*) * original->list_size);
    
    copy->list_size = original->list_size;
    copy->list_max_size = original->list_max_size;

    return copy;
}
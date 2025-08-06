#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "../libs/load_WAV.h"

typedef struct chunk chunk;

struct chunk {
    int16_t* data;
    size_t len;
    int start_index;
    int16_t* start_array;
    chunk* next;
};

typedef struct {
    chunk* head_node;
    size_t total_length;
} sound_seg;

chunk* chunk_init(int16_t* data,int start_index, size_t len){ //create and setup a new chunk
    chunk* node = malloc(sizeof(chunk));
    if(node == NULL){
        perror("malloc failed!");
    }
    node->data = data;
    node->start_index = start_index;
    node->len = len;
    node->start_array = data;
    return node;
}

void free_chunk(chunk* node){
    free(node->data);
    //node->next = NULL;
    free(node);
}

sound_seg* tr_init() { //Creates heap memory for a new sound_seg track.
    sound_seg* new_track = malloc(sizeof(sound_seg));
    if (new_track == NULL){
        perror("failed to allocate mem for new sound_seg!");
        return NULL;
    }
    new_track->total_length = 0;
    new_track->head_node = NULL;
    return new_track;
}

void tr_destroy(sound_seg* obj) {
    (void)obj;



    return;
}

size_t tr_length(sound_seg* seg) {
    size_t curr_len;
    chunk* curr_node = seg->head_node;
    while(curr_node != NULL){
        curr_len += curr_node->len;
        curr_node = curr_node->next;
    }

    return curr_len;
}

void tr_read(sound_seg* track, int16_t* dest, size_t pos, size_t len) { 
    (void)track;
    (void)dest;
    (void)pos;
    (void)len;
    return;
}

void tr_write(sound_seg* track, int16_t* src, size_t pos, size_t len) {
    size_t total_len = tr_length(track);
    if (pos > total_len) {
        printf("pos is out of bounds!\n");
        return;
    }
    
    //if (track->head_node == NULL) {
        //track->head_node = chunk_init(src, pos, len);
        //return;
    //}
    
    size_t write_start = pos;
    size_t write_end = pos + len - 1;

    int16_t* src_cpy = src;
    
    chunk* curr_node = track->head_node;

    while(curr_node != NULL) {
        size_t chunk_start = curr_node->start_index;
        size_t chunk_end = curr_node->start_index + curr_node->len - 1;
        
        // Check if overlap between write range and chunk
        if (write_start <= chunk_end && write_end >= chunk_start) {
            size_t overlap_start = (write_start > chunk_start) ? write_start : chunk_start;
            size_t overlap_end = (write_end < chunk_end) ? write_end : chunk_end;
            
            size_t overlap_len = overlap_end - overlap_start + 1;
            size_t chunk_offset = overlap_start - chunk_start;
            size_t src_offset = overlap_start - write_start;
            
            memcpy(&curr_node->data[chunk_offset], &src[src_offset], overlap_len * sizeof(int16_t));

            src_cpy += overlap_len;
            len -= overlap_len;
            pos += overlap_len; // make pos shift to the current pos.

        }
        curr_node = curr_node->next;
    }

    if (len > 0){
        chunk* new_chunk = chunk_init(src_cpy, pos, len);

        if (track->head_node == NULL) { //if track is empty. 
            track->head_node = new_chunk;
        } else {
            chunk* tail = track->head_node;
            while (tail->next != NULL){ // find the tail node and append the new_chunk.
                tail = tail->next;
            }
            tail->next = new_chunk;
        }
    }

}

bool tr_delete_range(sound_seg* track, size_t pos, size_t len) {
    (void)track;
    (void)pos;
    (void)len;
    return true;
}

char* tr_identify(sound_seg* target, sound_seg* ad){
    (void)target;
    (void)ad;
    return NULL;
}

void tr_insert(sound_seg* src_track,
            sound_seg* dest_track,
            size_t destpos, size_t srcpos, size_t len) {
    (void)src_track;
    (void)dest_track;
    (void)destpos;
    (void)srcpos;
    (void)len;
    return;
}

void print_int16_array(const int16_t* arr, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void print_track(sound_seg* gogoltha){


}

void print_sound_seg(sound_seg* seg) {//prints the structure of sound_seg.
    if (seg == NULL || seg->head_node == NULL) {
        printf("sound_seg is empty.\n");
        return;
    }

    printf("Sound segment (total length: %zu):\n", seg->total_length);

    chunk* current = seg->head_node;
    int chunk_num = 0;

    while (current != NULL) {
        printf("Chunk %d:\n", chunk_num);
        printf("  Start Index: %d\n", current->start_index);
        printf("  Length: %zu\n", current->len);
        printf("  Data: [");

        for (size_t i = 0; i < current->len; i++) {
            printf("%d", current->data[i]);
            if (i < current->len - 1) {
                printf(", ");
            }
        }
        printf("]\n");

        current = current->next;
        chunk_num++;
    }
}


int main(){
    sound_seg* obj = tr_init();
    int16_t src[3] = {1,2,3};
    int16_t src1[5] = {5,6,7,8,9};
    int16_t src3[1] = {4};
    tr_write(obj,src,0,3);
    tr_write(obj,src1,1,5);
    tr_write(obj,src3,6,1);
    print_sound_seg(obj);
    printf("length is %zu\n",tr_length(obj));

}
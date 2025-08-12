#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "../libs/load_WAV.h"
#include "../libs/array_int64.h"
#include "../libs/chunk_list.h"
#include "../libs/sound_seg.h"

double dot_product(int16_t* arr1,int16_t* arr2, size_t size);
 
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
    size_t curr_len = 0;
    chunk* curr_node = seg->head_node;
    while(curr_node != NULL){
        curr_len += curr_node->len;
        curr_node = curr_node->next;
    }

    return curr_len;
}

void tr_read(sound_seg* track, int16_t* dest, size_t pos, size_t len) {  //
    size_t total_len = tr_length(track); 
    size_t last_index = pos+len;

    if (pos >= total_len){
        printf("pos is out of bounds!\n");
        return;
    }
    else if (last_index > total_len){
        len = total_len - pos;
    }

    size_t elem_trav = 0;
    size_t dest_idx = 0;

    chunk* curr = track->head_node;
    while(curr != NULL){
        size_t len_chunk = curr->len;
        if (len_chunk+elem_trav <= pos){ //check if chunk is within read range
            elem_trav += len_chunk;
            curr = curr->next;
            continue;
        }

        for (size_t j=0; j<len_chunk && dest_idx < len; j++){ //once in range update dest.
            if (elem_trav >= pos){
                dest[dest_idx] = curr->data[j];
                dest_idx++;
            }
            elem_trav += 1;
        }  
        curr = curr->next;
    }
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


char* solution_formatter(list_64t* solution){
    int size = solution->size;

    if (size == 0){ // when no elements return empty string.
        char* empty_sol = malloc(1); 
        if (empty_sol != NULL){
            empty_sol[0]='\0';
            return empty_sol;
        }
    }
    size_t str_len= (size/2)*50+1; //each int64_t has a maxima of 20 digits so a pair of int64_t with , and \n will be < 45 len;
    char* sol = malloc(str_len); // each char is 1 byte.

    char* curr_chunk = sol; // point to specific chunk in sol we are inserting into. initally start of sol.
    for (size_t i =0; i< size; i +=2){
        if (i < size - 2) { // case less then last pair.
            int bytes_inserted = sprintf(curr_chunk, "%ld,%ld\n", solution->data[i], solution->data[i+1]);
            if (bytes_inserted < 0) {
                free(sol);
                return NULL;
            }
            curr_chunk += bytes_inserted;

        } else {
            // for the last pair we don't want \n at the end.
            int bytes_inserted = sprintf(curr_chunk, "%ld,%ld", solution->data[i], solution->data[i+1]);
            if (bytes_inserted < 0) {
                free(sol);
                return NULL;
            }
            curr_chunk += bytes_inserted;
        }
    }
    return sol;
}

void convert_into_1array(sound_seg* track,int16_t* array){// converts ad sound_seg into 1 single int16 array(preping for dot_product);
    
    chunk* curr = track->head_node;
    size_t array_idx = 0;
    while(curr != NULL){
        for(size_t i=0; i < curr->len; i++){
            array[array_idx] = curr->data[i];
            array_idx++;
        }
    }
}

// Returns a string containing <start>,<end> ad pairs in target
char* tr_identify(sound_seg* target,sound_seg* ad){
    size_t ad_len = tr_length(ad);
    size_t target_len = tr_length(target);
    int16_t* ad_array = malloc(ad_len * sizeof(int16_t));
    int16_t* target_array = malloc(target_len * sizeof(int16_t));

    convert_into_1array(ad,ad_array);
    convert_into_1array(target,target_array);

    list_64t* solution = malloc(sizeof(list_64t)); //using a int64_t dynamic array to store index counts. as int16_t is too small
    init_int64_list(solution,100);
    
    double auto_correlation = dot_product(ad_array,ad_array,ad_len);

    int64_t curr_idx = 0;

    while(curr_idx +ad_len <= target_len){
        double correlation = dot_product(target_array+curr_idx,ad_array,ad_len); // shift target array pointer to new chunk

        double comparison_val = (correlation /auto_correlation) * 100.0;
        if (comparison_val >= 95.0 && comparison_val <= 105.0){ //if theres a match add to sol and skip entire ad_len to ensure no overlap.
            int size = solution->size;
            insert_int64(solution,size,curr_idx);
            insert_int64(solution,size+1,curr_idx + ad_len -1);
            curr_idx += ad_len;
        }
        else{
            curr_idx++;
        }
    }
    char* final_solution = solution_formatter(solution);
    free_int64_list(solution);
    free(target_array);
    free(ad_array);
    
    return final_solution;
}

double dot_product(int16_t* arr1,int16_t* arr2, size_t size){
    double cumml_sum = 0;
    for (size_t i =0; i<size; i++){
        cumml_sum += (double)arr1[i]*(double)arr2[i];
    }
    return cumml_sum;
}
chunk* find_node_with_pos(chunk* head, size_t pos,int* local_index, chunk** prev) { //returns the node and the index of where its located!
    chunk* curr = head;
    *prev = NULL;
    while (curr != NULL) {
        if (pos >= curr->start_index && pos < curr->start_index + curr->len) {
            *local_index = pos - curr->start_index;
            return curr;
        }
        *prev = curr;
        curr = curr->next;
    }
    printf("Pos not found within range");
    return NULL;
}


chunk* split(chunk* node_2_split, size_t local_split_pos, bool right_contain) {
    
    size_t split_index;
    if (right_contain) {
        // Split position should be included in the RIGHT chunk
        // So left chunk gets [0, local_split_pos), right gets [local_split_pos, end)
        split_index = local_split_pos;
    } else {
        // Split position should be included in the LEFT chunk  
        // So left chunk gets [0, local_split_pos], right gets [local_split_pos+1, end)
        split_index = local_split_pos + 1;
    }
    
    // Create the new right chunk
    chunk* new_right_chunk = chunk_init(
        node_2_split->data + split_index,           
        node_2_split->start_index + split_index,    
        node_2_split->len - split_index          
    );
    node_2_split->len = split_index;
    // Insert new_right_chunk into the linked list after the original chunk
    chunk* temp = node_2_split->next;
    node_2_split->next = new_right_chunk;
    new_right_chunk->next = temp;
    
    // Return the chunk that contains the split position
    if (right_contain) {
        return new_right_chunk; 
    } else {
        return node_2_split; 
    }
}

chunk* split_dest_track(sound_seg* dest_track, size_t destpos){//setsup dest_track for inserting by splitting dest around insert. Returns prev_node
    
    int local_index;
    chunk* prev;
    chunk* node = find_node_with_pos(dest_track->head_node,destpos,&local_index,&prev);
    if (node == NULL){
        printf("destpos outside dest_track range\n");
        return NULL;
    }
    if (local_index == 0){//already split can just insert one before.
        return prev;
    }
    else{
        split(node,local_index,true);
        return node;
    }
}
chunk* create_child_copy(chunk* parent_node){ //create child copy with same data as parent
    chunk* child_node = chunk_init(parent_node->data,0,parent_node->len); // set start_index as 0 will change at insertion.
    return child_node;
}

chunk* connect_chunk_list(chunk_list* children_nodes, chunk** head_ref, chunk* prev_node) {
    
    if (prev_node == NULL) {// If prev_node is NULL, we are inserting at the head
        // First child becomes new head
        *head_ref = children_nodes->data[0];
        chunk* curr = *head_ref;

        // Link the rest of the child nodes
        for (size_t i = 1; i < children_nodes->list_size; i++) {
            curr->next = children_nodes->data[i];
            curr = curr->next;
        }
        return curr; // Return last chunk
    } 
    else {
        // Normal insertion after prev_node
        chunk* curr = prev_node;
        for (size_t i = 0; i < children_nodes->list_size; i++) {
            curr->next = children_nodes->data[i];
            curr = curr->next;
        }
        return curr; // Return last child
    }
}

void recalc_start_indexes(sound_seg* track) {
    size_t index = 0;
    chunk* curr = track->head_node;
    while (curr) {
        curr->start_index = index;
        index += curr->len;
        curr = curr->next;
    }
}

void tr_insert(sound_seg* src_track,sound_seg* dest_track,size_t destpos, size_t srcpos, size_t len) {

    size_t track_start = srcpos;
    size_t track_end = srcpos + len - 1;

    chunk_list* children_nodes = malloc(sizeof(chunk_list));
    init_chunk_list(children_nodes,10); 
    
    chunk* curr_node = src_track->head_node;

    while(curr_node != NULL) {
        size_t chunk_start = curr_node->start_index;
        size_t chunk_end = curr_node->start_index + curr_node->len - 1;
        
        // Check if overlap between insert_track range and chunk
        if (track_start <= chunk_end && track_end >= chunk_start) { //case 0: skip if insert_track has zero overlap with curr_chunk
            size_t overlap_start = (track_start > chunk_start) ? track_start : chunk_start;
            size_t overlap_end = (track_end < chunk_end) ? track_end : chunk_end;

            //srcpos <= curr_arr_st && curr_arr_ed <= end_insert
            if (overlap_start == chunk_start && overlap_end == chunk_end){ //case 1: entire chunk is within insert_track region.
                chunk* temp_next = curr_node->next;
                chunk* insert_node = create_child_copy(curr_node);
                add_chunk(children_nodes,insert_node);

                curr_node = temp_next;
                //change this to a parent node and add to a list and insert it later.
            }

            else if (overlap_start > chunk_start && overlap_end == chunk_end){ //case 2: insert_track starts in middle of chunk.
                chunk* temp_next = curr_node->next;//store next so we can skip to the next unseen section of track.

                size_t local_split_pos = overlap_start - chunk_start;
                chunk* new_node = split(curr_node, local_split_pos, true);
                chunk* insert_node = create_child_copy(new_node);
                add_chunk(children_nodes,insert_node);

                curr_node = temp_next;

                //add new_node to a list and then insert it later. Also maybe update parents.
            }
            else if (overlap_start == chunk_start && overlap_end < chunk_end){ //case 3 insert track ends in middle of our chunk. 
                chunk* temp_next = curr_node->next;

                size_t local_split_pos = overlap_end - chunk_start;
                chunk* new_node = split(curr_node, local_split_pos, false);
                chunk* insert_node = create_child_copy(new_node);
                add_chunk(children_nodes,insert_node);

                curr_node = temp_next;
                //add new_node to a list and then insert it later. Also maybe update parents.
            }

            else if (overlap_start > chunk_start && overlap_end < chunk_end){ //case 4 entire track ends and starts in the middle of our chunk's array. |---\---\---|
                chunk* temp_next = curr_node->next;

                size_t local_split_start = overlap_start - chunk_start;
                chunk* left_new_node = split(curr_node, local_split_start, true);
                
                size_t local_split_end = overlap_end - overlap_start;
                split(left_new_node, local_split_end, false);
                
                chunk* insert_node_left = create_child_copy(left_new_node);
                add_chunk(children_nodes,insert_node_left);

                curr_node = temp_next;
            }
        }
        else{ //if not in overlap region move on normally.
            curr_node = curr_node->next;
        }
    }

    // 2nd phase of algo inserting copied nodes into dest.
    size_t len_dest = tr_length(dest_track);

    if (destpos == 0) {
        chunk* old_head = dest_track->head_node;
        chunk* last_chunk = connect_chunk_list(children_nodes, &dest_track->head_node, NULL);
        if (old_head != NULL){
            last_chunk->next = old_head;
        }
    } 
    else if (destpos < len_dest) {
        chunk* prev_node = split_dest_track(dest_track, destpos);
        
        chunk* temp = prev_node->next;
        chunk* last_chunk = connect_chunk_list(children_nodes, &dest_track->head_node, prev_node);
        last_chunk->next = temp;
    } 
    else if (destpos == len_dest) {
        chunk* curr = dest_track->head_node;
        while (curr->next != NULL) { // find tail
            curr = curr->next;
        }
        connect_chunk_list(children_nodes, &dest_track->head_node, curr);
    }

    recalc_start_indexes(dest_track);
    free_chunk_list(children_nodes);
    return;
}

void print_int16_array(const int16_t* arr, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void print_sound_seg(sound_seg* seg) {//prints the structure of sound_seg.
    if (seg == NULL || seg->head_node == NULL) {
        printf("sound_seg is empty.\n");
        return;
    }

    printf("Sound segment (total length: %zu):\n", tr_length(seg));

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




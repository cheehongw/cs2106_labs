#include <stdio.h>
#include <stdlib.h>
#include "llist.h"
#include "mymalloc.h"
#include "bitmap.h"

char _heap[MEMSIZE] = {0};
int map_size = MEMSIZE / 8;
unsigned char _bitmap[MEMSIZE / 8] = {0};
TNode *linked_list = NULL;

// Do not change this. Used by the test harness.
// You may however use this function in your code if necessary.
long get_index(void *ptr) {
    if(ptr == NULL)
        return -1;
    else
        return (long) ((char *) ptr - &_heap[0]);
}

void print_memlist() {
    print_map(_bitmap, map_size);
}

// Allocates size bytes of memory and returns a pointer
// to the first byte. 
void *mymalloc(size_t size) {
    TNode *node;
    TData *data;

    long start = search_map(_bitmap, map_size, size); // handle case where allocation fail
    
    if (start == -1) {
        return NULL;
    }
    

    allocate_map(_bitmap, start, size);

    data = (TData *) malloc(sizeof(TData));
    data->len = size;
    node = make_node(start, data);
    insert_node(&linked_list, node, ASCENDING);

    return _heap + start;
}

// Frees memory pointer to by ptr.
void myfree(void *ptr) {
    int start_idx = ((char*) ptr - _heap) / sizeof(char);

    TNode *node = find_node(linked_list, start_idx);

    if (node == NULL) { //cannot find pointer information
        return;
    }

    int length = node->pdata->len;
    delete_node(&linked_list, node);

    free_map(_bitmap, start_idx, length);
}


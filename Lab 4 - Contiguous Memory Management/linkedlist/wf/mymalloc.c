#include <stdio.h>
#include <stdlib.h>
#include "../mymalloc.h"
#include "../llist.h"


char _heap[MEMSIZE] = {0};
TNode *_memlist = NULL; // To maintain information about length

// Do not change this. Used by the test harness.
// You may however use this function in your code if necessary.
long get_index(void *ptr) {
    if(ptr == NULL)
        return -1;
    else
        return (long) ((char *) ptr - &_heap[0]);
}

void print_node(TNode *node) {
    
    char *status = node->pdata->status == 0 ? "FREE" : "ALLOCATED";
    unsigned int start_idx = node->key;
    int length = node->pdata->val;

    printf("Status: %s Start index: %d Length: %d\n", status, start_idx, length);
}

void print_memlist() {
    process_list(_memlist, &print_node);
}


//initializes the memlist to contain a initial node if none
void init_memlist() {
    TData *empty_mem_data;
    TNode *node;

    if (_memlist == NULL) {
        empty_mem_data = (TData *) malloc(sizeof(TData));
        empty_mem_data->val = MEMSIZE;
        empty_mem_data->status = 0;

        node = make_node(0, empty_mem_data);
        insert_node(&_memlist, node, ASCENDING);
    }

}


// Allocates size bytes of memory and returns a pointer
// to the first byte.
void *mymalloc(size_t size) {
    TData *data;
    unsigned int new_key;

    TNode *currWorstFit;
    int currWorst = 0;

    data = (TData*) malloc(sizeof(TData));
    data->status = 1;
    data->val = size;


    init_memlist();
    reset_traverser(_memlist, FRONT);

    while(1) {
        TNode *curr = succ(_memlist);

        if (curr == NULL) {
            break;
        }

        if ((curr->pdata->status == 0 && curr->pdata->val >= size) && 
                (curr->pdata->val > currWorst || currWorst == 0)) {

            currWorstFit = curr;
            currWorst = curr->pdata->val;
        }
    }

    if (currWorstFit == NULL) {
        return NULL;
    }


    int partition_size = currWorstFit->pdata->val;
    int status = currWorstFit->pdata->status;

    if (partition_size > size && status == 0) { //left over data
        TData *oldData = currWorstFit->pdata;

        TData *leftOverData = (TData*) malloc(sizeof(TData));
        leftOverData->status = 0;
        leftOverData->val = ((oldData->val) - size);
        new_key = (currWorstFit->key) + size;
        TNode *leftOverNode = make_node(new_key, leftOverData);
        insert_node(&_memlist, leftOverNode, ASCENDING);

        currWorstFit->pdata = data;  //update curr node to new data;
        free(oldData);
        return _heap + (currWorstFit->key);
    } else if (partition_size == size && status == 0) {
        TData *oldData = currWorstFit->pdata;
        currWorstFit->pdata = data;
        free(oldData);
        return _heap + (currWorstFit->key);
    }

    return NULL;
}

// Frees memory pointer to by ptr.
void myfree(void *ptr) {
    unsigned int key = ((char*) ptr) - _heap;
    reset_traverser(_memlist, FRONT);
    TNode *targetNode = find_node(_memlist, key);

    if (targetNode == NULL) {
        return;
    }

    TData *targetData = targetNode->pdata;

    TNode *prev = targetNode->prev;
    TNode *next = targetNode->next;

    targetData->status = 0;

    if (next != NULL && next->pdata->status == 0) {
        targetData->val = (targetData->val) + (next->pdata->val);
        free(next->pdata);
        merge_node(_memlist, targetNode, SUCCEEDING);
    }

    if (prev != NULL && prev->pdata->status == 0) {
        prev->pdata->val = (prev->pdata->val) + (targetData->val);
        free(targetData);
        merge_node(_memlist, targetNode, PRECEDING);
    }


}


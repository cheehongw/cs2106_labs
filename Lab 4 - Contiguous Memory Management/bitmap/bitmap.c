#include <stdio.h>
#include "bitmap.h"

// IMPLEMENTED FOR YOU
// Utility function to print out an array of char as bits
// Print the entire bitmap. Arguments: The bitmap itself, and the length of the bitmap 
// Each bit of the bitmap represents 1 byte of memory. 0 = free, 1 = allocated.
// map: The bitmap itself, an array of unsigned char.
//      Each bit of the bitmap represents one byte of memory
// len: The length of the bitmap array in characters
//
// Returns: Nothing
void print_map(unsigned char *map, int len) {
    int i, j;

    for(i=0; i<len; i++) {

        unsigned char mask = 0b10000000;
        for(j=0; j<8; j++) {
            if(map[i] & mask)
                printf("1");
            else
                printf("0");
            mask = mask >> 1;
        }
        printf(" ");
    }
    printf("\n");
}

// Search the bitmap for the required number of zeroes (representing
// free bytes of memory). Returns index of first stretch of 0s
// that meet the criteria. You can use this as an index into
// an array of char that represents the process heap
// bitmap = Bitmap declared as an array of unsigned char
// len = Length of bitmap in characters
// num_zeroes = Length of string of 0's required
// Returns: Index to stretch of 0's of required length, -1 if no such stretch can be found

long search_map(unsigned char *bitmap, int len, long num_zeroes) {
    long i = 0;

    unsigned char mask = 0b10000000;
    long consec_zero_count = 0;

    for (i = 0; i < (len * 8); i++) {
        if (consec_zero_count == num_zeroes) {
            // printf("found it: index %ld \n", i);
            return i - consec_zero_count;
        }

        if (bitmap[i / 8] & mask) {
            consec_zero_count = 0;
        } else {
            // printf("loop\n");
            consec_zero_count += 1;
        }
        mask = mask >> 1;
        if (!mask) mask = 0b10000000;

    }

    return -1;
} //main

// Set map bits to 0 or 1 depending on whether value is non-zero
// map = Bitmap, declared as an array of unsigned char
// start = Starting index to mark as 1 or 0
// length = Number of bits to mark
// value = Value to mark the bits as. value = 0 marks the bits
//          as 0, non-zero marks the bits as 1
// Returns: Nothing

void set_map(unsigned char *map, long start, long length, int value) {

    int i;
    unsigned char mask = 0b10000000;
    int offset_start_bit = start % 8;

    mask = mask >> offset_start_bit;


    for (int i = start; i < start + length; i++) {
        int curr_idx = i / 8;
        //printf("%d\n", curr_idx);
        if (map[curr_idx] & mask) { // current bit is 1
            if (value == 0) {
                map[curr_idx] = map[curr_idx] ^ mask; 
            }
        } else { //current bit is 0
            if (value == 1) {
                map[curr_idx] = map[curr_idx] | mask; 
            }
        }
        mask = mask >> 1;
        if (!mask) mask = 0b10000000;
    }

}

// IMPLEMENTED FOR YOU
// Marks a stretch of bits as "1", representing allocated memory
// map = Bitmap declared as array of unsigned char
// start = Starting index to mark
// length = Number of bits to mark as "1"
void allocate_map(unsigned char *map, long start, long length) {

    set_map(map, start, length, 1);

}

// IMPLEMENTED FOR YOU
// Marks a stretch of bits as "0", representing allocated memory
// map = Bitmap declared as array of unsigned char
// start = Starting index to mark
// length = Number of bits to mark as "0"
void free_map(unsigned char *map, long start, long length) {
    set_map(map, start, length, 0);
}


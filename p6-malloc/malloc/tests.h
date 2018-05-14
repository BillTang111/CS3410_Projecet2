#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "heaplib.h"

#define HEAP_SIZE 1024
#define ARRAY_LEN 16
#define SMALL_BLOCK_SIZE 2

// TODO: Add test descriptions as you add more tests...
const char* testDescriptions[] = {
    "init: heap should be created when enough space",
    "init: heap should not be created when not enough space",
    "alloc: block should be allocated when enough space",
    "alloc: block should not be allocated when not enough space",
    "alloc: block returned is aligned"
};

/* Checks whether a "heap" is created when there IS enough space.
 * THIS TEST IS COMPLETE.
 */
int test01() {

    char heap[HEAP_SIZE];

    int heap_created_f = hl_init(heap, HEAP_SIZE);

    if (heap_created_f) {
        return SUCCESS;
    }
    return FAILURE;
}

/* Checks whether a "heap" is created when there IS NOT enough space.
 * THIS TEST IS NOT COMPLETE. heaplame.c does not pass this test. Feel free to fix it!
 * Lab 12 TODO: COMPLETE THIS TEST!
 */
int test02() {

    char heap[HEAP_SIZE];
    
    int heap_created_f = hl_init(heap, 1);
    
    if (heap_created_f == 0) {
        return SUCCESS;
    }
    return FAILURE;
}

/* Checks whether a block can be allocated when there is enough space.
 * THIS TEST IS NOT COMPLETE.
 * Lab 12 TODO: COMPLETE THIS TEST!
 */
int test03() {

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);
    
    void* block = hl_alloc(heap, ARRAY_LEN);
    
    if (block == NULL) {
        return FAILURE;
    }
    return SUCCESS; 
}

/* Checks whether a block can be allocated when there is NOT enough space.
 * THIS TEST IS COMPLETE. heaplame.c does not pass this test. Feel free to fix it!
 */
int test04() {

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // should NOT work
    void* block = hl_alloc(heap, ARRAY_LEN * HEAP_SIZE);

    if (block == NULL) {
        return SUCCESS;
    }
    return FAILURE;
}

/* Checks whether hl_alloc returns a pointer that has the correct
 * alignment.
 * THIS TEST IS NOT COMPLETE. heaplame.c does not pass this test. Feel free to fix it!
 * LAB 12 TODO: COMPLETE THIS TEST! (it is not robust)
 */
int test05() {

    char array[HEAP_SIZE];
    char* ptr = array;

    hl_init(array, HEAP_SIZE - 1);
    
    void* block1;
    void* block2;
    void* block3;
    void* block4;
    void* block5;
    void* block6;
    void* block7;
    void* block8;
    void* block9;
    void* block10;
    void* block11;
    void* block12;
    void* block13;
    void* block14;
    void* block15;
    void* block16;
    void* block17;
    void* block18;
    void* block19;
    void* block20;
    
    block1 = hl_alloc(ptr, 1);
    block2 = hl_alloc(ptr, 2); 
    block3 = hl_alloc(ptr, 3); 
    block4 = hl_alloc(ptr, 4);
    block5 = hl_alloc(ptr, 5); 
    block6 = hl_alloc(ptr, 6);
    block7 = hl_alloc(ptr, 7);
    block8 = hl_alloc(ptr, 8);
    block9 = hl_alloc(ptr, 9);
    block10 = hl_alloc(ptr, 10);
    block11 = hl_alloc(ptr, 11);
    block12 = hl_alloc(ptr, 12);
    block13 = hl_alloc(ptr, 13);
    block14 = hl_alloc(ptr, 14);
    block15 = hl_alloc(ptr, 15);
    block16 = hl_alloc(ptr, 16);
    block17 = hl_alloc(ptr, 17);
    block18 = hl_alloc(ptr, 18);
    block19 = hl_alloc(ptr, 19);
    block20 = hl_alloc(ptr, 20);
    
    bool aligned_1 = false;
    bool aligned_2 = false;
    bool aligned_3 = false;
    bool aligned_4 = false;
    bool aligned_5 = false;
    bool aligned_6 = false;
    bool aligned_7 = false;
    bool aligned_8 = false;
    bool aligned_9 = false;
    bool aligned_10 = false;
    bool aligned_11 = false;
    bool aligned_12 = false;
    bool aligned_13 = false;
    bool aligned_14 = false;
    bool aligned_15 = false;
    bool aligned_16 = false;
    bool aligned_17 = false;
    bool aligned_18 = false;
    bool aligned_19 = false;
    bool aligned_20 = false;
    
    if (block1 != NULL) {
    aligned_1 = !((unsigned long)block1 & (ALIGNMENT - 1));
    if (!aligned_1) { return FAILURE;} }
    if (block2 != NULL) {
    aligned_2 = !((unsigned long)block2 & (ALIGNMENT - 1));
    if (!aligned_2) { return FAILURE;} }
    if (block3 != NULL) {
    aligned_3 = !((unsigned long)block3 & (ALIGNMENT - 1));
    if (!aligned_3) { return FAILURE;} }
    if (block4 != NULL) {
    aligned_4 = !((unsigned long)block4 & (ALIGNMENT - 1));
    if (!aligned_4) { return FAILURE;} }
    if (block5 != NULL) {
    aligned_5 = !((unsigned long)block5 & (ALIGNMENT - 1));
    if (!aligned_5) { return FAILURE;} }
    if (block6 != NULL) {
    aligned_6 = !((unsigned long)block6 & (ALIGNMENT - 1));
    if (!aligned_6) { return FAILURE;} }
    if (block7 != NULL) {
    aligned_7 = !((unsigned long)block7 & (ALIGNMENT - 1));
    if (!aligned_7) { return FAILURE;} }
    if (block8 != NULL) {
    aligned_8 = !((unsigned long)block8 & (ALIGNMENT - 1));
    if (!aligned_8) { return FAILURE;} }
    if (block9 != NULL) {
    aligned_9 = !((unsigned long)block9 & (ALIGNMENT - 1));
    if (!aligned_9) { return FAILURE;} }
    if (block10 != NULL) {
    aligned_10 = !((unsigned long)block10 & (ALIGNMENT - 1));
    if (!aligned_10) { return FAILURE;} }
    if (block11 != NULL) {
    aligned_11 = !((unsigned long)block11 & (ALIGNMENT - 1));
    if (!aligned_11) { return FAILURE;} }
    if (block12 != NULL) {
    aligned_12 = !((unsigned long)block12 & (ALIGNMENT - 1));
    if (!aligned_12) { return FAILURE;} }
    if (block13 != NULL) {
    aligned_13 = !((unsigned long)block13 & (ALIGNMENT - 1));
    if (!aligned_13) { return FAILURE;} }
    if (block14 != NULL) {
    aligned_14 = !((unsigned long)block14 & (ALIGNMENT - 1));
    if (!aligned_14) { return FAILURE;} }
    if (block15 != NULL) {
    aligned_15 = !((unsigned long)block15 & (ALIGNMENT - 1));
    if (!aligned_15) { return FAILURE;} }
    if (block16 != NULL) {
    aligned_16 = !((unsigned long)block16 & (ALIGNMENT - 1));
    if (!aligned_16) { return FAILURE;} }
    if (block17 != NULL) {
    aligned_17 = !((unsigned long)block17 & (ALIGNMENT - 1));
    if (!aligned_17) { return FAILURE;} }
    if (block18 != NULL) {
    aligned_18 = !((unsigned long)block18 & (ALIGNMENT - 1));
    if (!aligned_18) { return FAILURE;} }
    if (block19 != NULL) {
    aligned_19 = !((unsigned long)block19 & (ALIGNMENT - 1));
    if (!aligned_19) { return FAILURE;} }
    if (block20 != NULL) {
    aligned_20 = !((unsigned long)block20 & (ALIGNMENT - 1));
    if (!aligned_20) { return FAILURE;} } 

return SUCCESS;
}

/* Your test.
 * Test whether hl_init() supports multiple heaps. 
 * Also adds test case 2 to our multiple heap tests. */
int test06() {
    // create mutliple heaps and return FAILURE if the returned heapptrs are NULL
    char heap[HEAP_SIZE];
    int QUARTER_HEAPSIZE = HEAP_SIZE/4;
    char* ptr1 = heap;
    char* ptr2 = (char*) heap + QUARTER_HEAPSIZE;
    char* ptr3 = (char*) heap + 2*QUARTER_HEAPSIZE;
    char* ptr4 = (char*) heap + 3*QUARTER_HEAPSIZE;
    
    int heap_created_1 = hl_init(ptr1, QUARTER_HEAPSIZE);
    int heap_created_2 = hl_init(ptr2, QUARTER_HEAPSIZE);
    int heap_created_3 = hl_init(ptr3, QUARTER_HEAPSIZE);
    int heap_created_4 = hl_init(ptr4, 1); // this init will fail b/c heap_size is too small
    
    if (heap_created_1 == 0 || heap_created_2 == 0 || heap_created_3 == 0) {
        return FAILURE;
    }

    if (heap_created_4 != 0) {
        return FAILURE;
    }

    return SUCCESS;

}

/* Your test.
 * test if hl_release works robustly.
 */
int test07() {
    
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);
    void* blockptr;
    void* first_block = hl_alloc(heap, sizeof(char));
    
    // allocate space until full
    while (1) {
        blockptr = hl_alloc(heap, sizeof(char));
        if (blockptr == NULL) {
            break;
        }
    }
    
    // release the first block and alloc
    hl_release(heap, first_block);
    
    if (hl_alloc(heap, sizeof(char)) == NULL) {
        return FAILURE;
    }
    
    // test that hl_release() does nothing when blockptr is zero
    hl_release(heap, 0);
    if (hl_alloc(heap, sizeof(char)) != NULL) {
        return FAILURE;
    }

    return SUCCESS;

}

/* Your test.
 * test if resize behaves as expected when the heap has enough space.
 * 
 */
int test08() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    
    void* blockptr = hl_alloc(heap, ARRAY_LEN); // correct???
    char* charptr = (char*) blockptr;

    for (int i=0; i<ARRAY_LEN; i++) {
        charptr[i] = 'q';
    }

    // resize block to a larger block size
    blockptr = hl_resize(heap, blockptr, 2*ARRAY_LEN);
    char* charptr2 = (char*) blockptr;
    if (blockptr == NULL) {
        return FAILURE;
    }
    
    // check each element to see if data are preserved
    for (int i=0; i<ARRAY_LEN; i++) {
        if (charptr2[i] != 'q') {
            return FAILURE;
        }
    }
    
    // if I can write beyond ARRAY_LEN
    for (int i=ARRAY_LEN; i<2*ARRAY_LEN; i++) {
        charptr2[i] = 'z';
    }

    // resize to original size
    blockptr = hl_resize(heap, blockptr, ARRAY_LEN);
    char* charptr3 = (char*) blockptr;
    if (blockptr == NULL) {
        return FAILURE;
    }

    // check each element to see if data are preserved
    for (int i=0; i<ARRAY_LEN; i++) {
        if (charptr3[i] != 'q') {
            return FAILURE;
        }
    }

    // resize to a smaller size, should return a non-NULL block pointer
    blockptr = hl_resize(heap, blockptr, SMALL_BLOCK_SIZE);
    if (blockptr == NULL) {
        return FAILURE;
    }

    return SUCCESS;

}

/* Your test.
 * test if hl_resize behaves as expected when the heap does not have enough space
 */
int test09() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    
    void* blockptr = hl_alloc(heap, ARRAY_LEN);
    blockptr = hl_resize(heap, blockptr, HEAP_SIZE*HEAP_SIZE);

    // blockptr should be NULL because new block size is too large
    if (blockptr == NULL) {
        return SUCCESS;
    }
    return FAILURE;
}

/* Your test.
 * test corner cases when block size and block pointer are zero for hl_alloc()
 */
int test10() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    
    // blockptr should not be NULL
    void* blockptr = hl_alloc(heap, 0);
    if (blockptr == NULL) {
        return FAILURE;
    }

    // blockptr should be 8-byte aligned
    bool aligned = false;
    aligned = !((unsigned long)blockptr & (ALIGNMENT - 1));
    if (!aligned) {
        return FAILURE;
    }
    
    hl_release(heap, blockptr);
    blockptr = hl_alloc(heap, 0);

    // resize the block of size from 0 to 2*ARRAY_LEN
    void* blockptr_double = hl_resize(heap, blockptr, 2*ARRAY_LEN);
    if (blockptr_double == NULL) {
        return FAILURE;
    }
    bool aligned_double = false;
    aligned_double = !((unsigned long)blockptr_double & (ALIGNMENT - 1));
    if (!aligned_double) {
        return FAILURE;
    }
    char* charptr = (char*) blockptr_double;
    // store data onto the heap
    for (int i=0; i<ARRAY_LEN; i++) {
        charptr[i] = 'q';
    }
    
    // resize the block to size of ARRAY_LEN
    void* blockptr_arraylen = hl_resize(heap, blockptr_double, ARRAY_LEN);
    if (blockptr_arraylen == NULL) {
        return FAILURE;
    }
    bool aligned_arraylen = false;
    aligned_arraylen = !((unsigned long)blockptr_arraylen & (ALIGNMENT - 1));
    if (!aligned_arraylen) {
        return FAILURE;
    }
    // check if data are preserved
    char* charptr2 = (char*) blockptr_arraylen;
    for (int i=0; i<ARRAY_LEN; i++) {
        if (charptr2[i] != 'q') {
            return FAILURE;
        }
    }

    hl_release(heap, blockptr_arraylen);

    return SUCCESS;

}

/* Your test.
 * test if hl_resize() works like hl_alloc() when block pointer is zero
 */
int test11() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    void* blockptr = hl_resize(heap, NULL, ARRAY_LEN);
    void* blockptr2 = hl_resize(heap, NULL, HEAP_SIZE * 2);
    
    if (blockptr == NULL) {
        return FAILURE;
    }
    if(blockptr2 != NULL) {
        return FAILURE;
    }

    bool aligned = false;
    aligned = !((unsigned long) blockptr & (ALIGNMENT - 1));
    if (!aligned) {
        return FAILURE;
    }
    
    hl_release(heap, blockptr); 
    hl_release(heap, blockptr2);
    hl_release(heap, 0);
    
    blockptr = hl_resize(heap, NULL, ARRAY_LEN);
    blockptr2 = hl_resize(heap, NULL, HEAP_SIZE * 2);
    
    if (blockptr == NULL) {
        return FAILURE;
    }
    if(blockptr2 != NULL) {
        return FAILURE;
    }
    aligned = !((unsigned long) blockptr & (ALIGNMENT - 1));
    if (!aligned) {
        return FAILURE;
    }   
    return SUCCESS;
}

/* Your test.
 */
int test12() {

    // check on a heap not 8-byte aligned
    char heap2[HEAP_SIZE];
    char* heapptr_not_aligned = (char*)heap2 + 3;
    hl_init(heapptr_not_aligned, HEAP_SIZE/3 + 1);

    void* blockptr2 = hl_alloc(heapptr_not_aligned, 0);
    blockptr2 = hl_resize(heapptr_not_aligned, blockptr2, ARRAY_LEN - 1);
    bool aligned2 = false;
    aligned2 = !((unsigned long) blockptr2 & (ALIGNMENT - 1));
    if (!aligned2) {
        return FAILURE;
    }
    
    // check on a heap not 8-byte aligned, passing hl_alloc() a nonzero block size
    char heap3[HEAP_SIZE];
    char* heapptr_not_aligned_2 = (char*)heap3 + 5;
    hl_init(heapptr_not_aligned_2, HEAP_SIZE/3 + 2);

    void* blockptr3 = hl_alloc(heapptr_not_aligned_2, ARRAY_LEN + 3);
    bool aligned3 = false;
    aligned3 = !((unsigned long) blockptr3 & (ALIGNMENT - 1));
    if (!aligned3) {
        return FAILURE;
    }

    return SUCCESS;
}

/* Your test.
 */
int test13() {

    return SUCCESS;

}

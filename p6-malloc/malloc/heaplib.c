#include "heaplib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>


/* This file implements four major functions by using the explicit list method 
 * with header and footer. This method uses bidirectional coalescing 
 */

/*Use macros to define terms */
//For debug:
#define PRINT_DEBUG
#define MIN_HEAP_SIZE 1024
#define ALIGNMENT 8




/* Useful shorthands */
/* Casts a pointer to a (char *) before adding */
#define ADD_BYTES(base_addr, num_bytes) (((char *)(base_addr)) + (num_bytes))

/* Align the address to 8-bytes by taking the upper bound (address => unsigned long)*/
#define ALIGN(ptr) (((unsigned long)ptr + ALIGNMENT - 1) & ~(ALIGNMENT-1))

/* Check if a block is free */
#define IS_FREE(blk_size) ((blk_size & 1) == 0)

/* -------------------- PRINT DEBUG FNS ----------------- */

/* Debug print functions.  The entire body is
 * wrapped in an #ifdef so that the default compile does not include
 * them.
 */
void print_ooooo(int a) {
#ifdef PRINT_DEBUG
printf("YOU input is: %d \n", a);
#endif
}

/* ------------------- Model Structure  ----------------- */

/* Given the heap header & a block address, finds the block in
 * question. Returns BLOCK_NOT_FOUND if not found.
 *
 */
typedef struct {
    size_t heap_size; //size of the heap
    struct block_header* fst_block; // point to the first block in the heap
} heap_header;

typedef struct {
    size_t block_size; //size of block (addre is 8-byte alligned)
    struct block_header* prev; //points to previous empty block
    struct block_header* next; //points to next empty block 
} block_header;

typedef struct {
    size_t block_size; //size of block (addre is 8-byte alligned)
} block_footer;


/* -------------------- THE BIG FOUR FNS ----------------- */
/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * Initializes the heap size (after asserting size >= MIN_HEAP_SIZE).
 * Initializes block information for N_SUPPORTED_BLOCKS, which start
 * empty ( = size of 0, NULL pointer, and not in use).
 */
/* -------------------- hl_init ----------------- */
void hl_init(void *heap, unsigned int heap_size) {
    //check the constraints 
    if(heap_size < MIN_HEAP_SIZE || !heap){
        printf("Ahh, your input heap_size is wrong \n");
        return;
    }

    // start setting up the heap
    heap_header* heap_head = (heap_header*) heap;
    heap_head -> heap_size = heap_size;

    //calc the first block starting point after the heap_header
    void* fst_blk_unAlign = ADD_BYTES(heap_head, sizeof(heap_header));
    unsigned long offset = ALIGN(fst_blk_unAlign) - (unsigned long)fst_blk_unAlign;
    block_header* fst_blk_Align = (block_header *)(ADD_BYTES(heap_head, sizeof(heap_header) + offset));
    heap_head -> fst_block = fst_blk_Align;

    //initialize the first block
    // block_header
    // avail size = heap_size - sizeof(block_header) - sizeof(block_footer) - |fst_blk_Align|
    size_t size_available = (unsigned long)ADD_BYTES(heap_head, heap_size - sizeof(block_footer)) 
        - (unsigned long)ADD_BYTES(fst_blk_Align, sizeof(block_header));
    size_t size_avai_aligned = size_available == ALIGN(size_available)?size_available : ALIGN(size_available)-ALIGNMENT;
    fst_blk_Align -> block_size = size_avai_aligned+sizeof(block_footer)+sizeof(block_header);
    fst_blk_Align -> prev = NULL;
    fst_blk_Align -> next = NULL;

    // block_footer
    block_footer* fst_blk_footer = (block_footer*)ADD_BYTES(fst_blk_Align, 
        fst_blk_Align -> block_size - sizeof(block_footer));

    #ifdef PRINT_DEBUG
        printf("heap starts at addr %p\n", heap);
        // printf("block_header_size is  %d\n", sizeof(block_header));
        printf("first_block starts at  %p\n", fst_blk_Align);
        // printf("size_avai_aligned is  %d\n", size_avai_aligned);
        // printf("block size is  %d\n", fst_blk_Align -> block_size);
        printf("fst_blk_footer is  %p\n", fst_blk_footer);
    #endif

    return;
}

/* ------------------- HELPER FUNCTIONS ----------------- */

/*  Calculate the size of a block needed */
size_t calc_needed_size (unsigned int payload_size){
    size_t base_size = sizeof(block_header)+sizeof(block_footer);
    //calculate the required size after aligning (2*ALIGNMENT for *prev and *next)
    size_t needed_size = (size_t)(ALIGN(payload_size)+base_size-2*ALIGNMENT);
    if(needed_size < base_size){
        needed_size = base_size;
    }
    return needed_size;
}

/*  Finded the first freed block that meets the required payload size*/
block_header* find_block (block_header* lst_start, unsigned int payload_size){
    block_header* current_block = lst_start;
    size_t size_needed = calc_needed_size(payload_size);

    while(current_block != NULL){
        size_t current_size = lst_start -> block_size;
        if (size_needed > current_size){
            current_block = current_block -> next;
        }else{
            return current_block;
        }
    }
    return NULL;
} 

/* Insert the newly freed block by address order. */
block_header *insert(block_header *new_blk, block_header *head){

	if(new_blk == NULL) return head;
	if(head == NULL){
		new_blk -> prev = NULL;
		new_blk -> next = NULL;
		return new_blk;
	}
	block_header *new_head = new_blk;
    new_blk -> next = head;
    new_blk -> prev = NULL; 
	head -> prev = new_blk;

    return new_head;
}

/* -------------------- hl_alloc ----------------- */
void *hl_alloc(void *heap, unsigned int block_size) {

    //check constraints
    if(block_size == 0){ return heap; }

    //locate the first block from heap
    heap_header* heap_head = (heap_header*) heap;
    block_header* list_head = heap_head -> fst_block;

    //find available block address meeting our "block_size"
    block_header* tgt_blk = find_block (list_head, block_size);
    if (tgt_blk == NULL){ return NULL; }

    //find this block's size
    size_t cur_blk_size = tgt_blk -> block_size;
    //find the Minimum block size needed for "block_size"
    size_t Min_size = calc_needed_size(block_size);

    //Modify the memory
    //If the free block's size is greater than size we needed -> split the block
    if (cur_blk_size - Min_size > calc_needed_size(0)){
        //store the old block information for calcution reason
        size_t old_size = tgt_blk->block_size;
        block_footer* old_footer = (block_footer*) ADD_BYTES(tgt_blk, old_size - sizeof(block_footer));

        // update the header of tgt_block
        tgt_blk -> block_size = Min_size;
        // indicate the current block as unfreed
        tgt_blk -> block_size = tgt_blk -> block_size | 1;

        //create a footer for this newly allocated block
        block_footer* tgt_blk_footer = (block_footer*) ADD_BYTES(tgt_blk, Min_size-sizeof(block_footer));
        tgt_blk_footer -> block_size = tgt_blk -> block_size;

        //create a new header for the free splitted block;
        block_header* splt_header = (block_header*) ADD_BYTES(tgt_blk, Min_size);
        splt_header -> block_size = old_size - Min_size;
        splt_header -> prev = tgt_blk -> prev;
        splt_header -> next = tgt_blk -> next;

        //update the dependencies 
        if (tgt_blk -> prev != NULL)
            {
                block_header* temp = (tgt_blk -> prev);
                temp -> next = splt_header;
            }
        if (tgt_blk -> next != NULL)
            {
                block_header* temp = (tgt_blk -> next);
                temp  -> prev = splt_header;
            }

        //update the old_footer for now_splitted block
        old_footer -> block_size = old_size - Min_size;

        //if tgt_block is the starting blk in the heap -> update the heap header 
        if (tgt_blk -> prev == NULL){
            heap_head -> fst_block  = splt_header;
        }
    }else{ //No need to split; just remove the tgt_blk from the free block list
        //inidicating no free anymore
        block_footer* footer = (block_footer*) ADD_BYTES(tgt_blk, (tgt_blk -> block_size) - sizeof(block_footer));
        tgt_blk -> block_size = tgt_blk -> block_size | 1;
        footer -> block_size = tgt_blk -> block_size;

        //update the dependencies 
        if (tgt_blk -> prev != NULL)
            {
                block_header* temp = (tgt_blk -> prev);
                temp -> next = tgt_blk->next;
            }
        if (tgt_blk -> next != NULL)
            {
                block_header* temp = (tgt_blk -> next);
                temp -> prev = tgt_blk->prev;
            }
        //if tgt_block is the starting blk in the heap -> update the heap header 
        if (tgt_blk -> prev == NULL){
            heap_head -> fst_block  = tgt_blk->next;
        }
    }

    //Find a pointer to the block of memory (data section) for the return
    void * alloc_block_addr = ADD_BYTES(tgt_blk, ALIGNMENT);

    #ifdef PRINT_DEBUG
        printf("heap starts at addr %p\n", heap);
        printf("alloc_block_addr starts at  %p\n", alloc_block_addr);
        // printf("cur_blk_size is  %d\n", cur_blk_size);
    #endif

    return alloc_block_addr;
}

/* -------------------- hl_release ----------------- */
void hl_release(void *heap, void *block) {
    // check constraint 
    if (block == NULL) return;

    // releasing a previous allocaiton -> do nothing
    if((unsigned long)block == ALIGN(heap)) return;

    // retrieve the pointer
    heap_header* heap_head = (heap_header*)heap;
    block_header* block_free_hd = (block_header *)(ADD_BYTES(block, -ALIGNMENT));

    // indicating the block is freed (update the last bit)
    block_free_hd -> block_size = (block_free_hd -> block_size) & ~(1);

    block_footer* block_free_ft = (block_header *)
        (ADD_BYTES(block, (block_free_hd -> block_size)-sizeof(block_footer)));
    block_free_ft -> block_size = block_free_hd -> block_size;

    //link the freed block back to the list and check freeness
    // check head 
    block_footer* prev_footer = (block_footer*) ADD_BYTES(block_free_hd, -sizeof(block_footer));
    int is_prev_free = ((unsigned long)prev_footer <= (unsigned long)ADD_BYTES(heap, sizeof(heap_header)))
        ? 0 : IS_FREE(prev_footer-> block_size);

    // check the footer
    block_header* next_header = (block_header*)ADD_BYTES(block_free_hd, (block_free_hd->block_size)); 
    int is_next_free = ((unsigned long)next_header >= heap_head -> heap_size) 
         ? 0 : IS_FREE(next_header -> block_size);

    //Coalescing blocks if needed (is_x_free = 1): 4 cases
    if (is_prev_free && !is_next_free){

        //retrive the prev_block_header
        size_t prev_blk_size = prev_footer -> block_size;
        block_header* prev_block_header = (block_header*) ADD_BYTES(block_free_hd, - prev_blk_size);
        block_footer* next_footer = (block_footer *)ADD_BYTES(block_free_hd, (block_free_hd -> block_size) - sizeof(block_footer));
        //merge prev_block and update the size in header and footer
        size_t new_size = prev_blk_size + (block_free_hd -> block_size);
        prev_block_header -> block_size = new_size;
        next_footer -> block_size = new_size;

    }else if(!is_prev_free && is_next_free){

        //merge next_block and update the header and footer size
        size_t new_size = block_free_hd -> block_size + next_header -> block_size;
        block_free_hd -> block_size = new_size;
        block_footer* next_footer = 
            (block_footer*) ADD_BYTES(next_header, next_header -> block_size - sizeof(block_footer));
        next_footer -> block_size = new_size;

        // update the dependencies
        block_free_hd -> prev = next_header -> prev;
        block_free_hd -> next = next_header -> next;

        if (next_header -> prev != NULL)
            {
                block_header* temp = (next_header -> prev);
                temp -> next = block_free_hd;
            }
        if (next_header -> next != NULL)
            {
                block_header* temp = (next_header -> next);
                temp -> prev = block_free_hd;
            }
        //update the heap header 
        if (next_header -> prev == NULL){
            heap_head -> fst_block  = block_free_hd;
        }

    }else if (is_prev_free && is_next_free){
        //retrive the prev_block_header and next_footer
        size_t prev_blk_size = prev_footer -> block_size;
        block_header* prev_block_header = (block_header*)ADD_BYTES(block_free_hd, - prev_blk_size);
        block_footer* next_footer = (block_footer*)ADD_BYTES(next_header, next_header -> block_size - sizeof(block_footer));

        //merge next_block and update the header and footer size
        size_t new_size = prev_block_header -> block_size + next_footer -> block_size + block_free_hd -> block_size;
        prev_block_header -> block_size = new_size;
        next_footer -> block_size = new_size;

        // update external dependencies
        if(prev_block_header -> prev != NULL){
            block_header* temp = (prev_block_header -> prev);
    		temp -> next = prev_block_header -> next;
        }else{ 
    		heap_head -> fst_block = prev_block_header -> next;
        }

    	if(prev_block_header -> next != NULL){
            block_header* temp = (prev_block_header -> next);
    		temp -> prev = prev_block_header -> prev;
        }

    	if(next_header -> prev != NULL){ 
            block_header* temp = (next_header -> prev);
    		temp -> next = next_header -> next;
        }else{ 
    		heap_head -> fst_block = next_header -> next;
        }

        if (next_header -> next != NULL){
                block_header* temp = (next_header -> next);
                temp -> prev = next_header->prev;
            }
        
        // insert the new block at the root of the list
        heap_head -> fst_block = insert(prev_block_header, heap_head -> fst_block);

    }else{
        heap_head -> fst_block = insert(block_free_hd, heap_head-> fst_block);
    }

    #ifdef PRINT_DEBUG
        printf("heap_head -> fst_block starts at addr %p\n", heap_head -> fst_block);
        printf("block_free_hd starts at  %p\n", block_free_hd);
    #endif

}

/* -------------------- hl_resize ----------------- */
void *hl_resize(void *heap, void *block, unsigned int new_size) {
    // // check constraints
    // if(heap == NULL) return FAILURE;
    // if(block == NULL) return hl_alloc(heap, new_size);
    // // resizing the size 0 allocation 
    // if(ALIGN(heap)==(unsigned long)block) return hl_alloc(heap, new_size);

    // if(new_size == 0 ){
    //     hl_release(heap,block);
    //     return (void*)ALIGN(heap);
    // }

    // // acquire pointers
    // block_header* block_hd = (block_header*)(ADD_BYTES(block,-sizeof(block_header)+2*ALIGNMENT));
    // heap_header* heap = (heap_header*) heap;
    // void* result_pt = block; 

    // // get the minimum block size for the current block
    // size_t new_block_size = calc_needed_size(new_size);
    // // retrieve the old block size
    // size_t block_size = (block_hd-> block_size)& ~(1);

    // // starting resize procedure
    // // case 1
    // if(new_block_size == block_size) {return block;
    // }else if(new_block_size < block_size) { 
    // // case 2: split the block as hl_alloc did


    // }
    return NULL;
}

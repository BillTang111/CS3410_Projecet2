#include "heaplib.h"

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

/* ------------------- HELPER FUNCTIONS ----------------- */

/*  get the size of a block */


/* ------------------- Model Structure  ----------------- */

/* Given the heap header & a block address, finds the block in
 * question. Returns BLOCK_NOT_FOUND if not found.
 *
 */
typedef struct _heap_header{
    size_t heap_size; //size of the heap
    struct block_header* fst_block; // point to the first block in the heap
} heap_header ;

typedef struct _block_header {
    size_t block_size; //size of block (addre is 8-byte alligned)
    struct block_header* prev; //points to previous empty block
    struct block_header* next; //points to next empty block 
} block_header;

typedef struct _block_footer {
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
        printf("block_header_size is  %d\n", sizeof(block_header));
        printf("first_block starts at  %p\n", fst_blk_Align);
        printf("size_avai_aligned is  %d\n", size_avai_aligned);
        printf("block size is  %d\n", fst_blk_Align -> block_size);
        printf("fst_blk_footer is  %p\n", fst_blk_footer);
    #endif

    return;
}


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

/* -------------------- hl_alloc ----------------- */
/* Allocates a block of memory of size block_size bytes from the heap starting
 * at 'heap'. Returns a pointer to the block on success; returns
 * 0 if the allocator cannot satisfy the request.
 *
 * PRECONDITIONS:
 * (1) hl_init must have been called exactly once for this heap
 * If preconditions are violated, non-graceful failure is acceptable.
 */
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
        block_footer* old_footer = ADD_BYTES(tgt_blk, old_size - sizeof(block_footer));

        // update the header of tgt_block
        tgt_blk -> block_size = Min_size;
        // indicate the current block as unfreed
        tgt_blk -> block_size = tgt_blk -> block_size | 1;

        //create a footer for this newly allocated block
        block_footer* tgt_blk_footer = ADD_BYTES(tgt_blk, Min_size-sizeof(block_footer));
        tgt_blk_footer -> block_size = Min_size;

        //create a new header for the free splitted block;
        block_header* splt_header = ADD_BYTES(tgt_blk, Min_size);
        splt_header -> block_size = old_size - Min_size;
        splt_header -> prev = tgt_blk -> prev;
        splt_header -> next = tgt_blk -> next;

        //update the dependencies 
        if (tgt_blk -> prev != NULL){(tgt_blk -> prev) -> next = splt_header;}
        if (tgt_blk -> next != NULL){(tgt_blk -> next) -> prev = splt_header;}

        //update the old_footer for now_splitted block
        old_footer -> block_size = old_size - Min_size;

        //if tgt_block is the starting blk in the heap -> update the heap header 
        if (tgt_blk -> prev == NULL){
            heap_head -> fst_block  = splt_header;
        }
    }else{ //No need to split; just remove the tgt_blk from the free block list

    }

    //find a pointer to the block of memory for the return
    void * alloc_block_addr = ADD_BYTES(tgt_blk, ALIGNMENT);

    return alloc_block_addr;
}

void hl_release(void *heap, void *block) {
    return;
}

void *hl_resize(void *heap, void *block, unsigned int new_size) {
    return NULL;
}

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
typedef struct heap_header{
    unsigned long heap_size; //size of the heap
    struct block_header* fst_block; // point to the first block in the heap
} heap_header;

typedef struct block_header{
    unsigned long block_size; //size of block (addre is 8-byte alligned)
    struct block_header* prev; //points to previous empty block
    struct block_header* next; //points to next empty block 
} block_header;

typedef struct block_footer{
    unsigned long block_size; //size of block (addre is 8-byte alligned)
} block_footer;

// /* Given an unaligned pointer, how much offset do we need to make it aligned*/
// unsigned int get_align_offset(void* ptr){
// 	return (unsigned int)(ALIGN(ptr) - (unsigned long)ptr);
// }

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
    // Lock for this thread
    spin_lock(malloc_lock);

    //check the constraints 
    if(!heap){
        printf("Ahh, your input heap is wrong \n");

        // Unlock for this thread
        spin_unlock(malloc_lock);
        return;
        }
    if(heap_size < MIN_HEAP_SIZE || !heap){
        printf("Ahh, your input heap_size is wrong \n");

        // Unlock for this thread
        spin_unlock(malloc_lock);
        return;
    }

    // start setting up the heap
    heap_header* heap_head = (heap_header*) heap;
    // Alignment 
    unsigned long heap_head_int = ALIGN(heap_head);
    unsigned int offs = find_offset(heap_head, heap_head_int);
    heap_head = (heap_header*) (ADD_BYTES(heap_head, offs));
    heap_head -> heap_size = heap_size;

    //calc the first block starting point after the heap_header
    void* fst_blk_unAlign = ADD_BYTES(heap_head, sizeof(heap_header));
    unsigned int offset = (unsigned int)(ALIGN(fst_blk_unAlign) - (unsigned long)fst_blk_unAlign);

    // unsigned int offset = get_align_offset(ADD_BYTES(heap_head, sizeof(heap_header) + sizeof(block_header)));

    block_header* fst_blk_Align = (block_header *)(ADD_BYTES(heap_head, sizeof(heap_header) + offset));
    heap_head -> fst_block = (block_header*)fst_blk_Align;

    //initialize the first block
    // block_header
    // avail size = heap_size - sizeof(block_header) - sizeof(block_footer) - |fst_blk_Align|
    unsigned long size_available = (unsigned long)ADD_BYTES(heap_head, heap_size - sizeof(block_footer)) 
        - (unsigned long)ADD_BYTES(fst_blk_Align, sizeof(block_header));
    unsigned long size_avai_aligned = size_available == ALIGN(size_available)?size_available : ALIGN(size_available)-ALIGNMENT;
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

    // Unlock for this thread
    spin_unlock(malloc_lock);
    return;
}

/* ------------------- HELPER FUNCTIONS ----------------- */

/*  Calculate the size of a block needed */
unsigned long calc_needed_size (unsigned int payload_size){
    unsigned long base_size = sizeof(block_header)+sizeof(block_footer);
    //calculate the required size after aligning (2*ALIGNMENT for *prev and *next)
    unsigned long needed_size = (unsigned long)(ALIGN(payload_size)+base_size-2*ALIGNMENT);
    if(needed_size < base_size){
        needed_size = base_size;
    }
    return needed_size;
}

/*  Finded the first freed block that meets the required payload size*/
block_header* find_block (block_header* lst_start, unsigned int payload_size){
    block_header* current_block = lst_start;
    unsigned long size_needed = calc_needed_size(payload_size);

    while(current_block != NULL){
        unsigned long current_size = lst_start -> block_size;
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
    if(head != NULL){
        head -> prev = new_blk;
    } 

    return new_head;
}

/* Find the offset between two addresses */
unsigned int find_offset(addr_prt, addr_align_int){

    unsigned int offset = (unsigned int)(addr_align_int - (unsigned long)addr_prt);

    return offset;
}




/* -------------------- hl_alloc_helper ----------------- */
void *hl_alloc_helper(void *heap, unsigned int block_size) {

    //check constraints
    if(block_size == 0){ 
        return (void *)ALIGN(heap);
    }

    //locate the first block from heap
    heap_header* heap_head = (heap_header*) heap;

    // Alignment 
    unsigned long heap_head_int = ALIGN(heap_head);
    unsigned int offs = find_offset(heap_head, heap_head_int);
    heap_head = (heap_header*) (ADD_BYTES(heap_head, offs));

    block_header* list_head = heap_head -> fst_block;

    //find available block address meeting our "block_size"
    block_header* tgt_blk = find_block (list_head, block_size);
    if (tgt_blk == NULL){ return NULL; }

    //find this block's size
    unsigned long cur_blk_size = tgt_blk -> block_size;
    //find the Minimum block size needed for "block_size"
    unsigned long Min_size = calc_needed_size(block_size);

    //Modify the memory
    //If the free block's size is greater than size we needed -> split the block
    if (cur_blk_size - Min_size > calc_needed_size(0)){
        //store the old block information for calcution reason
        unsigned long old_size = tgt_blk->block_size;
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


/* -------------------- hl_alloc ----------------- */

void *hl_alloc(void *heap, unsigned int block_size) {
    // Lock for this thread
    spin_lock(malloc_lock);
    

    void* result = hl_alloc_helper(heap, block_size);

    // Unlock for this thread
    spin_unlock(malloc_lock);

    return result;
}

/* -------------------- hl_release_helper ----------------- */
void hl_release_helper(void *heap, void *block) {
    // check constraint 
    if (block == NULL) return;

    // releasing a previous allocaiton -> do nothing
    if((unsigned long)block == ALIGN(heap)) return;

    // retrieve the pointer
    heap_header* heap_head = (heap_header*)heap;

    // Alignment 
    unsigned long heap_head_int = ALIGN(heap_head);
    unsigned int offs = find_offset(heap_head, heap_head_int);
    heap_head = (heap_header*) (ADD_BYTES(heap_head, offs));


    block_header* block_free_hd = (block_header *)(ADD_BYTES(block, -ALIGNMENT));

    // indicating the block is freed (update the last bit)
    block_free_hd -> block_size = (block_free_hd -> block_size) & ~(1);

    block_footer* block_free_ft = (block_footer *)
        (ADD_BYTES(block_free_hd, (block_free_hd -> block_size)-sizeof(block_footer)));
    block_free_ft -> block_size = block_free_hd -> block_size;

    //link the freed block back to the list and check freeness
    // check head 
    block_footer* prev_footer = (block_footer*) ADD_BYTES(block_free_hd, -sizeof(block_footer));
    int is_prev_free = ((unsigned long)prev_footer <= (unsigned long)ADD_BYTES(heap_head, sizeof(heap_header)))
        ? 0 : IS_FREE(prev_footer-> block_size);

    // check the footer
    block_header* next_header = (block_header*)ADD_BYTES(block_free_hd, (block_free_hd->block_size)); 
    int is_next_free = ((unsigned long)next_header >= (unsigned long) ADD_BYTES(heap_head,heap_head -> heap_size) 
         ? 0 : IS_FREE(next_header -> block_size));

    //Coalescing blocks if needed (is_x_free = 1): 4 cases
    if (is_prev_free && !is_next_free){

        //retrive the prev_block_header
        unsigned long prev_blk_size = prev_footer -> block_size;
        block_header* prev_block_header = (block_header*) ADD_BYTES(block_free_hd, - prev_blk_size);
        block_footer* next_footer = (block_footer *)ADD_BYTES(block_free_hd, (block_free_hd -> block_size) - sizeof(block_footer));
        //merge prev_block and update the size in header and footer
        unsigned long new_size = prev_blk_size + (block_free_hd -> block_size);
        prev_block_header -> block_size = new_size;
        next_footer -> block_size = new_size;

    }else if(!is_prev_free && is_next_free){

        //merge next_block and update the header and footer size
        unsigned long new_size = block_free_hd -> block_size + next_header -> block_size;
        block_free_hd -> block_size = new_size;
        block_footer* next_footer = 
            (block_footer*) ADD_BYTES(next_header, next_header -> block_size - sizeof(block_footer));
        next_footer -> block_size = new_size;

        // update the dependencies
        block_free_hd -> prev = next_header -> prev;
        block_free_hd -> next = next_header -> next;

        if (next_header -> next != NULL)
            {
                block_header* temp = (next_header -> next);
                temp -> prev = block_free_hd;
            }
        if (next_header -> prev != NULL){
                block_header* temp = (next_header -> prev);
                temp -> next = block_free_hd;
        }else{
                heap_head -> fst_block  = block_free_hd;
        }

    }else if (is_prev_free && is_next_free){
        //retrive the prev_block_header and next_footer
        unsigned long prev_blk_size = prev_footer -> block_size;
        block_header* prev_block_header = (block_header*)ADD_BYTES(block_free_hd, - prev_blk_size);
        block_footer* next_footer = (block_footer*)ADD_BYTES(next_header, next_header -> block_size - sizeof(block_footer));

        //merge next_block and update the header and footer size
        unsigned long new_size = prev_block_header -> block_size + next_footer -> block_size + block_free_hd -> block_size;
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


/* -------------------- hl_release ----------------- */
void hl_release(void *heap, void *block) {
    // Lock for this thread
    spin_lock(malloc_lock);
    

    hl_release_helper(heap, block);

    // Unlock for this thread
    spin_unlock(malloc_lock);
}


/* -------------------- hl_resize ----------------- */
void *hl_resize(void *heap, void *block, unsigned int new_size) {
    // Lock for this thread
    spin_lock(malloc_lock);


    // check constraints
    if(heap == NULL) {
        // Unlock for this thread
        spin_unlock(malloc_lock);
        return FAILURE;
    }
    if(block == NULL) {
        void* result = hl_alloc_helper(heap, new_size);

        // Unlock for this thread
        spin_unlock(malloc_lock);
        return result;
    }
    // resizing the size 0 allocation that was done previously by resize 
    if(ALIGN(heap)==(unsigned long)block) {
        void* result = hl_alloc_helper(heap, new_size);

        // Unlock for this thread
        spin_unlock(malloc_lock);
        return result;
    }

    if(new_size == 0 ){
        hl_release_helper(heap,block);
        // Unlock for this thread
        spin_unlock(malloc_lock);
        return (void*)ALIGN(heap);
    }

    // acquire pointers
    block_header* block_hd = (block_header*)(ADD_BYTES(block,-sizeof(block_header)+2*ALIGNMENT));

    // Alignment 
    unsigned long heap_head_int = ALIGN(block_hd);
    unsigned int offs = find_offset(block_hd, heap_head_int);
    block_hd = (heap_header*) (ADD_BYTES(block_hd, offs));

    void* result_pt = block; 

    // get the minimum block size for the current block
    unsigned long new_block_size = calc_needed_size(new_size);
    // retrieve the old block size
    unsigned long block_size = (block_hd-> block_size)& ~(1);

    // starting resize procedure
    // case 1
    if(new_block_size == block_size) {
        // Unlock for this thread
        spin_unlock(malloc_lock);
        return block;
    }
    else if(new_block_size < block_size) 
    { // case 2: split the block as hl_alloc did
      //if remainning space is too small to hold block_header and footer -> do nothing
      if(block_size-new_block_size < calc_needed_size(0)){
        // Unlock for this thread
        spin_unlock(malloc_lock);
        return block;
      }else{ //if the space is large enough -> split into two blocks
            // store the old size and old pointer to footer
            unsigned long old_size = block_size;
            block_footer* old_footer = (block_footer*)ADD_BYTES(block_hd, old_size-sizeof(block_footer));

            // update the header's size to new size
            block_hd -> block_size = new_block_size;
            block_hd -> block_size = block_hd -> block_size | 1;

            // create a new footer for resized_block
            block_footer* footer1 = (block_footer*)ADD_BYTES (block_hd, new_block_size-sizeof(block_footer));
            footer1 -> block_size = block_hd -> block_size;

            // create a new header for the remainning free space
            block_header* new_header = (block_header*)ADD_BYTES(block_hd,new_block_size);
            new_header -> block_size = old_size - new_block_size;
            old_footer -> block_size = new_header -> block_size;

            // coalesce the freed spaces if there is any
            block_header* next_blk_hd = (block_header*)ADD_BYTES(new_header, new_header->block_size);
            int is_next_free = ((unsigned long)next_blk_hd >= 
                (unsigned long) ADD_BYTES( heap_hd, heap_hd -> heap_size - ALIGNMENT) ? 0 : IS_FREE(next_blk_hd -> block_size));
            if(is_next_free){ //if next block is free
                block_footer *new_footer = (block_footer*)ADD_BYTES(next_blk_hd,next_blk_hd->block_size - sizeof(block_footer));
                unsigned long new_size = (unsigned long)(next_blk_hd -> block_size + new_header -> block_size);
    			new_footer -> block_size = new_size;
    			new_header -> block_size = new_size;
    			new_header -> next = next_blk_hd -> next;
    			new_header -> prev = next_blk_hd -> prev;
                if(next_blk_hd -> next != NULL){
                    block_header* temp = next_blk_hd -> next;
	    		    temp -> prev = new_header;
		    	}
		    	if(next_blk_hd -> prev != NULL){
                    block_header* temp = next_blk_hd -> prev;
		    		temp -> next = new_header;
		    	}
		    	else{
		    		heap_hd -> fst_block = new_header;
		    	}
            }else{// add the free block to the free block list
                heap_hd -> fst_block = insert (new_header, heap_hd -> fst_block);
            }
            // store the result
            result_pt = block;
        }
    }else{ // new size is bigger than old size
        // Check if the next block is free (move pointer or not)
        block_header* next_block2 = (block_header*)ADD_BYTES(block_hd, block_size);

        // the free space next block contain:
        unsigned long next_free_size = 0; 
        if((unsigned long)next_block2 < (unsigned long)ADD_BYTES(heap_hd, heap_hd->heap_size)){
            if(!IS_FREE(next_block2 ->block_size)){
                next_free_size = 0;
            }else{ // 
                // next_free_size = next_block2->block_size;
                next_free_size = (unsigned long)
                    (ADD_BYTES(next_block2, next_block2 -> block_size) - ADD_BYTES(block_hd, block_size));
            }
        }

        unsigned long avai_size = next_free_size + block_size;
        // check: if the avai_size is big enough to split
        if(avai_size >= new_block_size + calc_needed_size(0)){
            // retrive next block's footer
            block_footer* next_block_footer1 = (block_footer*)
                ADD_BYTES(next_block2,next_block2->block_size -sizeof(block_footer));
            // store old pointers 
            block_header* old_prev = next_block2 ->prev;
            block_header* old_next = next_block2 ->next;
            // create a header for the remainning free space
            block_header* new_header1 = (block_header*)ADD_BYTES(block_hd, new_block_size);
            new_header1 -> block_size = avai_size - new_block_size;
            // update the old footer
            next_block_footer1 -> block_size = new_header1 -> block_size;

            // remove the original free block & add splited block to the list
            new_header1 -> next = old_next;
            new_header1 -> prev = old_prev;
            if(old_prev!=NULL){
                old_prev -> next = new_header1;
            }else{
                heap_hd->fst_block = new_header1;
            }

            if(old_next!=NULL){
                old_next -> prev = new_header1;
            }

            // change block size into allocated size
            block_hd ->block_size = new_block_size;
            block_hd -> block_size = block_hd ->block_size | 1; 

            // add new footer to the newly allocated block
            block_footer* alloc_footer = (block_footer*)ADD_BYTES(block_hd,new_block_size-sizeof(block_footer));
            alloc_footer -> block_size = block_hd -> block_size;

            // record result pointer
            result_pt = block;

        }else if(avai_size >= new_block_size){
            // update the old block size into allocated size
            block_hd ->block_size = avai_size;
            block_hd ->block_size = block_hd ->block_size | 1;

            // remove the original free block
            if(next_block2 ->prev!=NULL){
                block_header* temp = next_block2->prev;
                temp -> next = next_block2 -> next;
            }else{
                heap_hd-> fst_block = next_block2->next;
            }
            if(next_block2->next != NULL){
                block_header* temp = next_block2->next;
                temp -> prev = next_block2 ->prev;
            }

            // update footer
            block_footer* footer3 = (block_footer*)ADD_BYTES(block_hd, avai_size-sizeof(block_footer));
            footer3 -> block_size = block_hd->block_size;

            // record result pointer
            result_pt = block;

        }else{//move the pointer and do memory copy (memcpy)
            block_footer* prev_footer = (block_footer*) ADD_BYTES(block_hd, -sizeof(block_footer));
            unsigned long pre_block_size;
            if((unsigned long) prev_footer < (unsigned long)ADD_BYTES(heap_hd, sizeof(heap_header))){
                pre_block_size = 0;
            }else if (!IS_FREE(prev_footer -> block_size)){
                pre_block_size = 0;
            }else{
                pre_block_size = prev_footer ->block_size;
            }

            // return the new pointer for allocated block
            void* new_pt = block;
            if(block_size + next_free_size + pre_block_size >= new_block_size){
                hl_release_helper(heap_hd, block);
                new_pt = hl_alloc_helper(heap_hd, new_size);
            }else{
                new_pt= hl_alloc_helper(heap_hd,new_size);
                if(new_pt==NULL){

                    // Unlock for this thread
                    spin_unlock(malloc_lock);
                    return FAILURE;
                }
            }

            unsigned long old_block_size = block_size +2*ALIGNMENT - sizeof(block_header)-sizeof(block_footer);
            memcpy(new_pt, block, old_block_size);
            result_pt = new_pt;
        }
    }

    #ifdef PRINT_DEBUG
        printf("heap_head -> fst_block starts at addr %p\n", heap_hd -> fst_block);
        printf("result_pt is at  %p\n", result_pt);
    #endif

    // Unlock for this thread
    spin_unlock(malloc_lock);
    return result_pt;
}

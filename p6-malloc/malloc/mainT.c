#include <stdio.h>
#include "try.c"
#include "heaplib.c"

int main(){
    // addrs_cal1(4);
    algning(4); 
    // add_byte(4);

    print_daaaaa(21);  
    print_ooooo(8);
    printf("\n This is the start !!!!!  \n");

    char heap[1300];
    hl_init(heap, 1300);

    void* alloc = hl_alloc(heap, 105);
    // printf("sizeof(size_t) is  %d\n", sizeof(size_t));
    
    return 0;
} 

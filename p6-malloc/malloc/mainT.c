#include <stdio.h>
#include "try.c"
#include "heaplib.c"

int main(){
    // addrs_cal1(4);
    algning(4); 
    // add_byte(4);

    print_daaaaa(20);  
    print_ooooo(13);
    printf("\n This is the start !!!!!  \n");

    char heap[1240];
    printf("\n hl_init +++ \n");
    hl_init(heap, 1240);

    printf("\n hl_alloc +++ \n");
    void* alloc = hl_alloc(heap, 105);

    printf("\n hl_release +++ \n");
    hl_release(heap, alloc);
    printf("sizeof(size_t) is  %d\n", sizeof(size_t));
    printf("sizeof(unsigned int) is  %d\n", sizeof(unsigned long));

    // unsigned int mask = ~(1);

    // printf("this is bit test: %u \n", mask);
    
    return 0;
} 

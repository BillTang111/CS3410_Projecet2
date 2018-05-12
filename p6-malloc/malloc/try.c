#define ALIGNMENT 8
#define PRINT_DEBUG 
// #define ALIGN(ptr) ((char*)ptr + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
#define ALIGN(ptr) (((unsigned long)ptr + ALIGNMENT - 1) & ~(ALIGNMENT-1))

void print_daaaaa(int a) {
#ifdef PRINT_DEBUG
printf("YOU input is: %d \n", a);
#endif
}

void addrs_cal1(int a) {
    // printf("hello world");
    // int a = 8;
    // printf("the size in byte is: %d \n", (a+1));
    // printf("the size in un_long is: %lu \n",(((unsigned long) (a))+1));
    printf("This is addrs cal 1 : \n");

    unsigned long array[a];
    array[0] = 222;
    array[1] = 111;
    array[2] = 333;

    unsigned long *ptr = &array[0];
    printf("this is point before: %p \n", &array);
    ptr = ptr + 1; //add 8 in hex
    printf("this is point after: %p \n", ptr);

    //add (4x4)x1
    // printf("this is point before: %p \n", &array);
    // int *ptr = &array + 1;
    // printf("this is point after: %p \n", ptr);
}

void algning(int a) {
    printf("This is algning : \n");
    unsigned long array[a];
    array[0] = 222;
    array[1] = 111;
    array[2] = 333;

    unsigned long *ptr = &array[0];
    // printf("this is point before: %p \n", ptr);
    // unsigned long * pttr = (unsigned long)ptr + 7; //add 7 in hex (inapprop); ptr + 7: add 7x8
    // printf("this is point after: %p \n", pttr);

    printf("this is point before22: %p \n", ptr);
    char * ptttr = (char*)ptr + 7; //add 7 in hex; ptr + 7: add 7x8
    printf("this is point after22: %p \n", ptttr);

    unsigned long al_ptr = ALIGN(ptttr);
    printf("this is after align: %lu \n", al_ptr);

}

void add_byte(int a) {
    printf("This is add_byte : \n");
    unsigned long array[a];
    array[0] = 222;
    array[1] = 111;
    array[2] = 333;

    unsigned long *ptr = &array[0];
    printf("this is point before: %p \n", ptr);
    void *sixteen_later = ((char *)ptr) + 7; //add 7 in hex; ptr + 7: add 7x8
    printf("this is point after: %p \n", sixteen_later);
}


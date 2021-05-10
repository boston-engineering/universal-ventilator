//
// Created by Andrew Hoener on 5/7/21.
//

#ifndef UVENT_MEMTEST_H
#define UVENT_MEMTEST_H

#include <malloc.h>
#include <cstdlib>
#include <cstdio>

extern char _end;
extern "C" char *sbrk(int i);
char *ramstart = (char *) 0x20070000;
char *ramend = (char *) 0x20088000;

void printMem();

void printMem() {
    char *heapend = sbrk(0);
    register char *stack_ptr asm ("sp");
    struct mallinfo mi = mallinfo();
    printf("------------------------\n");
    printf("Dynamic ram used: %d bytes\n", mi.uordblks);
    printf("Program static ram used %d bytes\n", &_end - ramstart);
    printf("Stack ram used %d bytes\n", ramend - stack_ptr);
    printf("My guess at free mem: %d bytes\n", stack_ptr - heapend + mi.fordblks);
    printf("------------------------\n\n");

}

#endif //UVENT_MEMTEST_H

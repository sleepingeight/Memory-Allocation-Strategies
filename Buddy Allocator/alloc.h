#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#define PAGESIZE 4096 //size of memory to allocate from OS


enum state {yes = 1, no = 0};

typedef struct node node;
struct node
{
    enum state is_allocated;
    enum state is_child_allocated;
    uintptr_t address;
    size_t size;
};


// function declarations
int init();
void *alloc8(int);
void free8(void *);

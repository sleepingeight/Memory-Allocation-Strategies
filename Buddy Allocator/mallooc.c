#include "alloc.h"

void *start_ptr = NULL;
node *root = NULL;

#define LENGTH 2*PAGESIZE
node *tree;

int init(){
    void *mem = mmap(NULL, PAGESIZE+sizeof(node)*LENGTH, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, 0, 0);
    if(mem == MAP_FAILED){
        perror("eight: ");
        return -1;
    }


    tree = (node *)((uintptr_t)mem+PAGESIZE);
    start_ptr = mem;


    tree[1].is_allocated = no;
    tree[1].is_child_allocated = no;
    tree[1].address = (uintptr_t)mem;
    tree[1].size = PAGESIZE;
  
    for(int i = 2; i < LENGTH; i++){
        if(tree[i/2].size == 2) break;
        tree[i].is_allocated = no;
        tree[1].is_child_allocated = no;
        tree[i].size = (tree[i/2].size)/2;
        tree[i].address = tree[i/2].address;
        if(i%2) tree[i].address += tree[i].size;
    }

    return 1;
}

int nearest2power(int num){
    int p = 2;
    while(p < num){
        p = p*2; 
    }
    return p;
}


void *alloc8(int sz){
    sz = nearest2power(sz);

    int id = 0, i = 1;
    for(int level = 1; i < LENGTH; level++, i = (1<<level)){
        if(tree[i].size == sz){
            id = i;
            break;
        }
    }

    if(id == 0){
        printf("1not okay\n");
        return NULL;
    } 
    
    for(i = id; i < 2*id; i++){
        int temp = i;
        while(temp && tree[temp].is_allocated == no) temp /= 2;
        if(tree[i].is_child_allocated == no && temp == 0){
            id = i;
            i = i/2;
            while(i){
                tree[i].is_child_allocated = yes;
                i = i/2;
            }
            tree[id].is_allocated = yes;
            break;
        }
    }

    //printf("%d\n", i);

    if(i != 0){
        printf("2not okay\n");
        return NULL;
    } 

    i = id/2;
    while(i){
        if(tree[2*i].is_allocated == yes && tree[2*i+1].is_allocated == yes){
            tree[i].is_allocated = yes;
        }
        i = i/2;
    }

    return (void *)tree[id].address;
}



void free8(void *ptr){
    int id = ((uintptr_t)ptr-(uintptr_t)start_ptr)/2+2048;
    while(tree[id].is_allocated == no) id = id/2;
    printf("size - %d\n", tree[id].size);
    bzero(ptr, tree[id].size);
    tree[id].is_allocated = no;
    id = id/2;
    while(id){
        if(tree[2*id].is_allocated == no || tree[2*id+1].is_allocated == no){
            tree[id].is_allocated = no;
        }
        id = id/2;
    }

    for(int i = PAGESIZE/2-1; i >= 1; i--){
        if(tree[2*i].is_allocated == no && tree[2*i+1].is_allocated == no && tree[2*i].is_child_allocated == no && tree[2*i+1].is_child_allocated == no){
            tree[i].is_child_allocated = no;
        }
        else tree[i].is_child_allocated = yes;
    }

    return;
}


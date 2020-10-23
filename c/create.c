/* create.c : create a process
 */

#include <xeroskernel.h>

/* Your code goes here. */

/************************************
 * function name: create
 * return: returns 1 on success and 0 on failure
 * comment: creates a new process and push it on to ready queue
*************************************/
extern int create( void (*func)(), int stack ){
    if (!(unsigned long)(*func)) {
        kprintf("Error: invalid function address");
        return 0;
    }

    if (stack <= 0) {
        kprintf("Warning: invalid stack size");
        return 0;
    }

    pcb_t *new_pcb = dequeuepcb(STOPPED);
    void* new_memory = kmalloc((size_t)stack);
    //null if stack and heap collide
    if (new_memory == NULL) {
        return -1;
    }
    new_pcb->space_start = new_memory;
    void* tempEsp = new_memory - 16 - sizeof(context_frame_t);
    new_pcb->esp = (unsigned long) tempEsp;
    // Point to end of the allocated memory chunk 
    // Subtract a safety margin and size of context frame 
    // Stack pointer starts here 
    //Initialize the context frame and push to the stack
    *(long*)(tempEsp)    = 0;                               //edi
    *(long*)(tempEsp+4)  = 0;                               //esi
    *(long*)(tempEsp+8)  = (unsigned long) tempEsp;         //ebp
    *(long*)(tempEsp+12) = (unsigned long) tempEsp;         //esp
    *(long*)(tempEsp+16) = 0;                               //ebx
    *(long*)(tempEsp+20) = 0;                               //edx
    *(long*)(tempEsp+24) = 0;                               //ecx
    *(long*)(tempEsp+28) = 0;                               //eax
    *(long*)(tempEsp+32) = (unsigned long) func;            //eip
    *(long*)(tempEsp+36) = getCS();                         //cs
    *(long*)(tempEsp+40) = 0;                               //flags
    enqueuepcb(READY, new_pcb);
    return 1;
}
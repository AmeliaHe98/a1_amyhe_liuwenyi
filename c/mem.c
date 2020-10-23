// /* mem.c : memory manager
//  */

#include <xeroskernel.h>
#include <i386.h>
#include <xeroskernel.h>

static struct memHeader* start; /*start of free memory */
extern  long	freemem; 	/* start of free memory (set in i386.c) */
extern char	*maxaddr;	/* max memory address (set in i386.c)	*/

/************************************
 * function name: kmeminit
 * require: 
 * return:
 * comment: initialization the memory manager
*************************************/
extern void kmeminit(void){
    // set the memory from freemem to HOLESTART
    start = (struct memHeader*) freemem;
    start->size = (unsigned long) HOLESTART - freemem;
    start->prev = NULL;
    start->next = (struct memHeader*) HOLEEND;
    start->sanityCheck = "DEADBEEF";

    // set the memory after HOLEEND
    start->next->size = (unsigned long)maxaddr - HOLEEND;
    start->next->next = NULL;
    start->next->prev = start;
    start->next->sanityCheck = "DEADBEEF";
}


/************************************
 * function name: kmalloc
 * require: size > 0 && size < maxaddr
 * return: a pointer to the start of it, if not enough memory is available the function is to return 0
 * comment: allocates a corresponding amount of memory from the available memory 
*************************************/
extern void* kmalloc( int size){
    //computer amount of memory for this request and include header space
    if (size <= 0) {
        return NULL;
    }
    // newMem is the memory chunk that need to be allocated
    struct memHeader* newMem = start;
    // calculate the amount of memory chunk that need to be allocated
    int amount = size/16 + ((size%16)?1:0);
    amount = amount*16 + sizeof(struct memHeader);
    //
    while(newMem && newMem->next){
        if(newMem->size >= amount){
            break;
        }
        else{
            newMem=newMem->next;
        }
    }
    //no memory found
    if(!newMem->next && newMem->size < (amount + sizeof(struct memHeader))){
        return 0; 
    }
    // calculate the size of the memory chunk after being allocated
    int leftAmount = newMem->size - amount;
    // when the size of the memory to be allocated is equal to the current memory chunk
    if(leftAmount==0){
        if (newMem->next) newMem->next->prev = newMem->prev;
        if (newMem->prev) newMem->prev->next = newMem->next;
        if (newMem == start) start = start->next;
    } else{
        if (newMem->prev) {
            newMem->prev->next = newMem + amount / 16;
            newMem->prev->next->size = newMem->size - amount;
            newMem->prev->next->prev = newMem->prev;
            newMem->prev->next->next = newMem->next;
        }
        // populating the leftover memory chunk
        struct memHeader *orig_next = start->next;
        struct memHeader *orig_prev = start->prev;
        unsigned long orig_size = start->size;

        if (newMem == start) {
            start += amount/ 16;
            start->next = orig_next;
            start->prev = orig_prev;
            start->sanityCheck = "DEADBEEF";
            start->size = orig_size - amount;
        }

    }
    return newMem->dataStart;
}

/************************************
 * function name: kfree
 * return: void
 * comment:t takes a pointer to a previously allocated chunk of memory and returns it to the free memory pool.
*************************************/
extern void kfree( void* ptr ){
    // delete is the memory block that we want to free
    struct memHeader* delete =  ptr - sizeof(struct memHeader);
    // temp is a temporary pointer to find the memory chunk just before the memory block to be freed
    struct memHeader* temp;
    if (delete->sanityCheck != (char*) "DEADBEEF") return;
    if (start == delete) {
        start = start->prev;
    }
    // if the memory to be freed is before where the free memory starts
    else if (start > delete) {
        delete->next = start;
        start->prev = delete;
        start = delete;
        if (delete->next) 
            delete->size = (unsigned long)delete->next - (unsigned long)ptr;
        else delete->size = (unsigned long)maxaddr - (unsigned long)ptr;
        if ((unsigned long) ptr <= HOLESTART && (unsigned long) delete->next >= HOLEEND)
            delete->size -=  HOLESIZE;
    } else {
        temp = start;
        while (temp->next != NULL && temp->next < delete) {
            temp = temp->next;
        }
        // set the size of the memory block to be freed
        if (temp->next) {
            delete->size = (unsigned long)temp->next - (unsigned long)ptr;
            if ((unsigned long) ptr <= HOLESTART && (unsigned long) temp->next >= HOLEEND)
                delete->size -=  HOLESIZE;
        } else delete->size = (unsigned long) maxaddr - (unsigned long) ptr;
        struct memHeader* tempnext = temp->next;
        temp->next = delete;
        delete->prev = temp;
        if (tempnext != NULL) {
            tempnext->prev = delete;
            delete->next = tempnext;
        }
    }
    // coalesce with the memory after delete if possible
    if (((unsigned long) &delete->dataStart + (unsigned long)delete->size) == (unsigned long) delete->next) {
                    kprintf("deletenext: %d\n", delete->next);

        kprintf("deletenextsize: %d\n", delete->next->size);
        delete->size += delete->next->size + sizeof(struct memHeader); 
        delete->next = delete->next->next;

        if (delete->next) delete->next->prev = delete;
    }
    // coalesce with the memory before delete if possible
    if (delete->prev && ((unsigned long) &delete->prev->dataStart[0] + (unsigned long)delete->prev->size) == (unsigned long) delete) {
        delete->prev->size += delete->size;
        delete->prev->next = delete->next;
        if (delete->next) delete->next->prev = delete->prev;
    
    }
}

/************************************
 * function name: printFreeMem
 * comment: for debug, print of the list of free memory
*************************************/
extern void printFreeMem(void){
    struct memHeader* traverse; //pointer to the free memory list
    traverse = start;
    // //traverse list    
    while(traverse){
         traverse = traverse->next; 
    }
}

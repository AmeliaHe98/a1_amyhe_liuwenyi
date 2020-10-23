/* disp.c : dispatcher
 */

#include <xeroskernel.h>

pcb_t pcb_table[PCB_TABLE_SIZE];
int syscallReturn;
pcb_queue_t* stopped_queue;
pcb_queue_t* ready_queue;
extern long *args;
/* Your code goes here */
/************************************
 * function name: dispinit
 * return: void
 * comment: Initialized PCB table
*************************************/
extern void dispinit(void){
    pcbtableinit();
    
}

void dummyproc(void)
{
 // do a token thing?
 kprintf("In dummyproc\n");
 sysstop();
}

/************************************
 * function name: pcb_table_init
 * return: void
 * comment: Initialized PCB table and queue
*************************************/
void pcbtableinit(void) {
    ready_queue = NULL;

    // initialize the pcb table
    for(int i = 0; i < PCB_TABLE_SIZE; i++) {
        pcb_table[i].pid = i + 1;
        pcb_table[i].state = STOPPED;
    }

    stopped_queue = kmalloc(sizeof(pcb_queue_t));
    stopped_queue -> pid = pcb_table[0].pid;
    pcb_table[0].queue = stopped_queue;
    pcb_queue_t* prev_pointer = stopped_queue;
    // initialize the stopped queue
    for(int i = 1; i < PCB_TABLE_SIZE; i++) {
        pcb_queue_t* temp = kmalloc(sizeof(pcb_queue_t));
        temp->pid = pcb_table[i].pid;
        pcb_table[i].queue = temp;
        prev_pointer->next = temp;
        prev_pointer = temp;
    }
}
    
/************************************
 * function name: dispatch
 * return: 
 * comment: infinite loop that processes system calls, schedules the next process, and then calls the context switcher to switch into the next scheduled process
*************************************/

extern void dispatch(void){
    pcb_t* process = dequeuepcb(READY);
    kprintf("got process");
     for( ;; ) {
         kprintf("entering ctsw\n");
         int request = contextswitch( process );
      kprintf("got request");
    switch(request) {
        kprintf("in request");
        case(0): //CREATE
        {
            funcptr func = (funcptr) *args;
            int stack = (int)  *(args+1);
            syscallReturn = create(func, stack);
            break;
        }    
        case(1):  //YIELD
        {
            enqueuepcb(READY,process); //place into the ready queue
            process = dequeuepcb(READY); //fetch the next readt process from the ready queue   
            break; 
        }
        case( 2 ): { //STOPPED
            cleanup(process); //call the helper function to free the stack space and reset PCB         
            process = dequeuepcb(READY); //fetch the next ready process from the ready queue
            break;
        }
    }

}} 

// /************************************
//  * function name: enqueuepcb
//  * return: void
//  * comment: add new process to the queue
// *************************************/

extern void enqueuepcb(process_state_enum_t state, pcb_t *new_pcb) {
    new_pcb->queue -> next = NULL;
    if (state == STOPPED) {
        //change the state in new_pcb
        new_pcb->state = STOPPED;
        if(stopped_queue==NULL) {
            //If the queue is null, set the head to the new pcb.
            stopped_queue = new_pcb->queue;
        }else {
            //Otherwise, get the node pcb_queue_t from the new pcb
            pcb_queue_t* cur = new_pcb->queue;
            //Use a while loop to search for the last element in the queue
            pcb_queue_t* last = stopped_queue;
            while(last->next!= NULL) {
                last = last->next;
            }
            //append the cur node to the last element in queue
            last->next = cur;
        }
    } else if (state == READY) {
        //change the state in new_pcb
        new_pcb->state = READY;
        if(ready_queue==NULL) {
            //If the queue is null, set the head to the new pcb.
            ready_queue = new_pcb->queue;
        }else {
            //Otherwise, get the node pcb_queue_t from the new pcb
            pcb_queue_t* cur = new_pcb->queue;
            //Use a while loop to search for the last element in the queue
            pcb_queue_t* last = ready_queue;
            while(last->next!= NULL) {
                last = last->next;
            }
            //append the cur node to the last element in queue
            last->next = cur;
        }
    }else {
        kprintf("\n Error: enqueue failure!");
    }
}

// /************************************
//  * function name: dequeuepcb
//  * return: pcb_t
//  * comment: pop a process from queue
// *************************************/
extern pcb_t *dequeuepcb(process_state_enum_t state) {
    if (state == STOPPED) {
        if(stopped_queue != NULL) {
            //If the stopped_queue is not null, preserve its head in a temp pointer
            pcb_queue_t* temp = stopped_queue;
            //and set it to the next element.
            stopped_queue = temp->next;
            //dereference the head to the next element
            temp->next = NULL;
            //return corresponding pcb from the pcb_table
            
            //used for TEST 3 only
            kprintf("dequeue pcb with id %d from stopped_queue\n", temp->pid);
            // for(int i=0; i<200000; i++);
            return &pcb_table[temp->pid-1];
        } else {
            kprintf("stopped is null");
            return NULL;
        }
    } else if (state == READY) {
        if(ready_queue != NULL) {
            //If the ready_queue is not null, preserve its head in a temp pointer
            pcb_queue_t* temp = ready_queue;
            //and set it to the next element.
            ready_queue = temp->next;
            //dereference the head to the next element
            temp->next = NULL;
            //return corresponding pcb from the pcb_table

            //used for TEST 3 only
            kprintf("dequeue pcb with id %d from ready_queue\n", temp->pid);
            // for(int i=0; i<200000; i++);
            return &pcb_table[temp->pid-1];
        } else {
            kprintf("ready is null");
            return NULL;
        }
    } else {
        kprintf("\n Error: dequeue failure!");
        return NULL;
    }
    return NULL;
}

/************************************
 * function cleanup
 * return: void
 * comment: free memory of a process and add to enqueu pcb
*************************************/
extern  void cleanup(pcb_t *new_pcb) {
    new_pcb->state = STOPPED;
    kfree(new_pcb->space_start);
    enqueuepcb(STOPPED,new_pcb);
}
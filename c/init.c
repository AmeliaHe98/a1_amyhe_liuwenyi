/* initialize.c - initproc */

#include <i386.h>
#include <xeroskernel.h>
#include <xeroslib.h>

extern	int	entry( void );  /* start of kernel image, use &start    */
extern	int	end( void );    /* end of kernel image, use &end        */
extern  long	freemem; 	/* start of free memory (set in i386.c) */
extern char	*maxaddr;	/* max memory address (set in i386.c)	*/
/************************************************************************/
/***				NOTE:				      ***/
/***								      ***/
/***   This is where the system begins after the C environment has    ***/
/***   been established.  Interrupts are initially DISABLED.  The     ***/
/***   interrupt table has been initialized with a default handler    ***/
/***								      ***/
/***								      ***/
/************************************************************************/

/*------------------------------------------------------------------------
 *  The init process, this is where it all begins...
 *------------------------------------------------------------------------
 */
void initproc( void )				/* The beginning */
{
  kmeminit();
  kprintf("kmem initialized\n");
  // printFreeMem();
  // void *mallocSpace = kmalloc(16);
  // printFreeMem();
  // kfree(mallocSpace);
  // printFreeMem();
  pcbtableinit();
  // dispinit();
  kprintf("pcb tables initialized\n");
  create(root, 1024);
  // kprintf("context switcher initalized\n");
  // print_pcb_queue(READY);
  contextinit();
  kprintf("pcb tables initialized\n");
  // sysyield();
  dispatch();
  kprintf("dispatcher initialized\n");
  // test_get_next_proc();
  // kprintf("dispatcher test passed\n");
  // test_sysgetpid();
  // kprintf("sys test passed\n");
  // for(;;) ; /* loop forever */
}

/**
 * Tests get_next_proc() uses FIFO queue
 */
void test_get_next_proc(void) {
    for (int i = 0; i < 3; i++) {
        create(&dummy, INIT_STACK);
    }
    print_pcb_queue(READY);

    for (int i = 1; i <= 3; i++) {
        pcb_t* process = dequeuepcb(READY);    
        enqueuepcb(STOPPED, process);
    }

    print_pcb_queue(READY);
    print_pcb_queue(STOPPED);
}

/**
 * Debugging function to help print queue contents
 * @param queue: the queue to dump
 */
void print_pcb_queue(process_state_enum_t state) {

    if (state == READY){
      //change the state in new_pcb
      pcb_queue_t* temp = ready_queue;
      while(temp != NULL) {
        kprintf("{PID: %d}", temp->pid);
        temp = temp->next;
      }
    }
    kprintf("\n");
    if (state == STOPPED){
      //change the state in new_pcb
      pcb_queue_t* temp = stopped_queue;
      while(temp != NULL) {
        kprintf("{PID: %d}", temp->pid);
        temp = temp->next;
      }
    }

}

/**
 * Dummy func, for use by create(). Should not be entered.
 */
void dummy(void) {}

/**
 * Runs all syscall tests
 */
void syscall_run_all_tests(void) {
  // memory manager test
  test_mem_allocate_start();
  test_mem_allocate_middle();
    // test syscreate
    // test_sys_create();
}


void test_mem_allocate_start(void) {
  printFreeMem();
  void *mallocSpace = kmalloc(16);
  printFreeMem();
  kfree(mallocSpace);
  printFreeMem();
}

void test_mem_allocate_middle(void) {
  printFreeMem();
  void *mallocSpace = kmalloc(868351);
  printFreeMem();
  kfree(mallocSpace);
  printFreeMem();
}

// function2 for syscreate
void sys_create_fn1(void) {
    kprintf("fn1 created\n");
    // sysstop();
}
// // function2 for syscreate
// void sys_create_fn1(void) {
//     kprintf("fn1 created\n");
//     sysstop();
// }

// // function2 for sycreate
// void sys_create_fn2(void) {
//     kprintf("fn2 created\n");
//     // sysstop();
// }

// // test syscreate
// void test_sys_create(void) {
//   syscall(&sys_create_fn1, KERNEL_STACK);
//   syscall(&sys_create_fn1, KERNEL_STACK);
//   print_pcb_queue(READY);
// }

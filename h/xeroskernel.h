/* xeroskernel.h - disable, enable, halt, restore, isodd, min, max */

#ifndef XEROSKERNEL_H
#define XEROSKERNEL_H

/* Symbolic constants used throughout gutted Xinu */

typedef	char    Bool;        /* Boolean type                  */
typedef unsigned int size_t; /* Something that can hold the value of
                              * theoretical maximum number of bytes 
                              * addressable in this architecture.
                              */
#define	FALSE   0       /* Boolean constants             */
#define	TRUE    1
#define	EMPTY   (-1)    /* an illegal gpq                */
#define	NULL    0       /* Null pointer for linked lists */
#define	NULLCH '\0'     /* The null character            */


/* Universal return constants */

#define	OK            1         /* system call ok               */
#define	SYSERR       -1         /* system call failed           */
#define	EOF          -2         /* End-of-file (usu. from read)	*/
#define	TIMEOUT      -3         /* time out  (usu. recvtim)     */
#define	INTRMSG      -4         /* keyboard "intr" key pressed	*/
                                /*  (usu. defined as ^B)        */
#define	BLOCKERR     -5         /* non-blocking op would block  */
#define LASTCONST    -5
#define PCB_TABLE_SIZE 32
#define INIT_STACK    4096 * 4

//list of interrupt numbers
#define SYS_CALL 66
/* Functions defined by startup code */


void           bzero(void *base, int cnt);
void           bcopy(const void *src, void *dest, unsigned int n);
void           disable(void);
unsigned short getCS(void);
unsigned char  inb(unsigned int);
void           init8259(void);
int            kprintf(char * fmt, ...);
void           lidt(void);
void           outb(unsigned int, unsigned char);
void           set_evec(unsigned int xnum, unsigned long handler);

extern long *args;
typedef enum {
  CREATE = 0,
  YIELD = 1,
  STOP = 2
}request_enum_t;

typedef enum {
  READY = 0,
  STOPPED = 1,
  RUNNING,
  BLOCKED
}process_state_enum_t;

//function pointer
typedef void (*funcptr)(void);
//Memory header
struct memHeader {
  unsigned long size;
  struct memHeader *prev;
  struct memHeader *next;
  char* sanityCheck;
  unsigned char dataStart[0];
};

//CPU State
typedef struct context_frame{
    unsigned long   edi;
    unsigned long   esi;
    unsigned long   ebp;
    unsigned long   esp;
    unsigned long   ebx;
    unsigned long   edx;
    unsigned long   ecx;
    unsigned long   eax;
    unsigned long   iret_eip;
    unsigned long   iret_cs;
    unsigned long   eflags;
} context_frame_t;

typedef struct pcb_queue{
  struct pcb_queue* next; //next pointer
  int pid; // process id
}pcb_queue_t;
//PCB
typedef struct pcb{
    int pid; //process ID
    process_state_enum_t state; //prcess state
    unsigned long esp;
    void* space_start; //a pointer to allocated space
    pcb_queue_t* queue; //a pointer to the pcb_queue node
    
}pcb_t;

//init.c
void print_pcb_queue(process_state_enum_t state);
void test_mem_allocate_start(void);
void test_mem_allocate_middle(void);
void test_sys_create(void);
void sys_create_fn2(void);
void sys_create_fn1(void);
void test_get_next_proc(void);
void dummy(void);
extern pcb_queue_t* stopped_queue;
extern pcb_queue_t* ready_queue;
extern pcb_t pcb_table[PCB_TABLE_SIZE];
//mem.c
extern void kmeminit( void );
extern void *kmalloc( int size );
extern void kfree( void *ptr );
extern void printFreeMem(void);
//create.c
extern int create( void (*func)(), int stack );
extern int syscallReturn;
//disp.c
extern void dispinit(void);
extern void dispatch(void);
extern void pcbtableinit(void);
extern void enqueuepcb(process_state_enum_t state, pcb_t *new_pcb);
extern pcb_t *dequeuepcb(process_state_enum_t state);
extern  void cleanup(pcb_t *new_pcb);
//ctsw.c
extern int contextswitch(pcb_t *p );
extern void contextinit(void);
//syscall.c
extern void sysyield(void);
extern unsigned int syscreate( void (*func)(void), int stack);
extern void sysstop(void);
extern int syscall(int call, ... );
//user.c
extern void root(void);
extern void producer(void);
extern void consumer(void);
#endif

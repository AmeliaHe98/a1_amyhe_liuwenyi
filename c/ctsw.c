/* ctsw.c : context switcher
 */

#include <xeroskernel.h>

/* Your code goes here - You will need to write some assembly code. You must
   use the gnu conventions for specifying the instructions. (i.e this is the
   format used in class and on the slides.) You are not allowed to change the
   compiler/assembler options or issue directives to permit usage of Intel's
   assembly language conventions.
*/

void _ISREntryPoint(void);
void *k_stack;
static unsigned long ESP;
static unsigned long ECX;
static unsigned long EDX;
static int requestCode;
extern int syscallReturn;
long *args;
extern void contextinit () {
	set_evec(66, (unsigned long) _ISREntryPoint);
}

extern int contextswitch(pcb_t *p) {
	ESP = p->esp;
	kprintf("enter assembly");
	__asm __volatile("\
		pushf \n\
		pusha \n\
		movl %%esp, k_stack \n\
		movl ESP, %%esp \n\
		movl syscallReturn, %%eax \n\
		popa \n\
		iret \n\
		\
		_ISREntryPoint: \n\
	    	pusha \n\
	    	movl  %%esp, ESP \n\
	    	movl  k_stack, %%esp \n\
	    	movl  %%eax, requestCode \n\
	    	movl  %%edx, EDX \n\
	    	movl  %%ecx, ECX \n\
	    	popa \n\
	    	popf \n\
	  "
	  :
	  : 
      : "%eax"
	);
	kprintf("out of assembly");
	p->esp = ESP;
	//set up args
	*args = EDX;
	*(args+1) = ECX;

    return requestCode;
}

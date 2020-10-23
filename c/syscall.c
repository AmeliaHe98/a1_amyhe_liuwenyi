/* syscall.c : syscalls
 */

#include <xeroskernel.h>
#include <i386.h>
#include <stdarg.h>

// Store the value of call in one register, say eax, and a pointer to the start of the additional parameters (see man stdarg) in another register, say edx.
// 2. Execute an interrupt instruction to context switch into the kernel.
// 3. Upon return from the kernel, return the result stored in register eax
extern int syscall( int call, ... ) {
    //in syscreate, there are two additional parameters. 
    va_list arg_list;
    va_start(arg_list, call);
    int result=0;
    void (*func)(void) = va_arg(arg_list, void(*)(void));
	int stack = va_arg(arg_list, int);
    va_end(arg_list);  
    if (call == 0) {
        __asm__ __volatile__("\
			movl %1, %%eax \n\
			movl %2, %%edx \n\
			movl %3, %%ecx \n\
			int $66 \n\
			movl %%eax, %0 \n\
		  "
		  : "=r" (result)
		  :	"r" (call), "r" (func), "r" (stack)
		  : "%eax", "%edx"
		);
    } else if (call == 1 || call == 2) {
		__asm__ __volatile__("\
			movl %0, %%eax \n\
			int $66 \n\
		  "
		  : 
		  :	"r" (call)
		  : "%eax"
		);
	} else {
		kprintf("\n Error: invalid syscall. \n");
	}
    return result;
}

extern unsigned int syscreate( void (*func)(void), int stack ) {
    return syscall(0,func,stack);
}
/************************************
 * function name: sysyield
 * require: void
 * return: void
 * comment: make system call to yield current process for another on the ready queue
*************************************/
extern void sysyield( void ) {
    syscall(1);    

}
extern void sysstop( void ) {
    syscall(2);
}
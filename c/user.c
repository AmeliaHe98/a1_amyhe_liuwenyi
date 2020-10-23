/* user.c : User processes
 */

#include <xeroskernel.h>

void producer(void) {
	for(int i=0; i<15; i++) {
		kprintf("Happy 2019\n");
		for (int n = 0; n < 200000; n++);
		sysyield();
	}
	sysstop();
}

void consumer(void) {
	for(int i=0; i<15; i++) {
		kprintf("everyone\n");
		for (int n = 0; n < 200000; n++);
		sysyield();
	}
	sysstop();
}

extern void root( void ) {
	//a helper function that runs in root process.
	kprintf("Hello World!");
	syscreate(&producer, 2048);
	syscreate(&consumer, 2048);
	for (int n = 0; n < 200000; n++);
	for(;;) sysyield();
};
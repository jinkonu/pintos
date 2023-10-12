#include "userprog/syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "threads/palloc.h"

static void syscall_handler (struct intr_frame *);

void valid_check(void* vaddr) {
	if (!is_user_vaddr(vaddr)){
	  exit(-1);
	}
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{ 
  switch (*(uint32_t*)(f->esp)) {
  case SYS_HALT:
  	halt();
	break;
  case SYS_EXIT:
  	valid_check(f->esp + 4);
	exit(*(uint32_t*)(f->esp + 4));
	break;
  case SYS_EXEC:
  	valid_check(f->esp + 4);
  	f->eax = exec((const char*)*(uint32_t*)(f->esp + 4));
	break;
  case SYS_WAIT:
	valid_check(f->esp + 4);
	f->eax = wait((int)*(uint32_t*)(f->esp + 4));
  	break;
  case SYS_CREATE:
  	break;
  case SYS_REMOVE:
  	break;
  case SYS_OPEN:
  	break;
  case SYS_FILESIZE:
  	break;
  case SYS_READ:
	valid_check(f->esp + 20);
	valid_check(f->esp + 24);
	valid_check(f->esp + 28);
	f->eax = read((int*)*(uint32_t *)(f->esp + 20), (void *)*(uint32_t *)(f->esp + 24), (int)*(uint32_t *)(f->esp + 28));
  	break;
  case SYS_WRITE:
	valid_check(f->esp + 20);
	valid_check(f->esp + 24);
	valid_check(f->esp + 28);
	f->eax = write((int*)*(uint32_t *)(f->esp + 20), (void *)*(uint32_t *)(f->esp + 24), (int)*(uint32_t *)(f->esp + 28));
 	break; 	
}}

void halt(void){
	shutdown_power_off();
}

void exit(int status) {
	printf("%s: exit(%d)\n", thread_name(), status);
	
	// modified
	thread_current()->exit_status = status;	
	thread_exit();
}
int exec(const char *cmd_line) {
	return process_execute(cmd_line);
}
int wait(int pid) {
	return process_wait(pid);
}
int read(int fd, void *buffer, unsigned int size) {
	if (fd == 0) {
		*(char *)buffer = input_getc();

		return size;
	}
	else {
		return -1;
	}
}
int write(int fd, const void *buffer, unsigned int size) {
	if (fd == 1) {
		putbuf(buffer, size);

		return size;
	}
	else {
		return -1;
	}
}

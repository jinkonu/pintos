#include "userprog/syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <syscall-nr.h>
#include "lib/stdbool.h"
#include "threads/interrupt.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "threads/palloc.h"
#include "threads/synch.h"
#include "filesys/file.h"

static void syscall_handler (struct intr_frame *);

void valid_check(void* vaddr) {
	if (!is_user_vaddr(vaddr)){
	  exit(-1);
	}
}

struct semaphore file_sema;
void
syscall_init (void) 
{
  sema_init(&file_sema, 1);
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
	valid_check(f->esp + 16);
	valid_check(f->esp + 20);
	if ((const char*)*(uint32_t*)(f->esp + 16) == NULL) {
		exit(-1);
	}

	f->eax = create((const char*)*(uint32_t*)(f->esp + 16), (unsigned)*(uint32_t*)(f->esp + 20));
  	break;
  case SYS_REMOVE:
	valid_check(f->esp + 4);
	if ((const char*)*(uint32_t*)(f->esp + 4) == NULL) {
                exit(-1);
        }

	f->eax = remove((const char*)*(uint32_t*)(f->esp + 4));
  	break;
  case SYS_OPEN:
	valid_check(f->esp + 4);
	if ((const char*)*(uint32_t*)(f->esp + 4) == NULL) {
                exit(-1);
        }

	f->eax = open((const char*)*(uint32_t*)(f->esp + 4));
  	break;
  case SYS_FILESIZE:
	valid_check(f->esp + 4);
	f->eax = filesize((int)*(uint32_t*)(f->esp + 4));
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
  case SYS_SEEK:
	valid_check(f->esp + 16);
	valid_check(f->esp + 20);
	seek((int)*(uint32_t*)(f->esp + 16), (unsigned)*(uint32_t*)(f->esp + 20));
	break;
  case SYS_TELL:
	valid_check(f->esp + 4);
	f->eax = tell((int)*(uint32_t*)(f->esp + 4));
	break;
  case SYS_CLOSE:
	valid_check(f->esp + 4);
	close((int)*(uint32_t*)(f->esp + 4));
	break;
}}

void halt(void){
	shutdown_power_off();
}

void exit(int status) {
	printf("%s: exit(%d)\n", thread_name(), status);
	
	thread_current()->exit_status = status;	
	
	for (int walk = 3; walk < 128; walk++) {
		if (thread_current()->fileDescriptor[walk] != NULL) {
			close(walk);
		}
	}

	thread_exit();
}

int exec(const char *cmd_line) {
	return process_execute(cmd_line);
}

int wait(int pid) {
	return process_wait(pid);
}

bool create(const char *file, unsigned initial_size) {
	valid_check(file);
	return filesys_create(file, initial_size);	
}

bool remove(const char *file) {
	valid_check(file);
	return filesys_remove(file);
}

int open(const char *file) {
	if (file == NULL) {
		exit(-1);
	}
	valid_check(file);
	sema_down(&file_sema);
	
	struct file *tmp = filesys_open(file);
	int empty;
	if (tmp == NULL) {
		empty = -1;
	} else {
		for (empty = 3; empty < 128; empty++) {
			if (thread_current()->fileDescriptor[empty] == NULL) { 			        	char *name = thread_current()->name;
				if (strcmp(name, file) == 0) {
					file_deny_write(tmp);
				}

				thread_current()->fileDescriptor[empty] = tmp;
	     			break;
			}
		}
	}

	sema_up(&file_sema);

	if (empty == 128) { // not empty
		return -1;
	}
	return empty;
}

int filesize(int fd) {
	if (thread_current()->fileDescriptor[fd] == NULL) {
		exit(-1);
	}

	return file_length(thread_current()->fileDescriptor[fd]);
}

int read(int fd, void *buffer, unsigned int size) {
	valid_check(buffer);
	int actual = -1;

	sema_down(&file_sema);
	
	if (fd == 0) {	
		*(char *)buffer = input_getc();

		actual = 0;
		while (((char *)buffer)[actual] != '\0') {
			actual++;
		}
	}
	if (fd > 2) {
		if (thread_current()->fileDescriptor[fd] == NULL) {
			sema_up(&file_sema);
			exit(-1);
		}
		actual = file_read(thread_current()->fileDescriptor[fd], buffer, size);
	}

	sema_up(&file_sema);

	return actual;
}

int write(int fd, const void *buffer, unsigned int size) {
	valid_check(buffer);
	int actual;

	sema_down(&file_sema);

	if (fd == 1) {
		putbuf(buffer, size);

		actual = size;
	}
	if (fd > 2) {
		if (thread_current()->fileDescriptor[fd] == NULL) {
                        sema_up(&file_sema);
			exit(-1);
                }
		struct file* tmp = thread_current()->fileDescriptor[fd];
		if (tmp->deny_write) {
		       file_deny_write(thread_current()->fileDescriptor[fd]);
		}	       
		actual = file_write(thread_current()->fileDescriptor[fd], buffer, size);
	}
	
	sema_up(&file_sema);

	return actual;
}

void seek(int fd, unsigned position) {
	if (thread_current()->fileDescriptor[fd] == NULL) {
		exit(-1);
	}
	file_seek(thread_current()->fileDescriptor[fd], position);
}

unsigned tell(int fd) {
	if (thread_current()->fileDescriptor[fd] == NULL) {
                exit(-1);
        }
	return file_tell(thread_current()->fileDescriptor[fd]);
}

void close(int fd) {
	if (thread_current()->fileDescriptor[fd] == NULL) {
                exit(-1);
        }
	file_close(thread_current()->fileDescriptor[fd]);
	thread_current()->fileDescriptor[fd] = NULL;}

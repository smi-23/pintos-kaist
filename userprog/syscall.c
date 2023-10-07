#include "userprog/syscall.h"
#include "intrinsic.h"
#include "threads/flags.h"
#include "threads/interrupt.h"
#include "threads/loader.h"
#include "threads/thread.h"
#include "userprog/gdt.h"
#include <stdio.h>
#include <syscall-nr.h>

#include "include/lib/user/syscall.h"

void syscall_entry(void);
void syscall_handler(struct intr_frame *);

void check_addr(void *addr);

void halt(void);
void exit(int status);
pid_t fork(const char *thread_name);
int exec(const char *file);
int wait(pid_t pid);
bool create(const char *file, unsigned initial_size);
bool remove(const char *file);
int open(const char *file);
int filesize(int fd);
int read(int fd, void *buffer, unsigned size);
int write(int fd, const void *buffer, unsigned size);
void seek(int fd, unsigned position);
unsigned tell(int fd);
void close(int fd);

// int dup2(int oldfd, int newfd);
// void *mmap(void *addr, size_t length, int writable, int fd, off_t offset);
// void munmap(void *addr);

/* System call.
 *
 * Previously system call services was handled by the interrupt handler
 * (e.g. int 0x80 in linux). However, in x86-64, the manufacturer supplies
 * efficient path for requesting the system call, the `syscall` instruction.
 *
 * The syscall instruction works by reading the values from the the Model
 * Specific Register (MSR). For the details, see the manual. */

#define MSR_STAR 0xc0000081         /* Segment selector msr */
#define MSR_LSTAR 0xc0000082        /* Long mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084 /* Mask for the eflags */

void syscall_init(void) {
  write_msr(MSR_STAR, ((uint64_t)SEL_UCSEG - 0x10) << 48 | ((uint64_t)SEL_KCSEG)
                                                               << 32);
  write_msr(MSR_LSTAR, (uint64_t)syscall_entry);

  /* The interrupt service rountine should not serve any interrupts
   * until the syscall_entry swaps the userland stack to the kernel
   * mode stack. Therefore, we masked the FLAG_FL. */
  write_msr(MSR_SYSCALL_MASK,
            FLAG_IF | FLAG_TF | FLAG_DF | FLAG_IOPL | FLAG_AC | FLAG_NT);
}

/* The main system call interface */
void syscall_handler(struct intr_frame *f UNUSED) {
  // TODO: Your implementation goes here.
  int syscall_n = f->R.rax; /* 시스템 콜 넘버 */
#ifdef VM
  thread_current()->rsp = f->rsp;
#endif
  switch (syscall_n) {
  case SYS_HALT:
    halt();
    break;
  case SYS_EXIT:
    exit(f->R.rdi);
    break;
  case SYS_FORK:
    fork(f->R.rdi);
    break;
  case SYS_EXEC:
    exec(f->R.rdi);
    break;
  case SYS_WAIT:
    wait(f->R.rdi);
    break;
  case SYS_CREATE:
    create(f->R.rdi, f->R.rsi);
    break;
  case SYS_REMOVE:
    remove(f->R.rdi);
    break;
  case SYS_OPEN:
    open(f->R.rdi);
    break;
  case SYS_FILESIZE:
    filesize(f->R.rdi);
    break;
  case SYS_READ:
    read(f->R.rdi, f->R.rsi, f->R.rdx);
    break;
  case SYS_WRITE:
    write(f->R.rdi, f->R.rsi, f->R.rdx);
    break;
  case SYS_SEEK:
    seek(f->R.rdi, f->R.rsi);
    break;
  case SYS_TELL:
    tell(f->R.rdi);
    break;
  case SYS_CLOSE:
    close(f->R.rdi);
    break;
    //   case SYS_MMAP:
    //     f->R.rax = mmap(f->R.rdi, f->R.rsi, f->R.rdx, f->R.r10, f->R.r8);
    //     break;
    //   case SYS_MUNMAP:
    //     munmap(f->R.rdi);
    //     break;
  }
  // printf ("system call!\n"); // 테스트 결과를 위한 주석 처리
}

// SECTION - Project 2 USERPROG SYSTEM CALL
// SECTION - Process based System Call

void check_addr(void *addr) {
  if (addr == NULL)
    exit(-1);
  if (!is_user_vaddr(addr))
    exit(-1);
  // if (pml4_get_page(thread_current()->pml4, addr) == NULL)
  // 	exit(-1);
}

void halt(void) { power_off(); }

void exit(int status) {
  struct thread *cur = thread_current();
  printf("%s: exit(%d)\n", cur->name, status);
  thread_exit();
}

pid_t fork(const char *thread_name) {
  check_addr();
  process_fork();
}

int exec(const char *file) { check_address(file); }

int wait(pid_t pid) { return process_wait(pid); }
//! SECTION - Process based System Call
// SECTION - File based System Call
bool create(const char *file, unsigned initial_size) {}

bool remove(const char *file) {
  check_address(file);
  return filesys_remove(file);
}

int open(const char *file) {}

int filesize(int fd) {}

int read(int fd, void *buffer, unsigned size) {}

int write(int fd, const void *buffer, unsigned size) {
  check_addr(buffer);
  if (fd == 1) {
    putbuf(buffer, size);
    return size;
  }
  return -1;
}

void seek(int fd, unsigned position) {}

unsigned tell(int fd) {}

void close(int fd) {}
// !SECTION - File based System Call
/* Extra */
int dup2(int oldfd, int newfd) { return 0; }
// !SECTION - Project 2 USERPROG SYSTEM CALL

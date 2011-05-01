
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  proc.h
  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Forrest Yu, 2005
  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef _ORANGES_PROC_H_
#define _ORANGES_PROC_H_

#include "protect.h"
#include "type.h"

typedef struct _StackFrame {	/* proc_ptr points here				↑ Low			*/
	 u32	gs;		/* ┓						│			*/
	 u32	fs;		/* ┃						│			*/
	 u32	es;		/* ┃						│			*/
	 u32	ds;		/* ┃						│			*/
	 u32	edi;		/* ┃						│			*/
	 u32	esi;		/* ┣ pushed by save()				│			*/
	 u32	ebp;		/* ┃						│			*/
	 u32	kernel_esp;	/* <- 'popad' will ignore it			│			*/
	 u32	ebx;		/* ┃						↑栈从高地址往低地址增长*/		
	 u32	edx;		/* ┃						│			*/
	 u32	ecx;		/* ┃						│			*/
	 u32	eax;		/* ┛						│			*/
	 u32	retaddr;	/* return address for assembly code save()	│			*/
	 u32	eip;		/*  ┓						│			*/
	 u32	cs;		/*  ┃						│			*/
	 u32	eflags;		/*  ┣ these are pushed by CPU during interrupt	│			*/
	 u32	esp;		/*  ┃						│			*/
	 u32	ss;		/*  ┛						┷High			*/
}StackFrame;


typedef struct _Process {
	 StackFrame regs;			/* process' registers saved in stack frame */

	 u16 ldt_sel;		/* selector in gdt giving ldt base and limit*/
	 Descriptor	ldts[LDT_SIZE];		/* local descriptors for code and data */
	 /* 2 is LDT_SIZE - avoid include protect.h */
	 int ticks;
	 int priority;
	 u32 pid;			/* process id passed in from MM */
	 char name[16];		/* name of the process */

	 int p_flags;
	 Message *p_msg;
	 int p_recvfrom;
	 int p_sendto;

	 int has_int_msg;			/*
								 * nozero if an INTERRUPT occurred when
								 * the task is not ready to deal with it
								 */
	 struct _Process *q_sending;		/* queue of procs sending messages to
									 * this proc
									 */
	 struct _Process *next_sending;		/* next proc in the sending
										 * queue (q_sending)
										 */

	 int                nr_tty;
}Process;


typedef struct _Task {
	 task_f	initial_eip;
	 int	stacksize;
	 char	name[32];
}Task;

#define proc2pid(p) (p - proc_table)

/* Number of tasks & procs */
#define NR_TASKS	2
#define NR_PROCS    3

/* stacks of tasks */
#define STACK_SIZE_TESTA	0x8000
#define STACK_SIZE_TESTB	0x8000
#define STACK_SIZE_TESTC	0x8000
#define STACK_SIZE_TTY      0x8000
#define STACK_SIZE_SYS      0x8000

#define STACK_SIZE_TOTAL	(STACK_SIZE_TESTA + \
							 STACK_SIZE_TESTB + \
							 STACK_SIZE_TESTC + \
							 STACK_SIZE_TTY   + \
							 STACK_SIZE_SYS)

#endif

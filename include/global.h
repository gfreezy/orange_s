
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef	_ORANGES_GLOBAL_H_
#define _ORANGES_GLOBAL_H_

#ifdef GLOBAL_VARIABLES_HERE
#undef EXTERN
#define EXTERN
#endif

#include "type.h"
#include "const.h"
#include "proc.h"
#include "protect.h"

EXTERN int disp_pos;
EXTERN u8 gdt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN Descriptor gdt[GDT_SIZE];
EXTERN u8 idt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN Gate idt[IDT_SIZE];

EXTERN u32 k_reenter;

EXTERN Tss	tss;
EXTERN Process* p_proc_ready;
EXTERN int ticks;
EXTERN int nr_current_console;

extern	Process		proc_table[];
extern	char		task_stack[];
extern  Task        task_table[];
extern  Task        user_proc_table[];
extern  irq_handler irq_table[];
extern  TTY         tty_table[];
extern  Console     console_table[];
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  main.c
  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Forrest Yu, 2005
  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"


/*======================================================================*
  kernel_main
  *======================================================================*/
PUBLIC int kernel_main()
{
	 disp_str("-----\"kernel_main\" begins-----\n");

	 Task *p_task;
	 Process *p_proc = proc_table;
	 char *p_task_stack = task_stack + STACK_SIZE_TOTAL;
	 u16 selector_ldt = SELECTOR_LDT_FIRST;

	 u8 privilege;
	 u8 rpl;
	 int eflags;
	 u8 priority;
	 
	 int i;
	 for(i = 0; i < NR_TASKS + NR_PROCS; i++)
	 {
		  if(i < NR_TASKS)		/* 任务 */
		  {
			   p_task = task_table + i;
			   privilege = PRIVILEGE_TASK;
			   rpl = RPL_TASK;
			   eflags = 0x1202;					/* IF=1, IOPL=1, bit 2 is always 1 */
			   priority = 15;
		  }
		  else					/* 用户进程 */
		  {
			   p_task = user_proc_table + (i - NR_TASKS);
			   privilege = PRIVILEGE_USER;
			   rpl = RPL_USER;
			   eflags = 0x202;					/* IF =1, bit 2 is always 1 */
			   priority = 5;
		  }
		  
		  strcpy(p_proc->name, p_task->name);	// name of the process
		  p_proc->pid = i;

		  p_proc->ldt_sel = selector_ldt;

		  memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
				 sizeof(Descriptor));
		  p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		  memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
				 sizeof(Descriptor));
		  p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		  
		  p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			   | SA_TIL | rpl;
		  p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			   | SA_TIL | rpl;
		  p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			   | SA_TIL | rpl;
		  p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			   | SA_TIL | rpl;
		  p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			   | SA_TIL | rpl;
		  p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			   | rpl;

		  p_proc->regs.eip = (u32)p_task->initial_eip;
		  p_proc->regs.esp = (u32)p_task_stack;
		  p_proc->regs.eflags = eflags; /* IF=1, IOPL=1 */

		  p_proc->nr_tty = 0;
		  
		  p_proc->p_flags = 0;
		  p_proc->p_recvfrom = NO_TASK;
		  p_proc->p_sendto = NO_TASK;
		  p_proc->has_int_msg = 0;
		  p_proc->q_sending = 0;
		  p_proc->next_sending = 0;

		  p_proc->ticks = p_proc->priority = priority;
		  
		  p_task_stack -= p_task->stacksize;
		  p_proc++;
		  p_task++;
		  selector_ldt += 1 << 3;
	 }

	 /* 设置显示的tty*/
	 proc_table[NR_TASKS + 0].nr_tty = 0;
	 proc_table[NR_TASKS + 1].nr_tty = 1;
	 proc_table[NR_TASKS + 2].nr_tty = 1;
	
	 /* 初始化时钟中断 */
	 init_clock();

	 /* 中断重入标志 */
	 k_reenter = 0;

	 /* 时钟计数 */
	 ticks = 0;

	 p_proc_ready	= proc_table;
	 restart();
	 while(1){}
}

PUBLIC void panic(const char *fmt, ...)
{
	 int i;
	 char buf[256];

	 /* 4 is the size of fmt in the stack */
	 va_list arg = (va_list)((char *)&fmt + 4);

	 i = vsprintf(buf, fmt, arg);
	 printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	 /* should never arrive here */
	 __asm__ __volatile__("ud2");
}

/*======================================================================*
  TestA
  *======================================================================*/
PUBLIC void TestA()
{
	 while(1)
	 {
		  /* disp_color_str("A.", BRIGHT | MAKE_COLOR(BLACK, RED)); */
		  /* disp_int(get_ticks()); */
		  printf("<Ticks:%x>", get_ticks());
		  /* assert(0); */
		  /* printf("hello"); */
		  milli_delay(200);
	 }
}

/*======================================================================*
  TestB
  *======================================================================*/
PUBLIC void TestB()
{
	 while(1)
	 {
		  /* disp_color_str("B.", BRIGHT | MAKE_COLOR(BLACK, RED)); */
		  /* disp_int(get_ticks()); */
		  printf("B");
		  milli_delay(200);
	 }
}

/*======================================================================*
  TestB
  *======================================================================*/
PUBLIC void TestC()
{
	 while(1)
	 {
		  /* disp_color_str("C.", BRIGHT | MAKE_COLOR(BLACK, RED)); */
		  /* disp_int(get_ticks()); */
		  printf("C");
		  milli_delay(200);
	 }
}

PUBLIC int get_ticks()
{
	 Message msg;
	 reset_msg(&msg);
	 msg.type = GET_TICKS;
	 send_recv(BOTH, TASK_SYS, &msg);
	 return msg.RETVAL;
}

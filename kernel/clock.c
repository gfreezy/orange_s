
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               clock.c
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
                           clock_handler
 *======================================================================*/
PUBLIC void clock_handler(int irq)
{
	/* disp_str("#");			 */
	ticks++;
	p_proc_ready->ticks--;

	if(k_reenter != 0)
    {
        /* disp_str("!"); */
        return;
    }

	if(p_proc_ready->ticks > 0)
	{
		 return;
	}
	
	schedule();
	/* p_proc_ready++; */
	/* if (p_proc_ready >= proc_table + NR_TASKS)  */
	/* { */
	/* 	p_proc_ready = proc_table; */
	/* } */
}

PUBLIC void init_clock()
{
	 /* 初始化 8253 PIT */
	 out_byte(TIMER_MODE, RATE_GENERATOR);
	 out_byte(TIMER0, (u8) (TIMER_FREQ/HZ));
	 out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8));

	 /* 初始化时钟中断 */
	 put_irq_handler(CLOCK_IRQ, clock_handler);
	 enable_irq(CLOCK_IRQ);

}

PUBLIC void milli_delay(int milli_sec)
{
	 int t = get_ticks();

	 while(((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}

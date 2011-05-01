#include "const.h"
#include "type.h"
#include "protoh."

PRIVATE void init_hd();
/** 
 * main loop of hd
 * 
 */
PUBLIC void task_hd()
{
	 Message msg;

	 init_hd();

	 while(1)
	 {
		  send_recv(RECEIVE, ANY, &msg);

		  int src = msg.source;

		  switch(msg.type)
		  {
		  case DEV_OPEN:
			   hd_identify(0);
			   break;
		  default:
			   dump_msg("HD driver::unkonwn msg", &msg);
			   spin("FS::main loop (invalid msg type)");
			   break;
		  }

		  send_recv(SEND, src, &msg);
	 }
}

/** 
 * <Ring 0> Interrupt handler.
 * 
 * @param irq IRQ nr of the disk interrupt.
 */
PUBLIC void hd_handler(int irq)
{
	 /** 
	  * Interrupts are cleard when the host
	  *   - reads the Status Register,
	  *   - issues a reset, or
	  *   - writes to the Command Register.
	  */
	 hd_status = in_byte(REG_STATUS);

	 inform_int(TASK_HD);
}


/** 
 * <Ring 1> Check hard drive, set IRQ handler, enable IRQ and initialize data
 *          structures.
 */
PRIVATE void init_hd()
{
	 /* Get the number of drives from the bios data area */
	 u8 *p_nr_drives = (u8 *)(0x475);
	 printl("NrDrivers:%d.\n", *p_nr_drives);
	 assert(*p_nr_drives);

	 put_irq_handler(AT_WINI_IRQ, hd_handler);
	 enable_irq(CASCADE_IRQ);
	 enable_irq(AT_WINI_IRQ);
}

/** 
 * <Ring 1> Get the disk information.
 * 
 * @param drive Drive Nr
 */
PRIVATE void hd_identify(int drive)
{
	 
}




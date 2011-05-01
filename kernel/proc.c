#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "string.h"

PRIVATE void block(Process *p);
PRIVATE void unblock(Process *p);
PRIVATE int deadlock(int src, int dest);
PRIVATE int msg_send(Process *current, int dest, Message *m);
PRIVATE int msg_receive(Process *current, int src, Message *m);

PUBLIC void schedule()
{
	 Process* p;
	 int greatest_ticks = 0;

	 while(!greatest_ticks)
	 {
		  for(p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++)
		  {
			   if(p->p_flags == 0)
			   {
					if(p->ticks > greatest_ticks)
					{
						 /* disp_str("<"); */
						 /* disp_int(p->ticks); */
						 /* disp_str(">"); */
						 greatest_ticks = p->ticks;
						 p_proc_ready = p;
					}
			   }
		  }

		  if(!greatest_ticks)
		  {
		  	   for(p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++)
		  	   {
					if(p->p_flags == 0)
					{
						 p->ticks = p->priority;
					}
		  	   }
		  }
	 }
}

PUBLIC int sys_sendrec(int function, int src_dest, Message *p_msg, Process *p_proc)
{
	 assert(k_reenter == 0);
	 assert((src_dest >= 0 && src_dest < NR_TASKS + NR_PROCS) ||
			src_dest == ANY ||
			src_dest == INTERRUPT);

	 int ret = 0;
	 int caller = proc2pid(p_proc);
	 Message *mla = (Message *)va2la(caller, p_msg);
	 mla->source = caller;

	 assert(mla->source != src_dest);

	 /* Actually we have the third message type: BOTH. However, it is not
	 * allowed to be passed to the kernel directly. Kernel doesn't know
	 * it at all. It is transformed into a SEDN followed by a RECEIVE
	 * by 'send_recv()'.
	 */
	 if(function == SEND)
	 {
		  ret = msg_send(p_proc, src_dest, p_msg);
		  if(ret != 0)
		  {
			   return ret;
		  }
	 }
	 else if(function == RECEIVE)
	 {
		  ret = msg_receive(p_proc, src_dest, p_msg);
		  if(ret != 0)
		  {
			   return ret;
		  }
	 }
	 else
	 {
		  panic("{sys_sendrec} invalid function: "
				"%d (SEND: %d, RECEIVE: %d).",
				function, SEND, RECEIVE);
	 }
	 return 0;
}

/** 
 * <Ring 0~1> Virtual addr --> Linear addr.
 * 
 * @param pid  PID of the proc whose address is to be calculated.
 * @param va   Virtual address.
 * 
 * @return The linear address for the given virtual address.
 */
PUBLIC void * va2la(int pid, void *va)
{
	 Process* p = &proc_table[pid];

	 u32 seg_base = ldt_seg_linear(p, INDEX_LDT_RW);
	 u32 la = seg_base + (u32)va;

	 if(pid < NR_TASKS + NR_PROCS)
	 {
		  assert(la == (u32)va);
	 }

	 return (void *)la;
}

/** 
 * <Ring 0~1> Calculate the linear address of a certain segment of a given
 * proc.
 * 
 * @param p   Whose (the proc ptr).
 * @param idx Which (one proc has more than one segments).
 * 
 * @return  The required linear address.
 */
PUBLIC int ldt_seg_linear(Process *p, int idx)
{
	 Descriptor *d = &p->ldts[idx];

	 return d->base_high << 24 | d->base_mid << 16 | d->base_low;
}

/** 
 * <Ring 0~3> Clear up a Message by setting each byte to 0.
 *
 * @param p The message to be cleared
 */
PUBLIC void reset_msg(Message *p)
{
	 memset(p, 0, sizeof(Message));
}

/** 
 * <Ring 1~3> IPC syscall
 *
 * It is an encapsulation of 'sendrec',
 * invoking 'sendrec' directly should be avoided
 * 
 * @param function SEND, RECEIVE or BOTH
 * @param src_dest The caller's proc_nr
 * @param msg Pointer to the Message struct
 * 
 * @return always 0
 */
PUBLIC int send_recv(int function, int src_dest, Message *msg)
{
	 int ret = 0;

	 if(function == RECEIVE)
	 {
		  reset_msg(msg);
	 }
	 
	 switch(function)
	 {
	 case BOTH:
		  ret = sendrec(SEND, src_dest, msg);
		  if(ret == 0)
		  {
			   ret = sendrec(RECEIVE, src_dest, msg);
		  }
		  break;
	 case SEND:
	 case RECEIVE:
		  ret = sendrec(function, src_dest, msg);
		  break;
	 default:
		  assert((function == BOTH) ||
				 (function == SEND) ||
				 (function == RECEIVE));
		  break;
	 }

	 return ret;
}

/** 
 * <Ring 0> This routine is called after 'p_flags' has been set (!= 0), it
 * calls 'sechedule()' to choose another proc as the 'proc_ready'.
 *
 * @attention This routine does not change 'p_flags'. Make sure the 'p_flags'
 * of the proc to be blocked has been set properly.
 *
 * @param p The proc to be blocked
 */
PRIVATE void block(Process *p)
{
	 assert(p->p_flags);
	 schedule();
}

PRIVATE void unblock(Process *p)
{
	 assert(p->p_flags == 0);
}

/** 
 * <Ring 0> Check whether it is safe to send a message from src to dest.
 * The routine will detect if the messaging graph contains a cycle. For
 * instance, if we have procs trying to send messages like his:
 * A-> B -> C -> A, the a deadlock occurs, because all of them will
 * wait forever. If no cycles detected, it is considered as safe.
 *
 * @param src Who wants to send message.
 * @param dest To whom the message is send.
 * 
 * @return Zero if success
 */
PRIVATE int deadlock(int src, int dest)
{
	 Process *p = proc_table + dest;
	 while(1)
	 {
		  if(p->p_flags & SENDING)
		  {
			   if(p->p_sendto == src)
			   {
					/* print the chain */
					p = proc_table + dest;
					printl("=_=%s", p->name);
					do
					{
						 assert(p->p_msg);
						 p = proc_table + p->p_sendto;
						 printl("->%s", p->name);
					}while(p != proc_table + src);
					printl("=_=");

					return 1;
			   }
			   p = proc_table + p->p_sendto;
		  }
		  else
		  {
			   break;
		  }
	 }
	 return 0;
}

/** 
 * <Ring 0> Send a message to the dest proc. If dest is blocked waiting for
 * the message, copy the message to it and unblock dest. Otherwise the caller
 * will be blocked and appended to the dest's sending queue.
 * 
 * @param current The caller, the sender.
 * @param dest To whom the message is send
 * @param m The message
 * 
 * @return Zero if success
 */
PRIVATE int msg_send(Process *current, int dest, Message *m)
{
	 Process *sender = current;
	 Process *p_dest = proc_table + dest; /* proc dest */

	 assert(proc2pid(sender) != dest);

	 /* check for deadlock here */
	 if(deadlock(proc2pid(sender), dest))
	 {
		  panic(">>DEADLOCK<< %s->%s", sender->name, p_dest->name);
	 }

	 if((p_dest->p_flags & RECEIVING) && /* dest is waitting for the msg */
		(p_dest->p_recvfrom == proc2pid(sender)) ||
		(p_dest->p_recvfrom == ANY))
	 {
		  assert(p_dest->p_msg);
		  assert(m);

		  phys_copy(va2la(dest, p_dest->p_msg),
					va2la(proc2pid(sender), m),
					sizeof(Message));
		  p_dest->p_msg = 0;
		  p_dest->p_flags &= ~RECEIVING; /* dest has received the msg */
		  p_dest->p_recvfrom = NO_TASK;
		  unblock(p_dest);
		  assert(p_dest->p_flags == 0);
		  assert(p_dest->p_msg == 0);
		  assert(p_dest->p_recvfrom == NO_TASK);
		  assert(p_dest->p_sendto == NO_TASK);
		  assert(sender->p_flags == 0);
		  assert(sender->p_msg == 0);
		  assert(sender->p_recvfrom == NO_TASK);
		  assert(sender->p_sendto == NO_TASK);
	 }
	 else
	 {
		  /* dest is not waitting for the msg */
		  sender->p_flags |= SENDING;
		  assert(sender->p_flags == SENDING);
		  sender->p_sendto = dest;
		  sender->p_msg = m;

		  /* append to the sending queue */
		  Process *p;
		  if(p_dest->q_sending)
		  {
			   p = p_dest->q_sending;
			   while(p->next_sending)
			   {
					p = p->next_sending;
			   }
			   p->next_sending = sender;
		  }
		  else
		  {
			   p_dest->q_sending = sender;
		  }
		  sender->next_sending = 0;

		  block(sender);

		  assert(sender->p_flags == SENDING);
		  assert(sender->p_msg != 0);
		  assert(sender->p_recvfrom == NO_TASK);
		  assert(sender->p_sendto == dest);
	 }

	 return 0;
		  
}

/** 
 * <Ring 0> Try to get a message from the src proc. If src is blocked sending
 * the message, copy the message form it and unblock src. Otherwise the caller
 * will be blocked.
 * 
 * @param current The caller, the proc who wanna receive.
 * @param src From whom the message wil be receive
 * @param m The message ptr to accept the message
 * 
 * @return Zero if success
 */
PRIVATE int msg_receive(Process *current, int src, Message *m)
{
	 Process *p_who_wanna_recv = current; /* This name is a little
										   * wierd, but it makes me
										   * think clearly, so i keep
										   * it.
										   */
	 Process *p_from = 0;		/* from whick the message will be fetched */
	 Process *prev = 0;
	 int copyok = 0;

	 assert(proc2pid(p_who_wanna_recv) != src);

	 if((p_who_wanna_recv->has_int_msg) &&
		((src == ANY) || (src == INTERRUPT)))
	 {
		  /* There is an interrupt needs p_who_wanna_recv's handling and
		   * p_who_wanna_recv is ready to handle it.
		   */

		  Message msg;
		  reset_msg(&msg);
		  msg.source = INTERRUPT;
		  msg.type = HARD_INT;

		  assert(m);

		  phys_copy(va2la(proc2pid(p_who_wanna_recv), m), &msg,
					sizeof(Message));

		  p_who_wanna_recv->has_int_msg = 0;

		  assert(p_who_wanna_recv->p_flags == 0);
		  assert(p_who_wanna_recv->p_msg == 0);
		  assert(p_who_wanna_recv->p_sendto == NO_TASK);
		  assert(p_who_wanna_recv->has_int_msg == 0);

		  return 0;
	 }

	 /* Arrive here if no interrupt for p_who_wanna_recv */
	 if(src == ANY)
	 {

		  /* p_who_wanna_recv is ready to receive messages from
		   * ANY proc, we'll check the sending queue and pick the
		   * firs proc in it.
		   */
		  if(p_who_wanna_recv->q_sending)
		  {
			   p_from = p_who_wanna_recv->q_sending;
			   copyok = 1;

			   assert(p_who_wanna_recv->p_flags == 0);
			   assert(p_who_wanna_recv->p_msg == 0);
			   assert(p_who_wanna_recv->p_recvfrom == NO_TASK);
			   assert(p_who_wanna_recv->p_sendto == NO_TASK);
			   assert(p_who_wanna_recv->q_sending != 0);
			   assert(p_from->p_flags == SENDING);
			   assert(p_from->p_msg != 0);
			   assert(p_from->p_recvfrom == NO_TASK);
			   assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));
		  }
	 }
	 else if(src >= 0 && src < NR_TASKS + NR_PROCS)
	 {
		  /* p_who_wanna_recv want s to receive amessage from
		   * a certain proc: src
		   */
		  p_from = &proc_table[src];

		  if((p_from->p_flags & SENDING) &&
			 (p_from->p_sendto == proc2pid(p_who_wanna_recv)))
		  {
			   /* Perfect, src is sending a message to
				* p_who_wanna_recv
				*/
			   copyok = 1;

			   Process *p = p_who_wanna_recv->q_sending;

			   assert(p);		/* p_from must have been appended to the
								 * queue, so the queue must not be NULL
								 */

			   while(p)
			   {
					assert(p_from->p_flags & SENDING);

					if(proc2pid(p) == src) /* if p is src */
					{
						 break;
					}

					prev = p;
					p = p->next_sending;
			   }
			   

			   assert(p_who_wanna_recv->p_flags == 0);
			   assert(p_who_wanna_recv->p_msg == 0);
			   assert(p_who_wanna_recv->p_recvfrom == NO_TASK);
			   assert(p_who_wanna_recv->p_sendto == NO_TASK);
			   assert(p_who_wanna_recv->q_sending != 0);
			   assert(p_from->p_flags == SENDING);
			   assert(p_from->p_msg != 0);
			   assert(p_from->p_recvfrom == NO_TASK);
			   assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));
		  }
	 }

	 if(copyok)
	 {
		  /* It's determined from which proc the message will
		   * be copied. Note that this proc must have been
		   * waiting for this moment in the queue, so we should
		   * remove it from the queue.
		   */
		  /* 将p_from从队列中删除 */
		  if(p_from == p_who_wanna_recv->q_sending) /* the 1st one */
		  {
			   assert(prev == 0);
			   p_who_wanna_recv->q_sending = p_from->next_sending;
			   p_from->next_sending = 0;
		  }
		  else
		  {
			   assert(prev);
			   prev->next_sending = p_from->next_sending;
			   p_from->next_sending = 0;
		  }

		  assert(m);
		  assert(p_from->p_msg);

		  /* copy the message */
		  phys_copy(va2la(proc2pid(p_who_wanna_recv), m),
					va2la(proc2pid(p_from), p_from->p_msg),
					sizeof(Message));

		  p_from->p_msg = 0;
		  p_from->p_sendto = NO_TASK;
		  p_from->p_flags &= ~SENDING;
		  
		  unblock(p_from);
	 }
	 else
	 {
		  /* nobody's sending any msg
		   * Set p_flags so that p_who_wanna_recv will not
		   * be scheduled until it is unblocked.
		   */
		  p_who_wanna_recv->p_flags |= RECEIVING;

		  p_who_wanna_recv->p_msg = m;
		  p_who_wanna_recv->p_recvfrom = src;
		  block(p_who_wanna_recv);

		  assert(p_who_wanna_recv->p_flags == RECEIVING);
		  assert(p_who_wanna_recv->p_msg != 0);
		  assert(p_who_wanna_recv->p_recvfrom != NO_TASK);
		  assert(p_who_wanna_recv->p_sendto == NO_TASK);
		  assert(p_who_wanna_recv->has_int_msg == 0);
	 }

	 return 0;

}


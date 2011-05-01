#include "proto.h"
#include "const.h"
#include "global.h"

PUBLIC void task_sys()
{
	 Message msg;
	 while(1)
	 {
		  send_recv(RECEIVE, ANY, &msg);
		  int src = msg.source;

		  switch(msg.type)
		  {
		  case GET_TICKS:
			   msg.RETVAL = ticks;
			   send_recv(SEND, src, &msg);
			   break;
		  default:
			   panic("unknown msg type");
			   break;
		  }
	 }
}

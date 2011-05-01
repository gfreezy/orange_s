#ifndef _ORANGE_TTY_H_
#define _ORANGE_TTY_H_

#include "const.h"

#define TTY_IN_BYTES 256		/* tty input queue size */

struct _Console;

/* TTY */
typedef struct _TTY
{
	 u32 in_buf[TTY_IN_BYTES];	/* TTY缓冲区 */
	 u32* p_inbuf_head;			/* 指向缓冲区中下一个空闲位置 */
	 u32* p_inbuf_tail;			/* 指向键盘任务应处理的键值 */
	 int inbuf_count;			/* 缓冲区中已经填充了多少 */

	 struct _Console* p_console;
}TTY;

#endif

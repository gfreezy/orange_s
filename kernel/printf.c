#include "proto.h"
#include "type.h"

PUBLIC int printf(const char* fmt, ...)
{
	 int i;
	 char buf[256];

	 va_list arg = (va_list)((char*)(&fmt) + 4); /* 4是参数fmt所占堆栈中的大小 */
	 i = vsprintf(buf, fmt, arg);				 /* i 是字符串的长度 */
	 buf[i] = 0;
	 printx(buf);

	 return i;
}

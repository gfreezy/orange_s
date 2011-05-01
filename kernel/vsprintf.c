#include "type.h"
#include "proto.h"
#include "string.h"

PRIVATE char * i2a(int val, int base, char **ps)
{
	 int m = val % base;
	 int q = val / base;
	 if(q)
	 {
		  i2a(q, base, ps);
	 }
	 *(*ps)++ = (m < 10) ? (m + '0') : (m - 10 + 'A');

	 return *ps;
}

/* PUBLIC int vsprintf(char *buf, const char *fmt, va_list args) */
/* { */
/* 	 char*	p; */

/* 	 va_list	p_next_arg = args; */
/* 	 int	m; */

/* 	 char	inner_buf[STR_DEFAULT_LEN]; */
/* 	 char	cs; */
/* 	 int	align_nr; */

/* 	 for (p=buf;*fmt;fmt++) { */
/* 		  if (*fmt != '%') { */
/* 			   *p++ = *fmt; */
/* 			   continue; */
/* 		  } */
/* 		  else {		/\* a format string begins *\/ */
/* 			   align_nr = 0; */
/* 		  } */

/* 		  fmt++; */

/* 		  if (*fmt == '%') { */
/* 			   *p++ = *fmt; */
/* 			   continue; */
/* 		  } */
/* 		  else if (*fmt == '0') { */
/* 			   cs = '0'; */
/* 			   fmt++; */
/* 		  } */
/* 		  else { */
/* 			   cs = ' '; */
/* 		  } */
/* 		  while (((unsigned char)(*fmt) >= '0') && ((unsigned char)(*fmt) <= '9')) { */
/* 			   align_nr *= 10; */
/* 			   align_nr += *fmt - '0'; */
/* 			   fmt++; */
/* 		  } */

/* 		  char * q = inner_buf; */
/* 		  memset(q, 0, sizeof(inner_buf)); */

/* 		  switch (*fmt) { */
/* 		  case 'c': */
/* 			   *q++ = *((char*)p_next_arg); */
/* 			   p_next_arg += 4; */
/* 			   break; */
/* 		  case 'x': */
/* 			   m = *((int*)p_next_arg); */
/* 			   i2a(m, 16, &q); */
/* 			   p_next_arg += 4; */
/* 			   break; */
/* 		  case 'd': */
/* 			   m = *((int*)p_next_arg); */
/* 			   if (m < 0) { */
/* 					m = m * (-1); */
/* 					*q++ = '-'; */
/* 			   } */
/* 			   i2a(m, 10, &q); */
/* 			   p_next_arg += 4; */
/* 			   break; */
/* 		  case 's': */
/* 			   strcpy(q, (*((char**)p_next_arg))); */
/* 			   q += strlen(*((char**)p_next_arg)); */
/* 			   p_next_arg += 4; */
/* 			   break; */
/* 		  default: */
/* 			   break; */
/* 		  } */

/* 		  int k; */
/* 		  for (k = 0; k < ((align_nr > strlen(inner_buf)) ? (align_nr - strlen(inner_buf)) : 0); k++) { */
/* 			   *p++ = cs; */
/* 		  } */
/* 		  q = inner_buf; */
/* 		  while (*q) { */
/* 			   *p++ = *q++; */
/* 		  } */
/* 	 } */

/* 	 *p = 0; */

/* 	 return (p - buf); */
/* } */


int vsprintf(char *buf, const char *fmt, va_list args)
{
	 char *p;
	 char tmp[256];
	 va_list p_next_arg = args;
	 int m;
	 
	 for(p = buf; *fmt; fmt++)
	 {
		  if(*fmt != '%')
		  {
			   *p++ = *fmt;
			   continue;
		  }

		  fmt ++;

		  switch(*fmt)
		  {
		  case 'x':
			   itoa(tmp, *((int *)p_next_arg));
			   strcpy(p, tmp);
			   p_next_arg += 4;
			   p += strlen(tmp);
			   break;
		  case 's':
			   strcpy(p, *((char **)p_next_arg));
			   p += strlen(*((char **)p_next_arg));
			   p_next_arg += 4;
			   break;
		  case 'c':
			   *p++ = *((char *)p_next_arg);
			   p_next_arg += 4 ;
			   break;
		  case 'd':
			   m = *((int *)p_next_arg);
			   i2a(m, 10, &p);
			   p_next_arg += 4;
			   break;
		  default:
			   break;
		  }
	 }
	 return (p - buf);
}

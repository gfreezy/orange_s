#include "const.h"
#include "proto.h"


PUBLIC void assertion_failure(char *exp, char *file, char *base_file, int line)
{
	 printl("%c assert(%s) failed: file: %s, base_file: %s, line %d",
			MAG_CH_ASSERT, exp, file, base_file, line);

	 spin("assertion_failure()");

	 /* should never arrive here */
	 __asm__ __volatile__("ud2");
}

PUBLIC void spin(char *func_name)
{
	 printl("\nspinning in %s ...\n", func_name);
	 while(1){}
}

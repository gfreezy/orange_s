
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            type.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef	_ORANGES_TYPE_H_
#define	_ORANGES_TYPE_H_

typedef unsigned long long  u64;
typedef	unsigned int		u32;
typedef	unsigned short		u16;
typedef	unsigned char		u8;
typedef char * va_list;

typedef	void	(*int_handler)	();
typedef	void	(*task_f)	();
typedef void    (*irq_handler) (int irq);
typedef void    *system_call;

/* 存储段描述符/系统段描述符 */
typedef struct _Descriptor		/* 共 8 个字节 */
{
	 u16	limit_low;		/* Limit */
	 u16	base_low;		/* Base */
	 u8	base_mid;		/* Base */
	 u8	attr1;			/* P(1) DPL(2) DT(1) TYPE(4) */
	 u8	limit_high_attr2;	/* G(1) D(1) 0(1) AVL(1) LimitHigh(4) */
	 u8	base_high;		/* Base */
}Descriptor;

/* 门描述符 */
typedef struct _Gate
{
	 u16	offset_low;	/* Offset Low */
	 u16	selector;	/* Selector */
	 u8	dcount;		/* 该字段只在调用门描述符中有效。
					   如果在利用调用门调用子程序时引起特权级的转换和堆栈的改变，需要将外层堆栈中的参数复制到内层堆栈。
					   该双字计数字段就是用于说明这种情况发生时，要复制的双字参数的数量。 */
	 u8	attr;		/* P(1) DPL(2) DT(1) TYPE(4) */
	 u16	offset_high;	/* Offset High */
}Gate;

typedef struct _Tss {
	 u32	backlink;
	 u32	esp0;		/* stack pointer to use during interrupt */
	 u32	ss0;		/*   "   segment  "  "    "        "     */
	 u32	esp1;
	 u32	ss1;
	 u32	esp2;
	 u32	ss2;
	 u32	cr3;
	 u32	eip;
	 u32	flags;
	 u32	eax;
	 u32	ecx;
	 u32	edx;
	 u32	ebx;
	 u32	esp;
	 u32	ebp;
	 u32	esi;
	 u32	edi;
	 u32	es;
	 u32	cs;
	 u32	ss;
	 u32	ds;
	 u32	fs;
	 u32	gs;
	 u32	ldt;
	 u16	trap;
	 u16	iobase;	/* I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图 */
	 /*u8	iomap[2];*/
}Tss;

/* Message 的类型 */
typedef enum _MsgType
{
	 HARD_INT = 1,

	 GET_TICKS = 2,
}MsgType;

/** 
 * Message mechanism is borrowed from MINIX
 */
struct mess1
{
	 int m1i1;
	 int m1i2;
	 int m1i3;
	 int m1i4;
};

struct mess2
{
	 void* m2p1;
	 void* m2p2;
	 void* m2p3;
	 void* m2p4;
};

struct mess3
{
	 int	m3i1;
	 int	m3i2;
	 int	m3i3;
	 int	m3i4;
	 u64	m3l1;
	 u64	m3l2;
	 void*	m3p1;
	 void*	m3p2;
};

typedef struct
{
	 int source;
	 MsgType type;
	 union
	 {
		  struct mess1 m1;
		  struct mess2 m2;
		  struct mess3 m3;
	 }u;
}Message;

#endif /* _ORANGES_TYPE_H_ */

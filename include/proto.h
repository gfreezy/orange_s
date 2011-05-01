
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef _ORANGES_PROTO_H_
#define _ORANGES_PROTO_H_

#include "type.h"
#include "proc.h"
#include "tty.h"
#include "console.h"

/* kliba.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);
PUBLIC char * itoa(char * str, int num);
PUBLIC void disp_int(int input);

/* kernel.asm */
PUBLIC void sys_call();
PUBLIC void restart();

/* main.c */
PUBLIC void TestA();
PUBLIC void TestB();
PUBLIC void TestC();
PUBLIC int get_ticks();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void init_clock();
PUBLIC void clock_handler(int irq);

/* proc.c */
PUBLIC void schedule();
PUBLIC int sys_sendrec(int function, int src_dest, Message *p_m, Process *p_proc);
PUBLIC void * va2la(int pid, void *va);
PUBLIC int ldt_seg_linear(Process *p, int idx);
PUBLIC void reset_msg(Message *p);
PUBLIC int send_recv(int function, int src_dest, Message *msg);
PUBLIC void printx(char *s);
PUBLIC int sendrec(int function, int src_dest, Message* p_msg);

/* keyboard.c */
PUBLIC void init_keyboard();
PUBLIC void keyboard_read(TTY* p_tty);

/* tty.c */
PUBLIC void task_tty();
PUBLIC void in_process(TTY* p_tty, u32 key);
PUBLIC int sys_printx(int _unused1, int _unused2, char *s, Process *p_proc);

/* console.c */
PUBLIC void out_char(Console* p_con, char ch);
PUBLIC int is_current_console(Console* p_con);
PUBLIC void init_screen(TTY* p_tty);
PUBLIC void select_console(int nr_console);
PUBLIC void scroll_screen(Console* p_con, int direction);

/* printf.c */
PUBLIC int printf(const char *fmt, ...);
#define printl printf

/* vsprintf.c */
PUBLIC int vsprintf(char *buf, const char *fmt, va_list args);

/* misc.c */
PUBLIC void spin(char *func_name);

/* systask.c */
PUBLIC void task_sys();
#endif

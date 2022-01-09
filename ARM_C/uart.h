#define GPA0CON 		( *((volatile unsigned int *)0xE0200000) )

// UARTÏà¹Ø¼Ä´æÆ÷
#define ULCON0 			( *((volatile unsigned int *)0xE2900000) )
#define UCON0 			( *((volatile unsigned int *)0xE2900004) )
#define UFCON0 			( *((volatile unsigned int *)0xE2900008) )
#define UMCON0 			( *((volatile unsigned int *)0xE290000C) )
#define UTRSTAT0 		( *((volatile unsigned int *)0xE2900010) )
#define UERSTAT0 		( *((volatile unsigned int *)0xE2900014) )
#define UFSTAT0 		( *((volatile unsigned int *)0xE2900018) )
#define UMSTAT0 		( *((volatile unsigned int *)0xE290001C) )
#define UTXH0 			( *((volatile unsigned int *)0xE2900020) )
#define URXH0 			( *((volatile unsigned int *)0xE2900024) )
#define UBRDIV0 		( *((volatile unsigned int *)0xE2900028) )
#define UDIVSLOT0 		( *((volatile unsigned int *)0xE290002C) )
#define UINTP 			( *((volatile unsigned int *)0xE2900030) )
#define UINTSP 			( *((volatile unsigned int *)0xE2900034) )
#define UINTM 			( *((volatile unsigned int *)0xE2900038) )

#define UART_UBRDIV_VAL		35
#define UART_UDIVSLOT_VAL	0x1

typedef volatile char           s8;
typedef volatile short          s16;
typedef volatile int            s32;

typedef volatile unsigned char  u8;
typedef volatile unsigned short u16;
typedef volatile unsigned int   u32;

typedef char * va_list;
#define _INTSIZEOF(n)   ((sizeof(n)+sizeof(int)-1)&~(sizeof(int) - 1) )
#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)    ( ap = (va_list)0 )

void putchar(u8 c);
void puts(s8 *s);
void put_int(u32 v);
void put_hex(u8 v, u8 small);
void put_int_hex(u32 v, u8 small);
int printf(const char *fmt, ...);



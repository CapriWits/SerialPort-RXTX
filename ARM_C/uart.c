#include "uart.h"
// 初始化串口
void uart_init()
{
		// 1 配置引脚用于RX/TX功能
		GPA0CON &= ~0xFF;
		GPA0CON |= 0x22;
		// 2 设置数据格式等
		// 禁止fifo
		UFCON0 = 0;
		// 无流控
		UMCON0 = 0x0;
		// 数据位:8, 无校验, 停止位: 1
		ULCON0 = 0x3;
		// 时钟：PCLK，禁止中断，使能UART发送、接收
		UCON0  = 0x5;

		// 3 设置波特率
	 	 /*
	    ** 波特率计算：115200bps
	    ** PCLK_PSYS = 66MHz
	    ** DIV_VAL = (66000000/(115200 x 16))-1 = 35.8 - 1 = 34.8
	    ** UBRDIV0 = 34 (DIV_VAL的整数部分)
	    ** (num of 1's in UDIVSLOTn)/16 = 0.8
	    ** (num of 1's in UDIVSLOTn) = 12
	    ** UDIVSLOT0 = 0xDDDD (查表芯片手册880)
	    */
	    UBRDIV0 = 34;
	    UDIVSLOT0 = 0xDDDD;
}
/* 输出一字节的数据到终端 */
void uart_send_byte(u8 byte)
{
    while (!(UTRSTAT0 & (1 << 2)));   /* 等待发送缓冲区为空 */
    UTXH0 = byte;                   /* 发送一字节数据 */
}

/* 从终端接收一字节数据 */
u8 uart_recv_byte()
{
    while (!(UTRSTAT0 & 1));    /* 等待接收缓冲区有数据可读 */
    return URXH0;               /* 接收一字节数据 */
}

/* 打印字符c到终端 */
void putchar(u8 c)
{
    uart_send_byte(c);
    if (c == '\n')
        uart_send_byte('\r');
}

/* 打印字符串s到终端 */
void puts(s8 *s)
{
    s8 *p = s;
    while (*p)
        putchar(*p++);
}

/* 打印整数v到终端 */
void put_int(u32 v)
{
    int i;
    u8 a[10];
    u8 cnt = 0;

    if (v == 0)
    {
        putchar('0');
        return;
    }

    while (v)
    {
        a[cnt++] = v % 10;
        v /= 10;
    }

    for (i = cnt - 1; i >= 0; i--)
        putchar(a[i] + 0x30);
    /*
    ** 整数0-9的ASCII分别为0x30-0x39
    */
}

/* 将一字节十六进制数按十六进制显示打印到终端 */
void put_hex(u8 v, u8 small)
{
    /* 注意：必须用volatile修饰，否则会出错 */
    u8 h, l;        /* 高4位和第4位(这里按二进制算) */
    char *hex1 = "0123456789abcdef";        /* 这里放在数据段即data段中 */
    char *hex2 = "0123456789ABCDEF";

    h = v >> 4;
    l = v & 0x0F;

    if (small)  /* 小写 */
    {
        putchar(hex1[h]);   /* 高4位 */
        putchar(hex1[l]);   /* 低4位 */
    }
    else        /* 大写 */
    {
        putchar(hex2[h]);   /* 高4位 */
        putchar(hex2[l]);   /* 低4位 */
    }
}

/* 将int型整数按16进制打印到终端 */
void put_int_hex(u32 v, u8 small)
{
    if (v >> 24)
    {
        put_hex(v >> 24, small);
        put_hex((v >> 16) & 0xFF, small);
        put_hex((v >> 8) & 0xFF, small);
        put_hex(v & 0xFF, small);
    }
    else if ((v >> 16) & 0xFF)
    {
        put_hex((v >> 16) & 0xFF, small);
        put_hex((v >> 8) & 0xFF, small);
        put_hex(v & 0xFF, small);
    }
    else if ((v >> 8) & 0xFF)
    {
        put_hex((v >> 8) & 0xFF, small);
        put_hex(v & 0xFF, small);
    }
    else
        put_hex(v & 0xFF, small);
}
int printf(const char *fmt, ...)
{
    va_list ap;
    s8 c;
    s8 *s;
    u32 d;
    u8 small;

    va_start(ap, fmt);
    while (*fmt)
    {
        small = 0;
        c = *fmt++;
        if (c == '%')
        {
            switch (*fmt++)
            {
            case 'c':              /* char */
                c = (char) va_arg(ap, int);
                putchar(c);
                break;
            case 's':              /* string */
                s = va_arg(ap, char *);
                puts(s);
                break;
            case 'd':              /* int */
            case 'u':
                d = va_arg(ap, int);
                put_int(d);
                break;
            case 'x':
                small = 1;  // small
            case 'X':
                d = va_arg(ap, int);
                put_int_hex(d, small);
                break;
            }
        }
        else
            putchar(c);
    }
    va_end(ap);
}

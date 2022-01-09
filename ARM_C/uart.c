#include "uart.h"
// ��ʼ������
void uart_init()
{
		// 1 ������������RX/TX����
		GPA0CON &= ~0xFF;
		GPA0CON |= 0x22;
		// 2 �������ݸ�ʽ��
		// ��ֹfifo
		UFCON0 = 0;
		// ������
		UMCON0 = 0x0;
		// ����λ:8, ��У��, ֹͣλ: 1
		ULCON0 = 0x3;
		// ʱ�ӣ�PCLK����ֹ�жϣ�ʹ��UART���͡�����
		UCON0  = 0x5;

		// 3 ���ò�����
	 	 /*
	    ** �����ʼ��㣺115200bps
	    ** PCLK_PSYS = 66MHz
	    ** DIV_VAL = (66000000/(115200 x 16))-1 = 35.8 - 1 = 34.8
	    ** UBRDIV0 = 34 (DIV_VAL����������)
	    ** (num of 1's in UDIVSLOTn)/16 = 0.8
	    ** (num of 1's in UDIVSLOTn) = 12
	    ** UDIVSLOT0 = 0xDDDD (���оƬ�ֲ�880)
	    */
	    UBRDIV0 = 34;
	    UDIVSLOT0 = 0xDDDD;
}
/* ���һ�ֽڵ����ݵ��ն� */
void uart_send_byte(u8 byte)
{
    while (!(UTRSTAT0 & (1 << 2)));   /* �ȴ����ͻ�����Ϊ�� */
    UTXH0 = byte;                   /* ����һ�ֽ����� */
}

/* ���ն˽���һ�ֽ����� */
u8 uart_recv_byte()
{
    while (!(UTRSTAT0 & 1));    /* �ȴ����ջ����������ݿɶ� */
    return URXH0;               /* ����һ�ֽ����� */
}

/* ��ӡ�ַ�c���ն� */
void putchar(u8 c)
{
    uart_send_byte(c);
    if (c == '\n')
        uart_send_byte('\r');
}

/* ��ӡ�ַ���s���ն� */
void puts(s8 *s)
{
    s8 *p = s;
    while (*p)
        putchar(*p++);
}

/* ��ӡ����v���ն� */
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
    ** ����0-9��ASCII�ֱ�Ϊ0x30-0x39
    */
}

/* ��һ�ֽ�ʮ����������ʮ��������ʾ��ӡ���ն� */
void put_hex(u8 v, u8 small)
{
    /* ע�⣺������volatile���Σ��������� */
    u8 h, l;        /* ��4λ�͵�4λ(���ﰴ��������) */
    char *hex1 = "0123456789abcdef";        /* ����������ݶμ�data���� */
    char *hex2 = "0123456789ABCDEF";

    h = v >> 4;
    l = v & 0x0F;

    if (small)  /* Сд */
    {
        putchar(hex1[h]);   /* ��4λ */
        putchar(hex1[l]);   /* ��4λ */
    }
    else        /* ��д */
    {
        putchar(hex2[h]);   /* ��4λ */
        putchar(hex2[l]);   /* ��4λ */
    }
}

/* ��int��������16���ƴ�ӡ���ն� */
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

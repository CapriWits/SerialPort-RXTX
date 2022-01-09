

#define CLk_SRC0 	0xE0100200
#define APLL_LOCK	0xE0100000
#define MPLL_LOCK 	0xE0100008
#define CLK_DIV0	0xE0100300
#define APLL_CON0	0xE0100100
#define MPLL_CON	0xE0100108

#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)

.global clock_init

clock_init:
		/*1 ���ȹر�PLL��ѡ���ⲿ��ʱ��*/
		mov r0, #0
		ldr r1, =CLk_SRC0
		str r0, [r1]

		/*2 ��������ʱ�䣬ʹ��Ĭ��ֵ����
		 ����PLL��ʱ�Ӵ�Fin������Ŀ��Ƶ��ʱ��
		 ��Ҫһ����ʱ�䣬������ʱ��,оƬ�ֲ�P371*/
		ldr	r0,	=0x0000FFFF
		ldr r1, =APLL_LOCK
		ldr r2, =MPLL_LOCK
		str	r0,	[r1]
		str r0, [r2]

		/*3 ����PLL*/
		/* FOUT = MDIV*FIN/(PDIV*2^(SDIV-1))=0x7d*24/(0x3*2^(1-1))=1000 MHz
		оƬ�ֲ�P357,P372*/
		ldr r0, =APLL_CON0
		ldr	r1, =set_pll(0x7d, 0x3, 0x1)
		str	r1, [r0]
		// FOUT = MDIV*FIN/(PDIV*2^SDIV)=0x29b*24/(0xc*2^1)= 667 MHz
		ldr r0, =MPLL_CON
		ldr	r1, =set_pll(0x29b, 0xc, 0x1)
		str	r1, [r0]
		/*4 ���÷�Ƶ
		 0x14131440����������ο�ʵ��ָ�����еĽ��ⲿ��*/
		ldr r0, =CLK_DIV0
		ldr	r1, =0x14131440
		str	r1, [r0]
		/*5 ���ø���ʱ�ӿ���,ʹ��PLL*/
		ldr r0, =CLk_SRC0
		ldr	r1, =0x1111
		str	r1, [r0]

		mov pc,lr  //����




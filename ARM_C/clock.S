

#define CLk_SRC0 	0xE0100200
#define APLL_LOCK	0xE0100000
#define MPLL_LOCK 	0xE0100008
#define CLK_DIV0	0xE0100300
#define APLL_CON0	0xE0100100
#define MPLL_CON	0xE0100108

#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)

.global clock_init

clock_init:
		/*1 首先关闭PLL，选择外部的时钟*/
		mov r0, #0
		ldr r1, =CLk_SRC0
		str r0, [r1]

		/*2 设置锁定时间，使用默认值即可
		 设置PLL后，时钟从Fin提升到目标频率时，
		 需要一定的时间，即锁定时间,芯片手册P371*/
		ldr	r0,	=0x0000FFFF
		ldr r1, =APLL_LOCK
		ldr r2, =MPLL_LOCK
		str	r0,	[r1]
		str r0, [r2]

		/*3 设置PLL*/
		/* FOUT = MDIV*FIN/(PDIV*2^(SDIV-1))=0x7d*24/(0x3*2^(1-1))=1000 MHz
		芯片手册P357,P372*/
		ldr r0, =APLL_CON0
		ldr	r1, =set_pll(0x7d, 0x3, 0x1)
		str	r1, [r0]
		// FOUT = MDIV*FIN/(PDIV*2^SDIV)=0x29b*24/(0xc*2^1)= 667 MHz
		ldr r0, =MPLL_CON
		ldr	r1, =set_pll(0x29b, 0xc, 0x1)
		str	r1, [r0]
		/*4 设置分频
		 0x14131440的由来，请参考实验指导书中的讲解部分*/
		ldr r0, =CLK_DIV0
		ldr	r1, =0x14131440
		str	r1, [r0]
		/*5 设置各种时钟开关,使用PLL*/
		ldr r0, =CLk_SRC0
		ldr	r1, =0x1111
		str	r1, [r0]

		mov pc,lr  //返回





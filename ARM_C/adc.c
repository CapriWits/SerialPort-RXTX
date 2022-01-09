#include "uart.h"
#define  ADCTS_BASE		0xE1700000
#define  TSADCCON0		( *((volatile unsigned int *)(ADCTS_BASE+0x0)) )
#define  TSCON0			( *((volatile unsigned int *)(ADCTS_BASE+0x4)) )
#define  TSDLY0			( *((volatile unsigned int *)(ADCTS_BASE+0x8)) )
#define  TSDATX0		( *((volatile unsigned int *)(ADCTS_BASE+0xc)) )
#define  TSDATY0		( *((volatile unsigned int *)(ADCTS_BASE+0x10)) )
#define  TSPENSTAT0		( *((volatile unsigned int *)(ADCTS_BASE+0x14)) )
#define  CLRINTADC0		( *((volatile unsigned int *)(ADCTS_BASE+0x18)) )
#define  ADCMUX			( *((volatile unsigned int *)(ADCTS_BASE+0x1c)) )
#define	 CLRINTPEN0		( *((volatile unsigned int *)(ADCTS_BASE+0x20)) )

/*蜂鸣器相关寄存器地址*/
#define GPD0CON			(*(volatile unsigned int *)0xE02000A0)
#define GPD0DAT			(*(volatile unsigned int *)0xE02000A4)
/*按键相关的寄存器地址*/
#define GPJ1CON      	(*(volatile unsigned int *)0xE0200260)
#define GPJ1DAT      	(*(volatile unsigned int *)0xE0200264)
#define GPJ1PUD      	(*(volatile unsigned int *)0xE0200268)
#define GPJ4CON      	(*(volatile unsigned int *)0xE02002C0)
#define GPJ4DAT     	(*(volatile unsigned int *)0xE02002C4)
/*LED1相关的寄存器地址*/
#define GPJ2CON     	(*(volatile unsigned int *)0xE0200280)
#define GPJ2DAT      	(*(volatile unsigned int *)0xE0200284)
/*中断相关寄存器地址*/
#define GPJ1_INT_CON 	(*(volatile unsigned int *)0xE0200748)
#define GPJ1_INT_MASK	(*(volatile unsigned int *)0xE0200948)
#define GPJ1_INT_PEND   (*(volatile unsigned int *)0xE0200A48)
#define VIC0INTENABLE	(*(volatile unsigned int *)0xF2000010)
#define VIC0INTSELECT   (*(volatile unsigned int *)0xF200000C)
#define VIC0VECTADDR30	(*(volatile unsigned int *)0xF2000178)
#define VIC0ADDRESS		(*(volatile unsigned int *)0xF2000F00)
#define VIC1ADDRESS		(*(volatile unsigned int *)0xF2100F00)
#define VIC2ADDRESS		(*(volatile unsigned int *)0xF2200F00)
#define VIC3ADDRESS		(*(volatile unsigned int *)0xF2300F00)

extern unsigned int IRQ_handle;
static unsigned int i = 1;
static unsigned int counter = 0;

// 初始化蜂鸣器
void beep_init()
{
	// 清空低四位 
	GPD0CON = GPD0CON & ~(0xf);
	// GPD0CON 配置为 0001 输出模式，高电平蜂鸣器
	GPD0CON = GPD0CON | 1;
	// 初始状态蜂鸣器不响，DAT对应位置 0
	GPD0DAT = GPD0DAT & 0xfe;
}

// LED 灯初始化，设置 7 号灯 
void led_init()
{
	// 消除 7 号灯控制引脚，对应位设置为 0000 
	GPJ2CON = GPJ2CON & 0xf0ffffff;
	// 7 号灯设置为输出模式，为 0001 
	GPJ2CON = GPJ2CON | 0x01000000;
	// 熄灭 LED，对应位置 0 
	GPJ2DAT = GPJ2DAT & 0xbf;
}

// 7 号按钮对应 行管脚GPJ1_1 列管脚GPJ4_1
void key_init()
{
	// GPJ1_1 = 1111B, 配置为中断功能
	GPJ1CON = GPJ1CON & 0xffffff0f;
	GPJ1CON = GPJ1CON | (0xF << 4 * 1);
	// GPJ1_1,下拉使能
	GPJ1PUD = GPJ1PUD & 0xfff3;
	GPJ1PUD = GPJ1PUD | 0x4;
	// 设置CPJ4_1设置为输出模式
	GPJ4CON = GPJ4CON & 0xffffff0f;
	GPJ4CON = GPJ4CON | 0x10;
	GPJ4DAT = GPJ4DAT | 0x2;
}

// 清空中断地址寄存器中内容
void clear_Vicaddress()
{
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
	VIC2ADDRESS = 0;
	VIC3ADDRESS = 0;
}

// 中断初始化 
void interrupt_init()
{
	// 改为第4个按键有用，行管脚GPJ1_1 列管脚GPJ4_3
	/*上升沿触发，芯片手册251*/
	GPJ1_INT_CON = GPJ1_INT_CON & 0xffffff0f;
	GPJ1_INT_CON = GPJ1_INT_CON | (0x3<<4*1);

	/*取消中断屏蔽位，芯片手册297*/
	GPJ1_INT_MASK = GPJ1_INT_MASK & 0xfd;

	/*清除中断挂起位*/
	GPJ1_INT_PEND = GPJ1_INT_PEND & (1<<1);

	/*设置为IRQ*/
	VIC0INTSELECT = 0;

	/*安装中断服务函数*/
	VIC0VECTADDR30 = &IRQ_handle;

	/*清中断*/
	VIC0ADDRESS = 0;

	/*使能中断，芯片手册562*/
	VIC0INTENABLE = VIC0INTENABLE | (0x1<<30);
}

// 延时 
void delay(unsigned int n)
{
	for(;n>0;n--);
}

// ad 转换 
int adChangerHandler() {
	unsigned int result;
	// 设置位 0 位置为 1，启动 A/D 转换
	TSADCCON0 |= (1 << 0);
	// 当 A/D 转换开始时，0 位置自动清 0
	while (TSADCCON0 & (1 << 0));
	// 检测位 15 位置值为 1 时表示转换结束,不为 1 空转 
	while (!(TSADCCON0 & (1 << 15)));
	// 读数据
	result = TSDATX0 & 0xfff;
	printf("adc = #%d#\n",result);
	return result;
}

// IRQ中断处理程序
void irqhandler()
{
	// ad转换结果
	unsigned int result;
	
	if((GPJ1DAT & 0x2) == 0x2)
	{
		// 延时消抖 
		delay(0x20000);
		if((GPJ1DAT & 0x2) == 0x2)
		{
			counter++;
			result = adChangerHandler();  // ad 转换 
			if(counter >= 5 && result < 2400) {
				printf("Over temperature...");
				while (1) {
					// 蜂鸣器响、灯亮
					GPD0DAT |= 1 << 0;
					GPJ2DAT |= 1 << 6;
					delay(0x50000);
					// 蜂鸣器响、灯熄灭 
					GPD0DAT &= ~(1 << 0);
					GPJ2DAT &= ~(1 << 6);
					delay(0x50000);
					if((GPJ1DAT & 0x2) == 0x2){
						delay(0x20000);
						if((GPJ1DAT & 0x2) == 0x2){
							printf("The alarm has been disarmed...");
							printf("\n");
							// 关闭蜂鸣器和 LED 灯
							GPD0DAT = 0;
							GPJ2DAT = 0;
							break;
						}
					}
				}
			}
		}
	}
	/*清除中断挂起位*/
	GPJ1_INT_PEND = GPJ1_INT_PEND & (1 << 1);
	clear_Vicaddress();
}
// ADC初始化
void adc_init()
{
	// 使能预分频功能，设置A/D转换器的时钟 = PCLK/(65+1)
	TSADCCON0 &= ~((1<<1)|(1<<16)|(1 << 14) | (0xff<< 6));
	TSADCCON0 |= (1<<16)|(1 << 14) | (65 << 6);
	// 清除位[2]，设为普通转换模式
	TSADCCON0 &= ~(1<<2);
	// 选择通道
	ADCMUX = 0;
}

void main()
{
	// 按键次数清零
	counter = 0;
	// LED灯熄灭
	GPJ2DAT = 0;
	// 蜂鸣器关闭 
	GPD0DAT = 0;
	led_init();
	key_init();
	interrupt_init();
	adc_init();
	delay(0x100000);
	int i;
	while(1) {i = 1;}
}

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

/*��������ؼĴ�����ַ*/
#define GPD0CON			(*(volatile unsigned int *)0xE02000A0)
#define GPD0DAT			(*(volatile unsigned int *)0xE02000A4)
/*������صļĴ�����ַ*/
#define GPJ1CON      	(*(volatile unsigned int *)0xE0200260)
#define GPJ1DAT      	(*(volatile unsigned int *)0xE0200264)
#define GPJ1PUD      	(*(volatile unsigned int *)0xE0200268)
#define GPJ4CON      	(*(volatile unsigned int *)0xE02002C0)
#define GPJ4DAT     	(*(volatile unsigned int *)0xE02002C4)
/*LED1��صļĴ�����ַ*/
#define GPJ2CON     	(*(volatile unsigned int *)0xE0200280)
#define GPJ2DAT      	(*(volatile unsigned int *)0xE0200284)
/*�ж���ؼĴ�����ַ*/
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

// ��ʼ��������
void beep_init()
{
	// ��յ���λ 
	GPD0CON = GPD0CON & ~(0xf);
	// GPD0CON ����Ϊ 0001 ���ģʽ���ߵ�ƽ������
	GPD0CON = GPD0CON | 1;
	// ��ʼ״̬���������죬DAT��Ӧλ�� 0
	GPD0DAT = GPD0DAT & 0xfe;
}

// LED �Ƴ�ʼ�������� 7 �ŵ� 
void led_init()
{
	// ���� 7 �ŵƿ������ţ���Ӧλ����Ϊ 0000 
	GPJ2CON = GPJ2CON & 0xf0ffffff;
	// 7 �ŵ�����Ϊ���ģʽ��Ϊ 0001 
	GPJ2CON = GPJ2CON | 0x01000000;
	// Ϩ�� LED����Ӧλ�� 0 
	GPJ2DAT = GPJ2DAT & 0xbf;
}

// 7 �Ű�ť��Ӧ �йܽ�GPJ1_1 �йܽ�GPJ4_1
void key_init()
{
	// GPJ1_1 = 1111B, ����Ϊ�жϹ���
	GPJ1CON = GPJ1CON & 0xffffff0f;
	GPJ1CON = GPJ1CON | (0xF << 4 * 1);
	// GPJ1_1,����ʹ��
	GPJ1PUD = GPJ1PUD & 0xfff3;
	GPJ1PUD = GPJ1PUD | 0x4;
	// ����CPJ4_1����Ϊ���ģʽ
	GPJ4CON = GPJ4CON & 0xffffff0f;
	GPJ4CON = GPJ4CON | 0x10;
	GPJ4DAT = GPJ4DAT | 0x2;
}

// ����жϵ�ַ�Ĵ���������
void clear_Vicaddress()
{
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
	VIC2ADDRESS = 0;
	VIC3ADDRESS = 0;
}

// �жϳ�ʼ�� 
void interrupt_init()
{
	// ��Ϊ��4���������ã��йܽ�GPJ1_1 �йܽ�GPJ4_3
	/*�����ش�����оƬ�ֲ�251*/
	GPJ1_INT_CON = GPJ1_INT_CON & 0xffffff0f;
	GPJ1_INT_CON = GPJ1_INT_CON | (0x3<<4*1);

	/*ȡ���ж�����λ��оƬ�ֲ�297*/
	GPJ1_INT_MASK = GPJ1_INT_MASK & 0xfd;

	/*����жϹ���λ*/
	GPJ1_INT_PEND = GPJ1_INT_PEND & (1<<1);

	/*����ΪIRQ*/
	VIC0INTSELECT = 0;

	/*��װ�жϷ�����*/
	VIC0VECTADDR30 = &IRQ_handle;

	/*���ж�*/
	VIC0ADDRESS = 0;

	/*ʹ���жϣ�оƬ�ֲ�562*/
	VIC0INTENABLE = VIC0INTENABLE | (0x1<<30);
}

// ��ʱ 
void delay(unsigned int n)
{
	for(;n>0;n--);
}

// ad ת�� 
int adChangerHandler() {
	unsigned int result;
	// ����λ 0 λ��Ϊ 1������ A/D ת��
	TSADCCON0 |= (1 << 0);
	// �� A/D ת����ʼʱ��0 λ���Զ��� 0
	while (TSADCCON0 & (1 << 0));
	// ���λ 15 λ��ֵΪ 1 ʱ��ʾת������,��Ϊ 1 ��ת 
	while (!(TSADCCON0 & (1 << 15)));
	// ������
	result = TSDATX0 & 0xfff;
	printf("adc = #%d#\n",result);
	return result;
}

// IRQ�жϴ������
void irqhandler()
{
	// adת�����
	unsigned int result;
	
	if((GPJ1DAT & 0x2) == 0x2)
	{
		// ��ʱ���� 
		delay(0x20000);
		if((GPJ1DAT & 0x2) == 0x2)
		{
			counter++;
			result = adChangerHandler();  // ad ת�� 
			if(counter >= 5 && result < 2400) {
				printf("Over temperature...");
				while (1) {
					// �������졢����
					GPD0DAT |= 1 << 0;
					GPJ2DAT |= 1 << 6;
					delay(0x50000);
					// �������졢��Ϩ�� 
					GPD0DAT &= ~(1 << 0);
					GPJ2DAT &= ~(1 << 6);
					delay(0x50000);
					if((GPJ1DAT & 0x2) == 0x2){
						delay(0x20000);
						if((GPJ1DAT & 0x2) == 0x2){
							printf("The alarm has been disarmed...");
							printf("\n");
							// �رշ������� LED ��
							GPD0DAT = 0;
							GPJ2DAT = 0;
							break;
						}
					}
				}
			}
		}
	}
	/*����жϹ���λ*/
	GPJ1_INT_PEND = GPJ1_INT_PEND & (1 << 1);
	clear_Vicaddress();
}
// ADC��ʼ��
void adc_init()
{
	// ʹ��Ԥ��Ƶ���ܣ�����A/Dת������ʱ�� = PCLK/(65+1)
	TSADCCON0 &= ~((1<<1)|(1<<16)|(1 << 14) | (0xff<< 6));
	TSADCCON0 |= (1<<16)|(1 << 14) | (65 << 6);
	// ���λ[2]����Ϊ��ͨת��ģʽ
	TSADCCON0 &= ~(1<<2);
	// ѡ��ͨ��
	ADCMUX = 0;
}

void main()
{
	// ������������
	counter = 0;
	// LED��Ϩ��
	GPJ2DAT = 0;
	// �������ر� 
	GPD0DAT = 0;
	led_init();
	key_init();
	interrupt_init();
	adc_init();
	delay(0x100000);
	int i;
	while(1) {i = 1;}
}

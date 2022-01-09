.text
.global _start
.global IRQ_handle
.global EnableVIC
_start:
	    ldr sp, =0xD0037D80
		bl EnableIRQ				//��ȫ��IRQ�жϿ���
		bl EnableVIC
		bl clock_init				//�������ǹٷ��Ƽ���ʱ��ֵ��оƬ�ֲ�356ҳ����ʼ��ϵͳʱ��
		bl uart_init
		bl delay
		bl main
halt:
		b halt							//"."������ǰ��ַ
IRQ_handle:

		ldr sp, =0xD0037F80			// �����ж�ģʽ��ջ

		sub lr, lr, #4				// �����ֳ�

		stmfd sp!, {r0-r12, lr}

		bl	irqhandler				// ��ת���жϴ�������

		ldmfd sp!, {r0-r12, pc}^	// �ָ��ֳ�
EnableIRQ:
        mrs r0,cpsr  		 		//���cpsr�Ĵ�����ֵ������r0����

        bic r0,r0,#(1<<7)  			//��r0�ĵ�7λ����

        msr cpsr_cxsf,r0   			//��������ֵ���·Ż�cpsr����

        mov pc,lr        			//lrλr14�������ú���������һ����
EnableVIC:
	    mrc  p15,0,r0,c1,c0,0

	    orr  r0,r0,#(1<<24)

	    mcr  p15,0,r0,c1,c0,0

	    mov  pc,lr

delay:
		mov r0, #0x100000
delay_loop:
		cmp r0, #0
		sub r0, r0, #1
		bne delay_loop
		mov pc, lr
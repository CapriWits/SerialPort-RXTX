.text
.global _start
.global IRQ_handle
.global EnableVIC
_start:
	    ldr sp, =0xD0037D80
		bl EnableIRQ				//打开全局IRQ中断开关
		bl EnableVIC
		bl clock_init				//按照三星官方推荐的时钟值（芯片手册356页）初始化系统时钟
		bl uart_init
		bl delay
		bl main
halt:
		b halt							//"."代表当前地址
IRQ_handle:

		ldr sp, =0xD0037F80			// 设置中断模式的栈

		sub lr, lr, #4				// 保存现场

		stmfd sp!, {r0-r12, lr}

		bl	irqhandler				// 跳转到中断处理函数

		ldmfd sp!, {r0-r12, pc}^	// 恢复现场
EnableIRQ:
        mrs r0,cpsr  		 		//获得cpsr寄存器的值，放入r0当中

        bic r0,r0,#(1<<7)  			//将r0的第7位清零

        msr cpsr_cxsf,r0   			//将清零后的值重新放回cpsr当中

        mov pc,lr        			//lr位r14，即调用函数语句的下一条语
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

all: start.o clock.o uart.o adc.o
	arm-none-eabi-ld  -Tadc.lds -o adc.elf $^
	arm-none-eabi-objcopy -O binary adc.elf adc.bin
	arm-none-eabi-objdump -D adc.elf > adc_elf.dis
%.o : %.S
	arm-none-eabi-gcc -g -o $@ $< -c
%.o : %.c
	arm-none-eabi-gcc -g -o $@ $< -c
clean:
	rm *.o *.elf *.bin *.dis  -f
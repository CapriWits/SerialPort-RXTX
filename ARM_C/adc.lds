ENTRY(_start)
SECTIONS {
	. = 0xd0020000;
	. = ALIGN(4);
	.text :
	{
	start.o (.text)
	*(.text)
	}
	.data :
	{
	*(.data)
	}
	.bss :
	{
	*(.bss)
	}
}
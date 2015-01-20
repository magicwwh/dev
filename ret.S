	xchg (%esp), %eax
	cmp %gs:0x0, %eax
	jb L
	and 0x0F, %al
	cmp %gs:0x04, %eax
	jae l
	xchg (%esp), %eax
	ret
L:
	jmp 0xF4
#include <stdio.h>
#include <unistd.h>

void help()
{
	asm ("pop %rax");
	asm ("pop %rdx");
	asm ("pop %rdi");
	asm ("pop %rsi");
	asm ("ret");
}

int main()
{
	char buf[12];

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);

	read(0, buf, 120);
	
	return 0;
}

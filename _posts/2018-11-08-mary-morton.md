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

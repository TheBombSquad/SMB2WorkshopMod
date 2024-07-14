
#function $bagel 0xa32cc
b .two
add r7, r6, r5
.two
mullw r7,r6,r5
sub r7,r6,r5
and r7,r6,r5
or r7,r6,r5
xor r7,r6,r5
nand r7,r6,r5
nor r7,r6,r5
mr r7,r6
not r7,r6
slw r7,r6,r5
sraw r7, r6, r5 % Shift right algebraic
srw r7,r6, r5
extsb r7, r6
extsh r7, r6
bl $debugfunc
cmpw r7,r6
beq .two
cmpwi r7,0x0708
bgt .two
cmplwi r7,0x0708
ble .two
lis r3,0x805C
subi r3,r3,0x3b90
addi r3,r1,0x0001
mulli r3,r1,0x0001
andi r3,r1,0x0001
ori r3,r1,0x0001
andis r3,r1,0x0002
oris r3,r1,0x0003
xoris r3,r1,0x0004
nop
rlwinm r3,r3,2,0,29
lbz r7, 0 (r6)
lhz r7, 0x0004 (r6)
lha r7, 0x0008 (r6)
lwz r7, 0x000C (r6)
stb r7, 0x0011 (r6)
sth r7, 0x001e ( r6 )
stw r7, -0x0020 (r6)
lwzx r7, r6, r5
lfs f7, 0x0012 (r5)
lfd f7, 0x0016 (r5)
stfs f7, 0x0016 (r5)
fmr f7, f6
mflr r0
mtlr r0
bl @fn808f8d9c
bc 16, 0, .two
nop
add r7, r6, r5
addo r7, r6, r5
add. r7, r6, r5
addo. r7, r6, r5
nop
blr

#offset @fn808f8d9c 0x688c9c

#function $debugfunc 0xa4208
nop
nop
nop


% #function $betta 0xa3978

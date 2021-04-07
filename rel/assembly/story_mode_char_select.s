.global get_monkey_id_hook

get_monkey_id_hook:
lis r9, 0x805b
ori r9, r9, 0xd7bb
lbz r3, 0(r9)
blr

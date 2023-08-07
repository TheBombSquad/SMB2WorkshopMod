.global get_monkey_id_hook
.extern active_monkey_id

get_monkey_id_hook:
lis r9, active_monkey_id@h
ori r9, r9, active_monkey_id@l
lwz r3, 0(r9)
blr

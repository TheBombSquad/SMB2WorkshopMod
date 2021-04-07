.global get_theme_id_hook_1, get_theme_id_hook_2
.extern theme_id_lookup, g_current_stage_id, g_next_stage_id


get_theme_id_hook_1:
    lis r0, theme_id_lookup@h
    ori r0, r0, theme_id_lookup@l
    mulli r3, r3, 0x2
    add r3, r0, r3
    addi r3, r3, 1
    lbz r3, 0(r3)
    blr

get_theme_id_hook_2:
    lis r0, theme_id_lookup@h
    ori r0, r0, theme_id_lookup@l
    mulli r3, r3, 0x2
    add r3, r0, r3
    addi r3, r3, 1
    lbz r0, 0(r3)
LAB_801f1a98:
    or r3, r0, r0
    extsh r0, r6
    lis r4, g_next_stage_id@h
    addi r4, r4, g_next_stage_id@l
    sth r0, -0xc(r4)
    blr

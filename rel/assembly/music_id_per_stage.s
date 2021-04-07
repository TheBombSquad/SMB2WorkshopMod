.global get_bgm_id_hook
.extern bgm_id_lookup, g_current_stage_id

get_bgm_id_hook:
    lis r0, bgm_id_lookup@h
    ori r0, r0, bgm_id_lookup@l
    lis r3, g_current_stage_id@h
    ori r3, r3, g_current_stage_id@l
    lwz r3, 0(r3)
    mulli r3, r3, 0x2
    add r3, r0, r3
    lhz r0, 0(r3)
    blr

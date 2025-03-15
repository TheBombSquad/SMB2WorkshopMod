.global get_bgm_id_hook
.extern bgm_id_lookup, g_current_stage_id, g_replay_stage_id_to_load, sub_mode

get_bgm_id_hook:
    lis r4, sub_mode@h
    ori r4, r4, sub_mode@l
    lwz r4, 0(r4)       
    cmpwi r4, 258       
    bne default_path    

    lis r3, g_replay_stage_id_to_load@h
    ori r3, r3, g_replay_stage_id_to_load@l
    lwz r3, 0(r3)       
    b skip              

default_path:
    lis r3, g_current_stage_id@h
    ori r3, r3, g_current_stage_id@l
    lwz r3, 0(r3)

skip:
    lis r0, bgm_id_lookup@h
    ori r0, r0, bgm_id_lookup@l
    mulli r3, r3, 0x2
    add r3, r0, r3
    lhz r0, 0(r3)
    blr

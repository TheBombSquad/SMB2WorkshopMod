.global fix_rain_ripple
.extern sub_mode

fix_rain_ripple:
    lis r4, sub_mode@h
    ori r4, r4, sub_mode@l
    lwz r0, 0x0(r4)
    cmplwi r0, 66
    beq end
    stwu r1, -0x40(r1)
    lis r0, 0x802d
    ori r0, r0, 0xe1d8
    mtctr r0
    bctr

    end:
    blr

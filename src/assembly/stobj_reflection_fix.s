.global reflection_draw_stage_hook

reflection_draw_stage_hook:
lis r3, 0x8054                  # Checks the render flag at 0x8054
ori r3, r3, 0xdce0
lwz r3, 0(r3)
cmpwi r3, 0x4                   # A value of 0x4 means we're drawing a reflection
bne returnDrawLoop              # So if we're not drawing a reflection, draw it normally

lwz r3, 0(r29)                  # Load current model from the pointer list
lwz r0, 0(r3)                   # Loads the effect bitflag of the current model
rlwinm r0, r0, 0, 0x1d, 0x1d    # Checks if flag 0x4 (unknown 3) is set
cmplwi r0, 0
bne returnDrawLoop             # If it's not, don't draw it
lis r3, 0x802c
ori r3, r3, 0x9540
mtlr r3
blr                             # Continue with the next model (object is not drawn)

returnDrawLoop:
lwz r3, 0(r29)
blr                             # Continue as normal (object is drawn)

.global reflection_view_stage_hook

reflection_view_stage_hook:
lis r3, 0x8054                  # Checks the render flag at 0x8054
ori r3, r3, 0xdce0
lwz r3, 0(r3)
cmpwi r3, 0x6                   # A value of 0x6 means we're drawing a reflection
bne returnDrawGame              # So if we're not drawing a reflection, draw it normally

lwz r3, 0(r26)                  # Load current model from the pointer list
lwz r0, 0(r3)                   # Loads the effect bitflag of the current model
rlwinm r0, r0, 0, 0x1d, 0x1d    # Checks if flag 0x4 (unknown 3) is set
cmplwi r0, 0
bne returnDrawGame             # If it's not, don't draw it
lis r3, 0x8091
ori r3, r3, 0x3f5c
mtlr r3
blr
                                # Continue with the next model (object is not drawn)
returnDrawGame:
lwz r3, 0(r26)
blr                             # Continue as normal (object is drawn)

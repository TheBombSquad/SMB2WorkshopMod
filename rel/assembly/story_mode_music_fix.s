.global story_mode_music_hook

story_mode_music_hook:
addis r3, r2, 0x3a            // loads top half of main_mode address
ori r3, r3, 0xdc34            // loads bottom half of the main_mode address
lwz r3, 0(r3)                 // loads the value of main_mode
cmpwi r3, 0x30                // compares against values <0x30 (we aren't on a stage)
blt nomatch
cmpwi r3, 0x5b                // compares against 0x5b (story mode 'retry')
beq match
cmpwi r3, 0x3b                // compares against values >0x3b (we aren't on a stage)
bgt nomatch

match:
mflr r14                      // saves link register in r14
addis r3, r2, 0x10            // loads top half of address hurry up fade-out function
ori r3, r3, 0x5e94            // loads bottom half of address of hurry up fade-out function
mtlr r3                       // moves to lr
li r3, 0x64                   // loads parameters (volume?)
blrl                          // (blrl) calls the function to cancel out hurry up music
li r3, 0x1                    // loads looping state 1 (don't restart/change music)
mtlr r14                      // restores link register from r14
b end

nomatch:
li r3, 0x0                    // loads default looping state (restart/change music)

end:
blr

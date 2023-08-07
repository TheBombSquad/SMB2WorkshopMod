# Original code created by PistonMiner and Zephiles for Paper Mario: The Thousand-Year Door
# Code ported to Super Monkey Ball 2 by Zephiles

# 80006d08 - Address to hook for this code

# Set global function/variable/offset locations/values
.set ARENA_LOW_OFFSET,-0x7f50
.set memset,0x800033a8
.set LOADER_LOWMEM_LOCATION,0x8000496c
.set LOADER_HOOK_LOCATION,0x80006d08
.set read_entire_file_using_dvdread_prio_async,0x800092fc
.set OSAllocFromArenaLow,0x8000d5a8
.set DCFlushRange,0x8000d8cc
.set OSLink,0x80010730
.set OSUnlink,0x80010b8c
.set DVDOpen,0x800187a0
.set DVDClose,0x80018868

__start:
# Make sure mkb2.main_game.rel was the REL file that was just loaded
lwz r3,0(r31) # Pointer to the Main Loop's Module
lwz r3,0(r3) # Main Loop's Module Id
cmpwi r3,0x1
bne+ exit

# Initialize static values used for addresses
# Do not overwrite r31
lis r30,0x8000

li r29,0 # Used to check if a REL file is loaded or not

# Back up the current Arena Low
lwz r28,ARENA_LOW_OFFSET(r13)

# Allocate memory for DVDFileInfo
li r3,0x44
bl allocateFromArenaLow

# Backup the returned address to be used for later
mr r27,r3

# Open file on DVD
ori r3,r30,(LOADER_LOWMEM_LOCATION + filenameString)@l
mr r4,r27 # File info pointer (argument 2)
bl (DVDOpen - LOADER_LOWMEM_LOCATION + __start)
cmpwi r3,0
beq- exit

# Get the file size and adjust it to be in multiples of 0x20 bytes
lwz r26,0x34(r27)
addi r26,r26,31
rlwinm r26,r26,0,0,26

# Get the pointer to the Main Loop's Relocation Data location
lwz r25,0(r31)
lwz r25,0x24(r25)

# Round the pointer to the Relocation Data up to the nearest multiple of 0x20 bytes
addi r25,r25,31
rlwinm r25,r25,0,0,26

# Allocate memory based on the adjusted file size
mr r3,r26 # File Size
bl allocateFromMainLoopRelocMemory

# Backup the returned address to be used for later
mr r24,r3

# Read file from DVD
mr r3,r27 # File info pointer (argument 1)
mr r4,r24 # File buffer (argument 2)
mr r5,r26 # File size (argument 3)
li r6,0 # File offset (argument 4)
bl (read_entire_file_using_dvdread_prio_async - LOADER_LOWMEM_LOCATION + __start)

# Get the BSS Area size and allocate memory for it
lwz r3,0x20(r24)
bl allocateFromMainLoopRelocMemory

# Backup the returned address to be used for later
mr r23,r3

# Link the functions in the REL
mr r3,r24 # Pointer to the Module
mr r4,r23 # Pointer to the BSS Area
bl (OSLink - LOADER_LOWMEM_LOCATION + __start)
cmpwi r3,1 # Check if something went wrong
bne- callOSUnlink

# Store the Main Loop's Module, the Main Loop BSS Area, the Main Loop's Relocation Data
# size, the next free spot in Relocation Data, the BSS Area, and the REL Module
lwz r3,0(r31) # Pointer to the Main Loop's Module
stw r3,0x4524(r30)
lwz r3,0x4(r31) # Pointer to the Main Loop's BSS Area
stw r3,0x4528(r30)
stw r25,0x452c(r30) # Pointer to the Main Loop's Relocation Data
stw r23,0x4530(r30) # Pointer to the BSS Area
stw r24,0x4534(r30) # Pointer to the Module

# Get the REL Prolog Pointer
lwz r29,0x34(r24)
b callDVDClose

allocateFromMainLoopRelocMemory:
stwu sp,-0x10(sp)
mflr r0
stw r0,0x14(sp)
stmw r30,0x8(sp)

addi r4,r3,31
rlwinm r4,r4,0,0,26 # Round the size up to the nearest multiple of 0x20 bytes
mr r31,r4 # Size
mr r3,r25 # Pointer to the Main Loop's Relocation Data

# Set the next free spot in the Main Loop's Relocation Data
add r25,r25,r4

clearAndFlushMemory:
mr r30,r3 # Dest
li r4,0
mr r5,r31 # Size
bl (memset - LOADER_LOWMEM_LOCATION + __start)

# Flush the memory
mr r3,r30 # Dest
mr r4,r31 # Size
bl (DCFlushRange - LOADER_LOWMEM_LOCATION + __start)

mr r3,r30 # Dest
lmw r30,0x8(sp)
lwz r0,0x14(sp)
mtlr r0
addi sp,sp,0x10
blr

allocateFromArenaLow:
stwu sp,-0x10(sp)
mflr r0
stw r0,0x14(sp)
stmw r30,0x8(sp)

addi r3,r3,31
rlwinm r3,r3,0,0,26 # Round the size up to the nearest multiple of 0x20 bytes
mr r31,r3 # Size
li r4,32 # Alignment
bl (OSAllocFromArenaLow - LOADER_LOWMEM_LOCATION + __start)
b clearAndFlushMemory

callOSUnlink:
mr r3,r24 # Pointer to the Module
bl (OSUnlink - LOADER_LOWMEM_LOCATION + __start)

callDVDClose:
mr r3,r27 # File info ptr
bl (DVDClose - LOADER_LOWMEM_LOCATION + __start)

# Restore the Arena Low
stw r28,ARENA_LOW_OFFSET(r13)

# Run the REL Prolog if the load was successful
cmpwi r29,0
beq- exit
mtctr r29
bctrl

exit:
# Restore the overwritten instruction
lwz r3,0(r31) # Pointer to the Main Loop's Module

# Go back to the main function
b ((LOADER_HOOK_LOCATION + 4) - LOADER_LOWMEM_LOCATION + __start)

filenameString:
.asciz "mkb2.rel_sample.rel"

__end:
#pragma once

#include <gc/mathtypes.h>

namespace gc
{

struct DVDDir;
struct DVDDiskID;

struct DVDCommandBlock {
    struct DVDCommandBlock * next;
    struct DVDCommandBlock * prev;
    u32 command;
    s32 state;
    u32 offset;
    u32 length;
    void * addr;
    u32 currTransferSize;
    u32 transferredSize;
    struct DVDDiskID * id;
    void (* callback)(s32, struct DVDCommandBlock *);
    void * userData;
};

struct DVDFileInfo {
    struct DVDCommandBlock cb;
    u32 startAddr;
    u32 length;
    struct DVDFileInfo * next;
    void (* callback)(s32, struct DVDFileInfo *);
};

extern "C" {

bool DVDOpen(char *fileName, DVDFileInfo *fileInfo);
s32 DVDConvertPathToEntrynum(char *fileName);
bool DVDFastOpen(s32 entrynum, DVDFileInfo *fileInfo);
void DVDChangeDir(char *dirName);
bool DVDOpenDir(char *dirName, DVDDir *dir);
bool DVDClose(DVDFileInfo *fileInfo);
u32 read_entire_file_using_dvdread_prio_async(DVDFileInfo *fileInfo,void *addr,s32 length,s32 offset);

// This used to be a documented public-facing SDK function but it was removed in post-2000 SDKs.
// Still present in SMB2 and potentially useful to us.
bool DVDConvertEntrynumToPath(s32 entrynum, char *outPath, u32 maxPathSize);

}

}

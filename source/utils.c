
#include <string.h>

#include <StandardFile.h>
#include <TextUtils.h>

#include <Debugging.h>

#include "utils.h"

// from DGD

const char* GetFullPath(char* buf, short vRefNum, const unsigned char* fname)
{
    Str255 str;
    CInfoPBRec dir;
 
    buf += MAX_PATHLEN - 1;
    buf[0] = '\0';
    memcpy(str, fname, fname[0] + 1);

    dir.dirInfo.ioNamePtr = str;
    dir.dirInfo.ioCompletion = NULL;
    dir.dirInfo.ioFDirIndex = 0;
    dir.dirInfo.ioVRefNum = vRefNum;
    dir.dirInfo.ioDrDirID = 0;
    for (;;)
    {
        PBGetCatInfoSync(&dir);
        memcpy(buf -= str[0], str + 1, str[0]);
        if (dir.dirInfo.ioDrDirID == 2)
        {
            return buf;
        }
        *--buf = ':';
        dir.dirInfo.ioFDirIndex = -1;
        dir.dirInfo.ioDrDirID = dir.dirInfo.ioDrParID;
    }
}


#include <stdlib.h>

#include <StandardFile.h>
#include <Windows.h>
#include <TextUtils.h>

#include <Debugging.h>

#include "render.h"
#include "utils.h"

int main()
{
#if !TARGET_API_MAC_CARBON
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(NULL);
    InitCursor();
#endif

    Point       where;
    SFTypeList  typeList;
    SFReply     reply;
    char        path_buf[MAX_PATHLEN];
    char*       path;

    SetPt(&where, 100, 80);
    SFGetFile(where, "\p", NULL, -1, typeList, NULL, &reply);
    if (reply.good)
    {
        path = GetFullPath(path_buf, reply.vRefNum, reply.fName);

        WindowPtr doc_window;
        doc_window = NewWindow(NULL, &(qd.screenBits.bounds), path, true, 0, (WindowPtr)-1, false, 0);
        SetPort(doc_window);
        
	render(path, doc_window, 100, 0);
    }

    EventRecord myEvent;
    for (;;)
    {
        SystemTask();
        if (WaitNextEvent(everyEvent, &myEvent, 5L, NULL))
            if (myEvent.what == mouseDown) break;
    }

    FlushEvents(everyEvent, -1);
    return 0;
}


#include <stdio.h>
#include <stdlib.h>

#include <Quickdraw.h>
#include <MacWindows.h>
#include <TextUtils.h>

#include <Debugging.h>

#include "mupdf/fitz.h"
#include "render.h"

int render(const char* filename, WindowPtr window, float zoom, int page_number)
{
    FILE *fp;    
    int page_count;
    fz_context *ctx;
    fz_document *doc;
    fz_pixmap *pix;
    fz_matrix ctm;
    int x, y;

    /* Create a context to hold the exception stack and various caches. */
    ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    if (!ctx)
    {
        DebugStr("\pfz_new_context failed");
        return EXIT_FAILURE;
    }

    fz_set_aa_level(ctx, 0);

    /* Register the default file types to handle. */
    fz_try(ctx)
        fz_register_document_handlers(ctx);
    fz_catch(ctx)
    {
        //fprintf(stderr, "cannot register document handlers: %s\n", fz_caught_message(ctx));
        DebugStr("\pcannot register document handlers");
        fz_drop_context(ctx);
        return EXIT_FAILURE;
    }

    /* Open the document. */
    fz_try(ctx)
        doc = fz_open_document(ctx, filename);
    fz_catch(ctx)
    {
        DebugStr("\pcould not open document");
        fz_drop_context(ctx);
        return EXIT_FAILURE;
    }

    /* Count the number of pages. */
    fz_try(ctx)
        page_count = fz_count_pages(ctx, doc);
    fz_catch(ctx)
    {
        fprintf(stderr, "cannot count number of pages: %s\n", fz_caught_message(ctx));
        fz_drop_document(ctx, doc);
        fz_drop_context(ctx);
        return -1;
    }

    if (page_number < 0 || page_number >= page_count)
    {
            fprintf(stderr, "page number out of range: %d (page count %d)\n", page_number + 1, page_count);
            fz_drop_document(ctx, doc);
            fz_drop_context(ctx);
            return EXIT_FAILURE;
    }

    /* Compute a transformation matrix for the zoom and rotation desired. */
    /* The default resolution without scaling is 72 dpi. */
    fz_scale(&ctm, zoom / 100, zoom / 100);
    //fz_pre_rotate(&ctm, 0);

    /* Render page to an RGB pixmap. */
    fz_try(ctx)
        pix = fz_new_pixmap_from_page_number(ctx, doc, page_number, &ctm, fz_device_gray(ctx), 0);
    fz_catch(ctx)
    {
        fz_drop_document(ctx, doc);
        fz_drop_context(ctx);
        return EXIT_FAILURE;
    } 

    fz_bitmap *bitmap = fz_new_bitmap_from_pixmap(ctx, pix, fz_default_halftone(ctx, 1));

    Rect bm_rect;
    SetRect(&bm_rect, 0, 0, pix->w, pix->h);
    BitMap qd_bitmap;
    qd_bitmap.baseAddr = bitmap->samples;
    qd_bitmap.rowBytes = bitmap->stride;
    qd_bitmap.bounds = bm_rect;

    CopyBits(&qd_bitmap, &(window->portBits), &bm_rect, &bm_rect, srcCopy, NULL);

    return 0;

}

/******************************Module*Header*******************************\
* Module Name: bank.c
*
* Contains all the banking code for the display driver.
*
* It's helpful not to have to implement all the DDI drawing functionality
* in a driver (who wants to write the code to support true ROP4's with
* arbitrary sized patterns?).  Fortunately, we can punt to GDI for any
* drawing we don't want to do.  And if GDI can write directly on the frame
* buffer bits, performance won't even be toooo bad.
*
* NT's GDI can draw on any standard format frame buffer.  When the entire
* frame buffer can be mapped into main memory, it's very simple to set up:
* the display driver tells GDI the frame buffer format and location, and
* GDI can then just draw directly.
*
* When only one bank of the frame buffer can be mapped into main memory
* at one time (e.g., there is a moveable 64k aperture) things are not
* nearly so easy.  For every bank spanned by a drawing operation, we have
* to set the hardware to the bank, and call back to GDI.  We tell GDI
* to draw only on the mapped-in bank by mucking with the drawing call's
* CLIPOBJ.
*
* This module contains the code for doing all banking support.
*
* This code supports 8, 16 and 32bpp colour depths, arbitrary bank
* sizes, and handles 'broken rasters' (which happens when the bank size
* is not a multiple of the scan length; some scans will end up being
* split over two separate banks).
*
* Note:  If you mess with this code and break it, you can expect to get
*        random access violations on call-backs in internal GDI routines
*        that are very hard to debug.
*
* Copyright (c) 1993-1995 Microsoft Corporation
\**************************************************************************/

#include "precomp.h"

VOID vBankSelectMode(
PDEV*        ppdev,
BANK_MODE    bankm)
{
    if (bankm == BANK_ON)
    {
        // Make sure the processor graphics engine is idle before we start
        // drawing:

        if (ppdev->pjMmBase != NULL)
        {
            MM_WAIT_FOR_IDLE(ppdev, ppdev->pjMmBase);
            MM_DATAPATH_CTRL(ppdev, ppdev->pjMmBase, SRC_IS_CPU_DATA);
            MM_CTRL_REG_1(ppdev, ppdev->pjMmBase, PACKED_PIXEL_VIEW |
                                                  BITS_PER_PIX_8    |
                                                  ENAB_TRITON_MODE);
        }
        else
        {
            IO_WAIT_FOR_IDLE(ppdev, ppdev->pjIoBase);
            IO_DATAPATH_CTRL(ppdev, ppdev->pjIoBase, SRC_IS_CPU_DATA);
            IO_CTRL_REG_1(ppdev, ppdev->pjIoBase, PACKED_PIXEL_VIEW |
                                                  BITS_PER_PIX_8    |
                                                  ENAB_TRITON_MODE);
        }
    }
}

VOID vBankMap(
PDEV*       ppdev,
LONG        iBank)
{
#if defined(i386)

    volatile PFN pfnBank;

    pfnBank = ppdev->pfnBankSwitchCode;

    _asm mov eax,iBank;
    _asm add eax,eax;
    _asm mov edx,eax;
    _asm inc edx;

    _asm call pfnBank;

#else

    ASSERTDD(iBank == 0, "Shouldn't be banking on non-x86");

#endif
}


BOOL bBankInitialize(
PDEV*       ppdev)
{
    // Set this now in case we have to early-out:

    ppdev->pBankInfo = NULL;

    // Only the x86 miniport has banking code; the Alpha miniport always
    // maps the frame buffer linearly:

    #if defined(i386)
    {
        PVIDEO_BANK_SELECT  pBankInfo;
        UINT                ReturnedDataLength;
        VIDEO_BANK_SELECT   TempBankInfo;
        DWORD               status;

        // Query the miniport for banking info for this mode.
        //
        // First, figure out how big a buffer we need for the banking info
        // (returned in TempBankInfo->Size).

        if (status = EngDeviceIoControl(ppdev->hDriver,
                             IOCTL_VIDEO_GET_BANK_SELECT_CODE,
                             NULL,                      // input buffer
                             0,
                             (LPVOID) &TempBankInfo,    // output buffer
                             sizeof(VIDEO_BANK_SELECT),
                             &ReturnedDataLength))
        {
        //
        // We expect this call to fail, because we didn't allow any room
        // for the code; we just want to get the required output buffer
        // size. Make sure we got the expected error, ERROR_MORE_DATA.
        //
// BUGBUG
// BUGBUG reenable this check when we fix error reporting from miniports

        //if (status != ERROR_MORE_DATA) {
        //
        //    //
        //    // Should post error and return FALSE
        //    //
        //
        //    RIP("Initialization error - GetBankSelectCode, first call");
        //
        //    goto ReturnFailure;
        //}
        }

        // Now, allocate a buffer of the required size and get the banking info.

        pBankInfo = (PVIDEO_BANK_SELECT) EngAllocMem(FL_ZERO_MEMORY,
                        TempBankInfo.Size, ALLOC_TAG);
        if (pBankInfo == NULL)
        {
            RIP("Initialization ReturnFailure -- couldn't get memory for bank info");
            goto ReturnFailure;
        }

        // Remember it so we can free it later:

        ppdev->pBankInfo = pBankInfo;

        if (EngDeviceIoControl(ppdev->hDriver,
                             IOCTL_VIDEO_GET_BANK_SELECT_CODE,
                             NULL,
                             0,
                             (LPVOID) pBankInfo,
                             TempBankInfo.Size,
                             &ReturnedDataLength))
        {
            RIP("Initialization ReturnFailure -- bBankInitialize, second call");
            goto ReturnFailure;
        }

        ppdev->pfnBankSwitchCode =
            (PFN) (((BYTE*) pBankInfo) + pBankInfo->CodeOffset);

        return(TRUE);

    ReturnFailure:

        if (ppdev->pBankInfo)
        {
            EngFreeMem(ppdev->pBankInfo);
        }

        return(FALSE);
    }
    #endif

    return(TRUE);
}

/******************************Public*Routine******************************\
* BOOL bEnableBanking
*
\**************************************************************************/

BOOL bEnableBanking(
PDEV*   ppdev)
{
    CLIPOBJ*            pcoBank;
    SURFOBJ*            psoBank;
    SIZEL               sizl;
    HSURF               hsurf;
    LONG                lDelta;
    LONG                cjBank;
    LONG                cPower2;

    // This routine may be called multiple times (e.g., each time
    // full-screen is exited), so make sure we do any allocations
    // only once:

    if (ppdev->pcoBank == NULL)
    {
        // Create a temporary clip object that we'll use for the bank
        // when we're given a Null or DC_TRIVIAL clip object:

        pcoBank = EngCreateClip();
        if (pcoBank == NULL)
            goto ReturnFalse;

        // We break every per-bank GDI call-back into simple rectangles:

        pcoBank->iDComplexity = DC_RECT;
        pcoBank->fjOptions    = OC_BANK_CLIP;

        // Create a GDI surface that we'll wrap around our bank in
        // call-backs:

        sizl.cx = ppdev->cxMemory;
        sizl.cy = ppdev->cyMemory;

        hsurf = (HSURF) EngCreateBitmap(sizl,
                                        ppdev->lDelta,
                                        ppdev->iBitmapFormat,
                                        BMF_TOPDOWN,
                                        ppdev->pjScreen);

        // Note that we hook zero calls -- after all, the entire point
        // of all this is to have GDI do all the drawing on the bank.
        // Once we're done the association, we can leave the surface
        // permanently locked:

        if ((hsurf == 0)                                        ||
            (!EngAssociateSurface(hsurf, ppdev->hdevEng, 0))    ||
            (!(psoBank = EngLockSurface(hsurf))))
        {
            DISPDBG((0, "Failed wrapper surface creation"));

            EngDeleteSurface(hsurf);
            EngDeleteClip(pcoBank);

            goto ReturnFalse;
        }

        ppdev->pcoBank    = pcoBank;
        ppdev->psoBank    = psoBank;

        lDelta = ppdev->lDelta;
        cjBank = ppdev->cjBank;

        ASSERTDD(lDelta > 0, "Bad things happen with negative lDeltas");
        ASSERTDD(cjBank > lDelta, "Worse things happen with bad bank sizes");

        if (((lDelta & (lDelta - 1)) != 0) || ((cjBank & (cjBank - 1)) != 0))
        {
            // When either the screen stride or the bank size is not a power
            // of two, we have to use the slower 'bBankComputeNonPower2'
            // function for bank calculations, 'cause there can be broken
            // rasters and stuff:

            ppdev->pfnBankCompute = bBankComputeNonPower2;
        }
        else
        {
            // We can use the super duper fast bank calculator.  Yippie,
            // yahoo!  (I am easily amused.)

            cPower2 = 0;
            while (cjBank != lDelta)
            {
                cjBank >>= 1;
                cPower2++;
            }

            // We've just calculated that cjBank / lDelta = 2 ^ cPower2:

            ppdev->cPower2ScansPerBank = cPower2;

            while (cjBank != 1)
            {
                cjBank >>= 1;
                cPower2++;
            }

            // Continuing on, we've calculated that cjBank = 2 ^ cPower2:

            ppdev->cPower2BankSizeInBytes = cPower2;

            ppdev->pfnBankCompute = bBankComputePower2;
        }
    }

    // Warm up the hardware:

    if (!bBankInitialize(ppdev))
        goto ReturnFalse;

    vBankSelectMode(ppdev, BANK_ENABLE);

    DISPDBG((5, "Passed bEnableBanking"));

    return(TRUE);

ReturnFalse:

    DISPDBG((0, "Failed bEnableBanking!"));

    return(FALSE);
}

/******************************Public*Routine******************************\
* VOID vDisableBanking
*
\**************************************************************************/

VOID vDisableBanking(PDEV* ppdev)
{
    HSURF hsurf;

    if (ppdev->psoBank != NULL)
    {
        hsurf = ppdev->psoBank->hsurf;
        EngUnlockSurface(ppdev->psoBank);
        EngDeleteSurface(hsurf);
    }

    if (ppdev->pcoBank != NULL)
        EngDeleteClip(ppdev->pcoBank);

    if (ppdev->pBankInfo)
    {
        EngFreeMem(ppdev->pBankInfo);
    }
}

/******************************Public*Routine******************************\
* VOID vAssertModeBanking
*
\**************************************************************************/

VOID vAssertModeBanking(
PDEV*   ppdev,
BOOL    bEnable)
{
    // Inform the miniport bank code about the change in state:

    vBankSelectMode(ppdev, bEnable ? BANK_ENABLE : BANK_DISABLE);
}

/******************************Public*Routine******************************\
* BOOL bBankComputeNonPower2
*
* Given the bounds of the drawing operation described by 'prclDraw',
* computes the bank number and rectangle bounds for the first engine
* call back.
*
* Returns the bank number, 'prclBank' is the bounds for the first
* call-back, and 'pcjOffset' is the adjustment for 'pvScan0'.
*
* This routine does a couple of divides for the bank calculation.  We
* don't use a look-up table for banks because it's not straight forward
* to use with broken rasters, and with large amounts of video memory
* and small banks, the tables could get large.  We'd probably use it
* infrequently enough that the memory manager would be swapping it
* in and out whenever we touched it.
*
* Returns TRUE if prclDraw is entirely contained in one bank; FALSE if
* prclDraw spans multiple banks.
*
\**************************************************************************/

BOOL bBankComputeNonPower2( // Type FNBANKCOMPUTE
PDEV*       ppdev,
RECTL*      prclDraw,       // Extents of drawing operation, in absolute
                            //  coordinates
RECTL*      prclBank,       // Returns bounds of drawing operation for this
                            //  bank, in absolute coordinates
LONG*       pcjOffset,      // Returns the byte offset for this bank
LONG*       piBank)         // Returns the bank number
{
    LONG cjBufferOffset;
    LONG iBank;
    LONG cjBank;
    LONG cjBankOffset;
    LONG cjBankRemainder;
    LONG cjScan;
    LONG cScansInBank;
    LONG cjScanRemainder;
    LONG lDelta;
    BOOL bOneBank;

    bOneBank = FALSE;
    lDelta   = ppdev->lDelta;

    cjBufferOffset  = prclDraw->top * lDelta
                    + (prclDraw->left << ppdev->cPelSize);

    cjBank          = ppdev->cjBank;

    // iBank        = cjBufferOffset / cjBank;
    // cjBankOffset = cjBufferOffset % cjBank;

    QUOTIENT_REMAINDER(cjBufferOffset, cjBank, iBank, cjBankOffset);

    *piBank         = iBank;
    *pcjOffset      = iBank * cjBank;
    cjBankRemainder = cjBank - cjBankOffset;
    cjScan          = (prclDraw->right - prclDraw->left) << ppdev->cPelSize;

    if (cjBankRemainder < cjScan)
    {
        // Oh no, we've got a broken raster!

        prclBank->left   = prclDraw->left;
        prclBank->right  = prclDraw->left +
                           (cjBankRemainder >> ppdev->cPelSize);
        prclBank->top    = prclDraw->top;
        prclBank->bottom = prclDraw->top + 1;
    }
    else
    {
        // cScansInBank    = cjBankRemainder / lDelta;
        // cjScanRemainder = cjBankRemainder % lDelta;

        ASSERTDD(lDelta > 0, "We assume positive lDelta here");

        QUOTIENT_REMAINDER(cjBankRemainder, lDelta,
                           cScansInBank, cjScanRemainder);

        if (cjScanRemainder >= cjScan)
        {
            // The bottom scan of the bank may be broken, but it breaks after
            // any drawing we'll be doing on that scan.  So we can simply
            // add the scan to this bank:

            cScansInBank++;
        }

        prclBank->left   = prclDraw->left;
        prclBank->right  = prclDraw->right;
        prclBank->top    = prclDraw->top;
        prclBank->bottom = prclDraw->top + cScansInBank;

        if (prclBank->bottom >= prclDraw->bottom)
        {
            prclBank->bottom  = prclDraw->bottom;
            bOneBank          = TRUE;
        }
    }

    return(bOneBank);
}

/******************************Public*Routine******************************\
* BOOL bBankComputePower2
*
* Functions the same as 'bBankComputeNonPower2', except that it is
* an accelerated special case for when both the screen stride and bank
* size are powers of 2.
*
\**************************************************************************/

BOOL bBankComputePower2(    // Type FNBANKCOMPUTE
PDEV*       ppdev,
RECTL*      prclDraw,       // Extents of drawing operation, in absolute
                            //  coordinates
RECTL*      prclBank,       // Returns bounds of drawing operation for this
                            //  bank, in absolute coordinates
LONG*       pcjOffset,      // Returns the byte offset for this bank
LONG*       piBank)         // Returns the bank number
{
    LONG iBank;
    LONG yTopNextBank;
    BOOL bOneBank;

    iBank        = prclDraw->top >> ppdev->cPower2ScansPerBank;
    yTopNextBank = (iBank + 1) << ppdev->cPower2ScansPerBank;
    *piBank      = iBank;
    *pcjOffset   = iBank << ppdev->cPower2BankSizeInBytes;

    prclBank->left   = prclDraw->left;
    prclBank->right  = prclDraw->right;
    prclBank->top    = prclDraw->top;
    prclBank->bottom = yTopNextBank;

    bOneBank = FALSE;
    if (prclBank->bottom >= prclDraw->bottom)
    {
        prclBank->bottom  = prclDraw->bottom;
        bOneBank          = TRUE;
    }

    return(bOneBank);
}

#if GDI_BANKING                 // GDI can't draw directly in the framebuffer
                                //   when running on the Alpha

/******************************Public*Routine******************************\
* VOID vBankStart
*
* Given the bounds of the drawing operation described by 'prclDraw' and
* the original clip object, maps in the first bank and returns in
* 'pbnk->pco' and 'pbnk->pso' the CLIPOBJ and SURFOBJ to be passed to the
* engine for the first banked call-back.
*
* Note: This routine only supports the screen being the destination, and
*       not the source.  We have a separate, faster routine for doing
*       SRCCOPY reads from the screen, so it isn't worth the extra code
*       size to implement.
*
\**************************************************************************/

VOID vBankStart(
PDEV*       ppdev,      // Physical device information.
RECTL*      prclDraw,   // Rectangle bounding the draw area, in relative
                        //  coordinates.  Note that 'left' and 'right'
                        //  should be set for correct handling with broken
                        //  rasters.
CLIPOBJ*    pco,        // Original drawing clip object (may be modified).
BANK*       pbnk)       // Resulting bank information.
{
    LONG cjOffset;
    LONG xOffset;
    LONG yOffset;

    xOffset = ppdev->xOffset;
    yOffset = ppdev->yOffset;

    if ((pco == NULL) || (pco->iDComplexity == DC_TRIVIAL))
    {
        pco = ppdev->pcoBank;

        // Reset the clipping flag to trivial because we may have left
        // it as rectangular in a previous call:

        pco->iDComplexity = DC_TRIVIAL;

        // At the same time we convert to absolute coordinates, make sure
        // we won't try to enumerate past the bounds of the screen:

        pbnk->rclDraw.left       = prclDraw->left   + xOffset;
        pbnk->rclDraw.right      = prclDraw->right  + xOffset;

        pbnk->rclDraw.top
            = max(0,               prclDraw->top    + yOffset);
        pbnk->rclDraw.bottom
            = min(ppdev->cyMemory, prclDraw->bottom + yOffset);
    }
    else
    {
        pbnk->rclSaveBounds    = pco->rclBounds;
        pbnk->iSaveDComplexity = pco->iDComplexity;
        pbnk->fjSaveOptions    = pco->fjOptions;

        // Let GDI know that it has to pay attention to the clip object:

        pco->fjOptions |= OC_BANK_CLIP;

        // We have to honour the original clip object's rclBounds, so
        // intersect the drawing region with it, then convert to absolute
        // coordinates:

        pbnk->rclDraw.left
            = max(prclDraw->left,   pco->rclBounds.left)   + xOffset;
        pbnk->rclDraw.right
            = min(prclDraw->right,  pco->rclBounds.right)  + xOffset;
        pbnk->rclDraw.top
            = max(prclDraw->top,    pco->rclBounds.top)    + yOffset;
        pbnk->rclDraw.bottom
            = min(prclDraw->bottom, pco->rclBounds.bottom) + yOffset;
    }

    if ((pbnk->rclDraw.left > pbnk->rclDraw.right)
     || (pbnk->rclDraw.top  > pbnk->rclDraw.bottom))
    {
        // It's conceivable that we could get a situation where we have
        // an empty draw rectangle.  Make sure we won't puke on our shoes:

        pbnk->rclDraw.left   = 0;
        pbnk->rclDraw.right  = 0;
        pbnk->rclDraw.top    = 0;
        pbnk->rclDraw.bottom = 0;
    }

    if (!ppdev->pfnBankCompute(ppdev, &pbnk->rclDraw, &pco->rclBounds,
                               &cjOffset, &pbnk->iBank))
    {
        // The drawing operation spans multiple banks.  If the original
        // clip object was marked as trivial, we have to make sure to
        // change it to rectangular so that GDI knows to pay attention
        // to the bounds of the bank:

        if (pco->iDComplexity == DC_TRIVIAL)
            pco->iDComplexity = DC_RECT;
    }

    pbnk->ppdev = ppdev;
    pbnk->pco   = pco;
    pbnk->pso   = ppdev->psoBank;

    // Convert rclBounds and pvScan0 from absolute coordinates back to
    // relative.  When GDI calculates where to start drawing, it computes
    // pjDst = pso->pvScan0 + y * pso->lDelta + (x << cPelSize), where 'x'
    // and 'y' are relative coordinates.  We'll muck with pvScan0 to get
    // it pointing to the correct spot in the bank:

    pbnk->pso->pvScan0 = ppdev->pjScreen - cjOffset
                       + yOffset * ppdev->lDelta
                       + (xOffset << ppdev->cPelSize);

    ASSERTDD((((ULONG_PTR) pbnk->pso->pvScan0) & 3) == 0,
             "Off-screen bitmaps must be dword aligned");

    pco->rclBounds.left   -= xOffset;
    pco->rclBounds.right  -= xOffset;
    pco->rclBounds.top    -= yOffset;
    pco->rclBounds.bottom -= yOffset;

    // Enable banking and map in bank iBank:

    vBankSelectMode(ppdev, BANK_ON);
    vBankMap(ppdev, pbnk->iBank);
}

/******************************Public*Routine******************************\
* BOOL bBankEnum
*
* If there is another bank to be drawn on, maps in the bank and returns
* TRUE and the CLIPOBJ and SURFOBJ to be passed in the banked call-back.
*
* If there were no more banks to be drawn, returns FALSE.
*
\**************************************************************************/

BOOL bBankEnum(
BANK* pbnk)
{
    LONG     iBank;
    LONG     cjOffset;
    PDEV*    ppdev;
    CLIPOBJ* pco;
    LONG     xOffset;
    LONG     yOffset;

    ppdev   = pbnk->ppdev;
    pco     = pbnk->pco;
    xOffset = ppdev->xOffset;
    yOffset = ppdev->yOffset;

    // We check here to see if we have to handle the second part of
    // a broken raster.  Recall that pbnk->rclDraw is in absolute
    // coordinates, but pco->rclBounds is in relative coordinates:

    if (pbnk->rclDraw.right - xOffset != pco->rclBounds.right)
    {
        // The clip object's 'top' and 'bottom' are already correct:

        pco->rclBounds.left  = pco->rclBounds.right;
        pco->rclBounds.right = pbnk->rclDraw.right - xOffset;

        pbnk->pso->pvScan0 = (BYTE*) pbnk->pso->pvScan0 - ppdev->cjBank;
        pbnk->iBank++;

        vBankMap(ppdev, pbnk->iBank);

        return(TRUE);
    }

    if (pbnk->rclDraw.bottom > pco->rclBounds.bottom + yOffset)
    {
        // Advance the drawing area 'top' to account for the bank we've
        // just finished, and map in the new bank:

        pbnk->rclDraw.top = pco->rclBounds.bottom + yOffset;

        ppdev->pfnBankCompute(ppdev, &pbnk->rclDraw, &pco->rclBounds,
                              &cjOffset, &iBank);

        // Convert rclBounds back from absolute to relative coordinates:

        pco->rclBounds.left   -= xOffset;
        pco->rclBounds.right  -= xOffset;
        pco->rclBounds.top    -= yOffset;
        pco->rclBounds.bottom -= yOffset;

        // If we just finished handling a broken raster, we've already
        // got the bank mapped in:

        if (iBank != pbnk->iBank)
        {
            pbnk->iBank = iBank;
            pbnk->pso->pvScan0 = (BYTE*) pbnk->pso->pvScan0 - ppdev->cjBank;

            vBankMap(ppdev, iBank);
        }

        return(TRUE);
    }

    // We're done!  Turn off banking and reset the clip object if necessary:

    vBankSelectMode(ppdev, BANK_OFF);

    if (pco != ppdev->pcoBank)
    {
        pco->rclBounds    = pbnk->rclSaveBounds;
        pco->iDComplexity = pbnk->iSaveDComplexity;
        pco->fjOptions    = pbnk->fjSaveOptions;
    }

    return(FALSE);
}

#endif // GDI_BANKING

/******************************Public*Routine******************************\
* VOID vAlignedCopy
*
* Copies the given portion of a bitmap, using dword alignment for the
* screen.  Note that this routine has no notion of banking.
*
* Updates ppjDst and ppjSrc to point to the beginning of the next scan.
*
\**************************************************************************/

VOID vAlignedCopy(
BYTE**  ppjDst,
LONG    lDstDelta,
BYTE**  ppjSrc,
LONG    lSrcDelta,
LONG    cjScan,
LONG    cyScan,
BOOL    bDstIsScreen)
{
    BYTE* pjDst;
    BYTE* pjSrc;
    LONG  cjMiddle;
    LONG  culMiddle;
    LONG  cjStartPhase;
    LONG  cjEndPhase;

    pjSrc = *ppjSrc;
    pjDst = *ppjDst;

    cjStartPhase = (LONG)((0 - ((bDstIsScreen) ? (ULONG_PTR) pjDst
                                               : (ULONG_PTR) pjSrc)) & 3);
    cjMiddle     = cjScan - cjStartPhase;

    if (cjMiddle < 0)
    {
        cjStartPhase = 0;
        cjMiddle     = cjScan;
    }

    lSrcDelta -= cjScan;
    lDstDelta -= cjScan;            // Account for middle

    cjEndPhase = cjMiddle & 3;
    culMiddle  = cjMiddle >> 2;

    #if defined(i386)
    {
        _asm {
            mov     eax,lSrcDelta   ; eax = Source delta accounting for middle
            mov     ebx,lDstDelta   ; ebx = Dest delta accounting for middle
            mov     edx,cyScan      ; edx = Count of scans
            mov     esi,pjSrc       ; esi = Source pointer
            mov     edi,pjDst       ; edi = Dest pointer

        Next_Scan:

            mov     ecx,cjStartPhase
            rep     movsb

            mov     ecx,culMiddle
            rep     movsd

            mov     ecx,cjEndPhase
            rep     movsb

            add     esi,eax         ; Advance to next scan
            add     edi,ebx
            dec     edx
            jnz     Next_Scan

            mov     eax,ppjSrc      ; Save the updated pointers
            mov     ebx,ppjDst
            mov     [eax],esi
            mov     [ebx],edi
        }
    }
    #else
    {
        LONG i;

        // Because of its bus design, we cannot do a straight memcpy
        // to/from the frame buffer on an Alpha -- we have to go
        // through WRITE/READ macros.

        // First, to be safe, flush all pending I/O:

        MEMORY_BARRIER();

        if (bDstIsScreen)
        {
            // Align to the destination (implying that the source may be
            // unaligned):

            for (; cyScan > 0; cyScan--)
            {
                for (i = cjStartPhase; i > 0; i--)
                {
                    WRITE_REGISTER_UCHAR(pjDst, *pjSrc);
                    pjSrc++;
                    pjDst++;
                }

                for (i = culMiddle; i > 0; i--)
                {
                    WRITE_REGISTER_ULONG(pjDst, *((ULONG UNALIGNED *) pjSrc));
                    pjSrc += sizeof(ULONG UNALIGNED);
                    pjDst += sizeof(ULONG);
                }

                for (i = cjEndPhase; i > 0; i--)
                {
                    WRITE_REGISTER_UCHAR(pjDst, *pjSrc);
                    pjSrc++;
                    pjDst++;
                }

                pjSrc += lSrcDelta;
                pjDst += lDstDelta;
            }
        }
        else
        {
            // Align to the source (implying that the destination may be
            // unaligned):

            for (; cyScan > 0; cyScan--)
            {
                for (i = cjStartPhase; i > 0; i--)
                {
                    *pjDst = READ_REGISTER_UCHAR(pjSrc);
                    pjSrc++;
                    pjDst++;
                }

                for (i = culMiddle; i > 0; i--)
                {
                    #if !defined(ALPHA)
                    {
                        *((ULONG UNALIGNED *) pjDst) = READ_REGISTER_ULONG(pjSrc);
                    }
                    #else
                    {
                        // There are some board 864/964 boards where we can't
                        // do dword reads from the frame buffer without
                        // crashing the Avanti.

                        *((ULONG UNALIGNED *) pjDst) =
                         ((ULONG) READ_REGISTER_UCHAR(pjSrc + 3) << 24) |
                         ((ULONG) READ_REGISTER_UCHAR(pjSrc + 2) << 16) |
                         ((ULONG) READ_REGISTER_UCHAR(pjSrc + 1) << 8)  |
                         ((ULONG) READ_REGISTER_UCHAR(pjSrc));
                    }
                    #endif

                    pjSrc += sizeof(ULONG);
                    pjDst += sizeof(ULONG UNALIGNED);
                }

                for (i = cjEndPhase; i > 0; i--)
                {
                    *pjDst = READ_REGISTER_UCHAR(pjSrc);
                    pjSrc++;
                    pjDst++;
                }

                pjSrc += lSrcDelta;
                pjDst += lDstDelta;
            }
        }

        *ppjSrc = pjSrc;            // Save the updated pointers
        *ppjDst = pjDst;
    }
    #endif
}

/******************************Public*Routine******************************\
* VOID vPutBits
*
* Copies the bits from the given surface to the screen, using the memory
* aperture.  Must be pre-clipped.
*
\**************************************************************************/

VOID vPutBits(
PDEV*       ppdev,
SURFOBJ*    psoSrc,
RECTL*      prclDst,            // Absolute coordinates!
POINTL*     pptlSrc)            // Absolute coordinates!
{
    RECTL   rclDraw;
    RECTL   rclBank;
    LONG    iBank;
    LONG    cjOffset;
    LONG    cyScan;
    LONG    lDstDelta;
    LONG    lSrcDelta;
    BYTE*   pjDst;
    BYTE*   pjSrc;
    LONG    cjScan;
    LONG    iNewBank;
    LONG    cjRemainder;

    // We need a local copy of 'rclDraw' because we'll be iteratively
    // modifying 'top' and passing the modified rectangle back into
    // bBankComputeNonPower2:

    rclDraw = *prclDst;

    ASSERTDD((rclDraw.left   >= 0) &&
             (rclDraw.top    >= 0) &&
             (rclDraw.right  <= ppdev->cxMemory) &&
             (rclDraw.bottom <= ppdev->cyMemory),
             "Rectangle wasn't fully clipped");

    // Compute the first bank, enable banking, then map in iBank:

    ppdev->pfnBankCompute(ppdev, &rclDraw, &rclBank, &cjOffset, &iBank);
    vBankSelectMode(ppdev, BANK_ON);
    vBankMap(ppdev, iBank);

    // Calculate the pointer to the upper-left corner of both rectangles:

    lDstDelta = ppdev->lDelta;
    pjDst     = ppdev->pjScreen + rclDraw.top  * lDstDelta
                                + (rclDraw.left << ppdev->cPelSize)
                                - cjOffset;

    lSrcDelta = psoSrc->lDelta;
    pjSrc     = (BYTE*) psoSrc->pvScan0 + pptlSrc->y * lSrcDelta
                                        + (pptlSrc->x << ppdev->cPelSize);

    while (TRUE)
    {
        cjScan = (rclBank.right  - rclBank.left) << ppdev->cPelSize;
        cyScan = (rclBank.bottom - rclBank.top);

        vAlignedCopy(&pjDst, lDstDelta, &pjSrc, lSrcDelta, cjScan, cyScan,
                     TRUE);             // Screen is the destination

        if (rclDraw.right != rclBank.right)
        {
            // Handle the second part of the broken raster:

            iBank++;

            vBankMap(ppdev, iBank);

            // Number of bytes we've yet to do on the broken scan:

            cjRemainder = (rclDraw.right - rclBank.right) << ppdev->cPelSize;

            // Account for the fact that we're now one bank lower in the
            // destination:

            pjDst -= ppdev->cjBank;

            // Implicitly back up the source and destination pointers to the
            // unfinished portion of the scan:

            #if GDI_BANKING
            {
                memcpy(pjDst + (cjScan - lDstDelta),
                       pjSrc + (cjScan - lSrcDelta),
                       cjRemainder);
            }
            #else
            {
                BYTE* pjTmpDst = pjDst + (cjScan - lDstDelta);
                BYTE* pjTmpSrc = pjSrc + (cjScan - lSrcDelta);

                vAlignedCopy(&pjTmpDst, 0, &pjTmpSrc, 0, cjRemainder, 1,
                             TRUE);    // Screen is the destination
            }
            #endif
        }

        if (rclDraw.bottom > rclBank.bottom)
        {
            rclDraw.top = rclBank.bottom;
            ppdev->pfnBankCompute(ppdev, &rclDraw, &rclBank, &cjOffset,
                                  &iNewBank);

            // If we just handled the second part of a broken raster,
            // then we've already got the bank correctly mapped in:

            if (iNewBank != iBank)
            {
                pjDst -= ppdev->cjBank;
                iBank = iNewBank;

                vBankMap(ppdev, iBank);
            }
        }
        else
        {
            // We're done!  Turn off banking and leave:

            vBankSelectMode(ppdev, BANK_OFF);

            return;
        }
    }
}

/******************************Public*Routine******************************\
* VOID vGetBits
*
* Copies the bits to the given surface from the screen, using the memory
* aperture.  Must be pre-clipped.
*
\**************************************************************************/

VOID vGetBits(
PDEV*       ppdev,
SURFOBJ*    psoDst,
RECTL*      prclDst,        // Absolute coordinates!
POINTL*     pptlSrc)        // Absolute coordinates!
{
    RECTL   rclDraw;
    RECTL   rclBank;
    LONG    iBank;
    LONG    cjOffset;
    LONG    cyScan;
    LONG    lDstDelta;
    LONG    lSrcDelta;
    BYTE*   pjDst;
    BYTE*   pjSrc;
    LONG    cjScan;
    LONG    iNewBank;
    LONG    cjRemainder;

    rclDraw.left   = pptlSrc->x;
    rclDraw.top    = pptlSrc->y;
    rclDraw.right  = rclDraw.left + (prclDst->right  - prclDst->left);
    rclDraw.bottom = rclDraw.top  + (prclDst->bottom - prclDst->top);

    ASSERTDD((rclDraw.left   >= 0) &&
             (rclDraw.top    >= 0) &&
             (rclDraw.right  <= ppdev->cxMemory) &&
             (rclDraw.bottom <= ppdev->cyMemory),
             "Rectangle wasn't fully clipped");

    // Compute the first bank, enable banking, then map in iBank.

    ppdev->pfnBankCompute(ppdev, &rclDraw, &rclBank, &cjOffset, &iBank);
    vBankSelectMode(ppdev, BANK_ON);
    vBankMap(ppdev, iBank);

    // Calculate the pointer to the upper-left corner of both rectangles:

    lSrcDelta = ppdev->lDelta;
    pjSrc     = ppdev->pjScreen + rclDraw.top  * lSrcDelta
                                + (rclDraw.left << ppdev->cPelSize)
                                - cjOffset;

    lDstDelta = psoDst->lDelta;
    pjDst     = (BYTE*) psoDst->pvScan0 + prclDst->top  * lDstDelta
                                        + (prclDst->left << ppdev->cPelSize);

    while (TRUE)
    {
        cjScan = (rclBank.right  - rclBank.left) << ppdev->cPelSize;
        cyScan = (rclBank.bottom - rclBank.top);

        vAlignedCopy(&pjDst, lDstDelta, &pjSrc, lSrcDelta, cjScan, cyScan,
                     FALSE);            // Screen is the source

        if (rclDraw.right != rclBank.right)
        {
            // Handle the second part of the broken raster:

            iBank++;

            vBankMap(ppdev, iBank);

            // Number of bytes we've yet to do on the broken scan:

            cjRemainder = (rclDraw.right - rclBank.right) << ppdev->cPelSize;

            // Account for the fact that we're now one bank lower in the
            // source:

            pjSrc -= ppdev->cjBank;

            // Implicitly back up the source and destination pointers to the
            // unfinished portion of the scan.  Note that we don't have to
            // advance the pointers because they're already pointing to the
            // beginning of the next scan:

            #if GDI_BANKING
            {
                memcpy(pjDst + (cjScan - lDstDelta),
                       pjSrc + (cjScan - lSrcDelta),
                       cjRemainder);
            }
            #else
            {
                BYTE* pjTmpDst = pjDst + (cjScan - lDstDelta);
                BYTE* pjTmpSrc = pjSrc + (cjScan - lSrcDelta);

                vAlignedCopy(&pjTmpDst, 0, &pjTmpSrc, 0, cjRemainder, 1,
                             FALSE);    // Screen is the source
            }
            #endif
        }

        if (rclDraw.bottom > rclBank.bottom)
        {
            rclDraw.top = rclBank.bottom;
            ppdev->pfnBankCompute(ppdev, &rclDraw, &rclBank, &cjOffset,
                                  &iNewBank);

            // If we just handled the second part of a broken raster,
            // then we've already got the bank correctly mapped in:

            if (iNewBank != iBank)
            {
                pjSrc -= ppdev->cjBank;
                iBank = iNewBank;

                vBankMap(ppdev, iBank);
            }
        }
        else
        {
            // We're done!  Turn off banking and leave:

            vBankSelectMode(ppdev, BANK_OFF);

            return;
        }
    }
}

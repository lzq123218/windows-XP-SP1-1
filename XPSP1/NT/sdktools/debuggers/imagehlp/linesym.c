/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    linesym.c

Abstract:

    Source file and line support.

Author:

    Drew Bliss (drewb) 07-07-1997

Environment:

    User Mode

--*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntldr.h>

#include "private.h"
#include "symbols.h"
#include "globals.h"

// private version of qsort used to avoid compat problems on NT4 and win2k.
// code is published from base\crts
extern
void __cdecl dbg_qsort(void *, size_t, size_t,
                       int (__cdecl *) (const void *, const void *));

// #define DBG_LINES
// #define DBG_COFF_LINES
// #define DBG_ADDR_SEARCH
BOOL
diaAddLinesForAllMod(
    PMODULE_ENTRY mi
    );


#if defined(DBG_LINES) || defined(DBG_COFF_LINES) || defined(DBG_ADDR_SEARCH)
void __cdecl
DbgOut(PCSTR Format, ...)
{
    char Buf[512];
    va_list Args;

    va_start(Args, Format);
    _vsnprintf(Buf, sizeof(Buf), Format, Args);
    va_end(Args);
    OutputDebugStringA(Buf);
}
#endif

int
__cdecl
CompareLineAddresses(
    const void *v1,
    const void *v2
    )
{
    PSOURCE_LINE Line1 = (PSOURCE_LINE)v1;
    PSOURCE_LINE Line2 = (PSOURCE_LINE)v2;

    if (Line1->Addr < Line2->Addr) {
        return -1;
    } else if (Line1->Addr > Line2->Addr) {
        return 1;
    } else {
        return 0;
    }
}

VOID
AddSourceEntry(
    PMODULE_ENTRY mi,
    PSOURCE_ENTRY Src
    )
{
    PSOURCE_ENTRY SrcCur;

    // Overlap is currently permitted.
#if 0
    // Check for overlap between SOURCE_ENTRY address ranges.
    for (SrcCur = mi->SourceFiles;
         SrcCur != NULL;
         SrcCur = SrcCur->Next)
    {
        if (!(SrcCur->MinAddr > Src->MaxAddr ||
              SrcCur->MaxAddr < Src->MinAddr))
        {
            DbgOut("SOURCE_ENTRY overlap between %08I64X:%08I64X "
                   "and %08I64X:%08I64X\n",
                   Src->MinAddr, Src->MaxAddr,
                   SrcCur->MinAddr, SrcCur->MaxAddr);
        }
    }
#endif

    // Sort line info by address.
    dbg_qsort((PVOID)Src->LineInfo, Src->Lines, sizeof(Src->LineInfo[0]),
          CompareLineAddresses);

    // Link new source information into list, sorted by address
    // range covered by information.

    for (SrcCur = mi->SourceFiles;
         SrcCur != NULL;
         SrcCur = SrcCur->Next) {
        if (SrcCur->MinAddr > Src->MinAddr) {
            break;
        }
    }

    Src->Next = SrcCur;
    if (SrcCur == NULL) {
        if (mi->SourceFilesTail == NULL) {
            mi->SourceFiles = Src;
        } else {
            mi->SourceFilesTail->Next = Src;
        }
        Src->Prev = mi->SourceFilesTail;
        mi->SourceFilesTail = Src;
    } else {
        if (SrcCur->Prev == NULL) {
            mi->SourceFiles = Src;
        } else {
            SrcCur->Prev->Next = Src;
        }
        Src->Prev = SrcCur->Prev;
        SrcCur->Prev = Src;
    }

#ifdef DBG_LINES
    DbgOut("%08I64X %08I64X: %5d lines, '%s'\n",
           Src->MinAddr, Src->MaxAddr, Src->Lines, Src->File);
#endif
}

#define IS_SECTION_SYM(Sym) \
    ((Sym)->StorageClass == IMAGE_SYM_CLASS_STATIC && \
     (Sym)->Type == IMAGE_SYM_TYPE_NULL && \
     (Sym)->NumberOfAuxSymbols == 1)

BOOL
AddLinesForCoff(
    PMODULE_ENTRY mi,
    PIMAGE_SYMBOL allSymbols,
    DWORD numberOfSymbols,
    PIMAGE_LINENUMBER LineNumbers
    )
{
    PIMAGE_LINENUMBER *SecLines;
    BOOL Ret = FALSE;
    PIMAGE_SECTION_HEADER sh;
    ULONG i;
    PIMAGE_SYMBOL Symbol;
    ULONG LowestPointer;

    // Allocate some space for per-section data.
    SecLines = (PIMAGE_LINENUMBER *)MemAlloc(sizeof(PIMAGE_LINENUMBER)*mi->NumSections);
    if (SecLines == NULL) {
        return FALSE;
    }

    //
    // Add line number information for file groups if such
    // groups exist.
    //

    // First locate the lowest file offset for linenumbers.  This
    // is necessary to be able to compute relative linenumber pointers
    // in split images because currently the pointers aren't updated
    // during stripping.
    sh = mi->SectionHdrs;
    LowestPointer = 0xffffffff;
    for (i = 0; i < mi->NumSections; i++, sh++) {
        if (sh->NumberOfLinenumbers > 0 &&
            sh->PointerToLinenumbers != 0 &&
            sh->PointerToLinenumbers < LowestPointer)
        {
            LowestPointer = sh->PointerToLinenumbers;
        }
    }

    if (LowestPointer == 0xffffffff) {
        goto EH_FreeSecLines;
    }

    sh = mi->SectionHdrs;
    for (i = 0; i < mi->NumSections; i++, sh++) {
        if (sh->NumberOfLinenumbers > 0 &&
            sh->PointerToLinenumbers != 0)
        {
            SecLines[i] = (PIMAGE_LINENUMBER)
                (sh->PointerToLinenumbers - LowestPointer + (DWORD_PTR)LineNumbers);

#ifdef DBG_COFF_LINES
            DbgOut("Section %d: %d lines at %08X\n",
                   i, sh->NumberOfLinenumbers, SecLines[i]);
#endif
        } else {
            SecLines[i] = NULL;
        }
    }

    // Look for a file symbol.
    Symbol = allSymbols;
    for (i = 0; i < numberOfSymbols; i++) {
        if (Symbol->StorageClass == IMAGE_SYM_CLASS_FILE) {
            break;
        }

        i += Symbol->NumberOfAuxSymbols;
        Symbol += 1+Symbol->NumberOfAuxSymbols;
    }

    // If no file symbols were found, don't attempt to add line
    // number information.  Something could be done with the raw
    // linenumber info in the image (if it exists) but this probably
    // isn't an important enough case to worry about.

    while (i < numberOfSymbols) {
        ULONG iNextFile, iAfterFile;
        ULONG iCur, iSym;
        PIMAGE_SYMBOL SymAfterFile, CurSym;
        PIMAGE_AUX_SYMBOL AuxSym;
        ULONG Lines;
        ULONG MinAddr, MaxAddr;
        LPSTR FileName;
        ULONG FileNameLen;

#ifdef DBG_COFF_LINES
        DbgOut("%3X: '%s', %X\n", i, Symbol+1, Symbol->Value);
#endif

        // A file symbol's Value is the index of the next file symbol.
        // In between the two file symbols there may be static
        // section symbols which give line number counts for all
        // the line numbers in the file.
        // The file chain can be NULL terminated or a circular list,
        // in which case this code assumes the end comes when the
        // list wraps around.

        if (Symbol->Value == 0 || Symbol->Value <= i) {
            iNextFile = numberOfSymbols;
        } else {
            iNextFile = Symbol->Value;
        }

        // Compute the index of the first symbol after the current file
        // symbol.
        iAfterFile = i+1+Symbol->NumberOfAuxSymbols;
        SymAfterFile = Symbol+1+Symbol->NumberOfAuxSymbols;

        // Look for section symbols and count up the number of linenumber
        // references, the min address and the max address.
        CurSym = SymAfterFile;
        iCur = iAfterFile;
        Lines = 0;
        MinAddr = 0xffffffff;
        MaxAddr = 0;
        while (iCur < iNextFile) {
            DWORD Addr;

            if (IS_SECTION_SYM(CurSym) &&
                SecLines[CurSym->SectionNumber-1] != NULL)
            {
                AuxSym = (PIMAGE_AUX_SYMBOL)(CurSym+1);

                Lines += AuxSym->Section.NumberOfLinenumbers;

                Addr = (ULONG)(CurSym->Value+mi->BaseOfDll);

#ifdef DBG_COFF_LINES
                DbgOut("    Range %08X %08X, min %08X max %08X\n",
                       Addr, Addr+AuxSym->Section.Length-1,
                       MinAddr, MaxAddr);
#endif

                if (Addr < MinAddr) {
                    MinAddr = Addr;
                }
                Addr += AuxSym->Section.Length-1;
                if (Addr > MaxAddr) {
                    MaxAddr = Addr;
                }
            }

            iCur += 1+CurSym->NumberOfAuxSymbols;
            CurSym += 1+CurSym->NumberOfAuxSymbols;
        }

        if (Lines > 0) {
            PSOURCE_ENTRY Src;
            PSOURCE_LINE SrcLine;
            ULONG iLine;

            // We have a filename and some linenumber information,
            // so create a SOURCE_ENTRY and fill it in.

            FileName = (LPSTR)(Symbol+1);
            FileNameLen = strlen(FileName);

            Src = (PSOURCE_ENTRY)MemAlloc(sizeof(SOURCE_ENTRY)+
                                          sizeof(SOURCE_LINE)*Lines+
                                          FileNameLen+1);
            if (Src == NULL) {
                goto EH_FreeSecLines;
            }

            Src->ModuleId = 0;
            Src->MinAddr = MinAddr;
            Src->MaxAddr = MaxAddr;
            Src->Lines = Lines;

            SrcLine = (PSOURCE_LINE)(Src+1);
            Src->LineInfo = SrcLine;

            // Now that we've got a place to put linenumber information,
            // retraverse the section symbols and grab COFF linenumbers
            // from the appropriate sections and format them into
            // the generic format.
            CurSym = SymAfterFile;
            iCur = iAfterFile;
            while (iCur < iNextFile) {
                if (IS_SECTION_SYM(CurSym) &&
                    SecLines[CurSym->SectionNumber-1] != NULL) {
                    PIMAGE_LINENUMBER CoffLine;

                    AuxSym = (PIMAGE_AUX_SYMBOL)(CurSym+1);
                    CoffLine = SecLines[CurSym->SectionNumber-1];

#ifdef DBG_COFF_LINES
                    DbgOut("    %d lines at %08X\n",
                           AuxSym->Section.NumberOfLinenumbers,
                           CoffLine);
#endif

                    for (iLine = 0;
                         iLine < AuxSym->Section.NumberOfLinenumbers;
                         iLine++)
                    {
                        SrcLine->Addr = CoffLine->Type.VirtualAddress+
                            mi->BaseOfDll;
                        SrcLine->Line = CoffLine->Linenumber;
                        CoffLine++;
                        SrcLine++;
                    }

                    SecLines[CurSym->SectionNumber-1] = CoffLine;
                }

                iCur += 1+CurSym->NumberOfAuxSymbols;
                CurSym += 1+CurSym->NumberOfAuxSymbols;
            }

            // Stick file name at the very end of the data block so
            // it doesn't interfere with alignment.
            Src->File = (LPSTR)SrcLine;
            memcpy(Src->File, FileName, FileNameLen+1);

            AddSourceEntry(mi, Src);

            // This routine is successful as long as it adds at least
            // one new source entry.
            Ret = TRUE;
        }

        // After the loops above iCur and CurSym refer to the next
        // file symbol, so update the loop counters from them.
        i = iCur;
        Symbol = CurSym;
    }

 EH_FreeSecLines:
    MemFree(SecLines);

    return Ret;
}

BOOL
AddLinesForOmfSourceModule(
    PMODULE_ENTRY mi,
    BYTE *Base,
    OMFSourceModule *OmfSrcMod,
    PVOID PdbModule
    )
{
    BOOL Ret;
    ULONG iFile;

    Ret = FALSE;

    for (iFile = 0; iFile < (ULONG)OmfSrcMod->cFile; iFile++) {
        OMFSourceFile *OmfSrcFile;
        BYTE OmfFileNameLen;
        LPSTR OmfFileName;
        PULONG OmfAddrRanges;
        OMFSourceLine *OmfSrcLine;
        ULONG iSeg;
        PSOURCE_ENTRY Src;
        PSOURCE_ENTRY Seg0Src;
        PSOURCE_LINE SrcLine;
        ULONG NameAllocLen;

        OmfSrcFile = (OMFSourceFile *)(Base+OmfSrcMod->baseSrcFile[iFile]);

        // The baseSrcLn array of offsets is immediately followed by
        // SVA pairs which define the address ranges for the segments.
        OmfAddrRanges = &OmfSrcFile->baseSrcLn[OmfSrcFile->cSeg];

        // The name length and data immediately follows the address
        // range information.
        OmfFileName = (LPSTR)(OmfAddrRanges+2*OmfSrcFile->cSeg)+1;
        OmfFileNameLen = *(BYTE *)(OmfFileName-1);

        // The compiler can potentially generate a lot of segments
        // per file.  The segments within a file have disjoint
        // address ranges as long as they are treated as separate
        // SOURCE_ENTRYs.  If all segments for a particular file get
        // combined into one SOURCE_ENTRY it leads to address range overlap
        // because of combining non-contiguous segments.  Allocating
        // a SOURCE_ENTRY per segment isn't that bad, particularly since
        // the name information only needs to be allocated in the first
        // entry for a file and the rest can share it.

        NameAllocLen = OmfFileNameLen+1;

        for (iSeg = 0; iSeg < (ULONG)OmfSrcFile->cSeg; iSeg++) {
            PULONG Off;
            PUSHORT Ln;
            ULONG iLine;
            PIMAGE_SECTION_HEADER sh;

            OmfSrcLine = (OMFSourceLine *)(Base+OmfSrcFile->baseSrcLn[iSeg]);

            Src = (PSOURCE_ENTRY)MemAlloc(sizeof(SOURCE_ENTRY)+
                                          sizeof(SOURCE_LINE)*OmfSrcLine->cLnOff+
                                          NameAllocLen);
            if (Src == NULL) {
                return Ret;
            }

            Src->ModuleId = (ULONG) (ULONG64) PdbModule;
            Src->Lines = OmfSrcLine->cLnOff;

            sh = &mi->SectionHdrs[OmfSrcLine->Seg-1];

            // Process raw segment limits into current addresses.
            Src->MinAddr = mi->BaseOfDll+sh->VirtualAddress+(*OmfAddrRanges++);
            Src->MaxAddr = mi->BaseOfDll+sh->VirtualAddress+(*OmfAddrRanges++);

            // Retrieve line numbers and offsets from raw data and
            // process them into current pointers.

            SrcLine = (SOURCE_LINE *)(Src+1);
            Src->LineInfo = SrcLine;

            Off = (ULONG *)&OmfSrcLine->offset[0];
            Ln = (USHORT *)(Off+OmfSrcLine->cLnOff);

            for (iLine = 0; iLine < OmfSrcLine->cLnOff; iLine++) {
                SrcLine->Line = *Ln++;
                SrcLine->Addr = (*Off++)+mi->BaseOfDll+sh->VirtualAddress;

                // Line symbol information names the IA64 bundle
                // syllables with 0,1,2 whereas the debugger expects
                // 0,4,8.  Convert.
                if (mi->MachineType == IMAGE_FILE_MACHINE_IA64 &&
                    (SrcLine->Addr & 3)) {
                    SrcLine->Addr = (SrcLine->Addr & ~3) |
                        ((SrcLine->Addr & 3) << 2);
                }
                
                SrcLine++;
            }

            if (iSeg == 0) {
                // Stick file name at the very end of the data block so
                // it doesn't interfere with alignment.
                Src->File = (LPSTR)SrcLine;
                memcpy(Src->File, OmfFileName, OmfFileNameLen);
                Src->File[OmfFileNameLen] = 0;

                // Later segments will share this initial name storage
                // space so they don't need to alloc their own.
                NameAllocLen = 0;
                Seg0Src = Src;
            } else {
                Src->File = Seg0Src->File;
            }

            AddSourceEntry(mi, Src);

            // This routine is successful as long as it adds at least
            // one new source entry.
            Ret = TRUE;
        }
    }

    return Ret;
}


VOID
FillLineInfo(
    PSOURCE_ENTRY Src,
    PSOURCE_LINE SrcLine,
    PIMAGEHLP_LINE64 Line
    )
{
    Line->Key = (PVOID)SrcLine;
    Line->LineNumber = SrcLine->Line;
    Line->FileName = Src->File;
    Line->Address = SrcLine->Addr;
}

PSOURCE_LINE
FindLineInSource(
    PSOURCE_ENTRY Src,
    DWORD64 Addr
    )
{
    int Low, Middle, High;
    PSOURCE_LINE SrcLine;

    Low = 0;
    High = Src->Lines-1;

    while (High >= Low) {
        Middle = (High <= Low) ? Low : (Low + High) >> 1;
        SrcLine = &Src->LineInfo[Middle];

#ifdef DBG_ADDR_SEARCH
        DbgOut("    Checking %4d:%x`%08X\n", Middle,
               (ULONG)(SrcLine->Addr>>32), (ULONG)SrcLine->Addr);
#endif

        if (Addr < SrcLine->Addr) {
            High = Middle-1;
        }
        else if (Middle < (int)Src->Lines-1 &&
                 Addr >= (SrcLine+1)->Addr) {
            Low = Middle+1;
        } else {
            PSOURCE_LINE HighLine;
            
            // Find the highest source line with this offset.
            // Source lines are sorted by offset so the highest
            // source line could be before or after this one.
            
            while (SrcLine > Src->LineInfo &&
                   (SrcLine - 1)->Addr == SrcLine->Addr) {
                SrcLine--;
            }
            HighLine = SrcLine;
            while (SrcLine < Src->LineInfo + Src->Lines - 1 &&
                   (++SrcLine)->Addr == HighLine->Addr) {
                if (SrcLine->Line > HighLine->Line) {
                    HighLine = SrcLine;
                }
            }
            return HighLine;
        }
    }

    return NULL;
}

PSOURCE_ENTRY
FindNextSourceEntryForAddr(
    PMODULE_ENTRY mi,
    DWORD64 Addr,
    PSOURCE_ENTRY SearchFrom
    )
{
    PSOURCE_ENTRY Src;

    Src = SearchFrom != NULL ? SearchFrom->Next : mi->SourceFiles;
    while (Src != NULL) {
        if (Addr < Src->MinAddr) {
            // Source files are kept sorted by increasing address so this
            // means that the address will not be found later and
            // we can stop checking.
            return NULL;
        } else if (Addr <= Src->MaxAddr) {
            // Found one.
            return Src;
        }
        Src = Src->Next;
    }

    return NULL;
}

BOOL
GetLineFromAddr(
    PMODULE_ENTRY mi,
    DWORD64 Addr,
    PDWORD Displacement,
    PIMAGEHLP_LINE64 Line
    )
{
    PSOURCE_ENTRY Src;
    DWORD Bias;
    DWORD64 srcAddr;

    if (mi == NULL) {
        return FALSE;
    }

    if (mi->dia)
        return diaGetLineFromAddr(mi, Addr, Displacement, Line);

    srcAddr = ConvertOmapToSrc( mi,
                               Addr,
                               &Bias,
                               (g.SymOptions & SYMOPT_OMAP_FIND_NEAREST) != 0
                               );

    if (srcAddr == 0) {
        return FALSE;
    }

    // We have successfully converted

    srcAddr += Bias;

    for (;;) {
        PSOURCE_ENTRY BestSrc;
        PSOURCE_LINE BestSrcLine;
        DWORD64 BestDisp;

        // Search through all the source entries that contain the given
        // address, looking for the line with the smallest displacement.

        BestDisp = 0xffffffffffffffff;
        BestSrc = NULL;
        Src = NULL;
        while (Src = FindNextSourceEntryForAddr(mi, srcAddr, Src)) {
            PSOURCE_LINE SrcLine;

#ifdef DBG_ADDR_SEARCH
            DbgOut("Found '%s' %d lines: %08I64X %08I64X for %08I64X\n",
                   Src->File, Src->Lines, Src->MinAddr, Src->MaxAddr, Addr);
#endif

            // Found a matching source entry, so look up the line
            // information.
            SrcLine = FindLineInSource(Src, srcAddr);
            if (SrcLine != NULL &&
                Addr-SrcLine->Addr < BestDisp) {
                BestDisp = Addr-SrcLine->Addr;

#ifdef DBG_ADDR_SEARCH
                DbgOut("  Best disp %I64X\n", BestDisp);
#endif

                BestSrc = Src;
                BestSrcLine = SrcLine;
                if (BestDisp == 0) {
                    break;
                }
            }
        }

        // Only accept displaced answers if there's no more symbol
        // information to load.
        if (BestSrc != NULL && BestDisp == 0) {
            FillLineInfo(BestSrc, BestSrcLine, Line);
            *Displacement = (ULONG)BestDisp;
            return TRUE;
        }

        return FALSE;
    }

    return FALSE;
}

PSOURCE_ENTRY
FindNextSourceEntryForFile(
    PMODULE_ENTRY mi,
    LPSTR FileStr,
    PSOURCE_ENTRY SearchFrom
    )
{
    PSOURCE_ENTRY Src;

    Src = SearchFrom != NULL ? SearchFrom->Next : mi->SourceFiles;
    while (Src != NULL)
    {
        if (SymMatchFileName(Src->File, FileStr, NULL, NULL))
        {
            return Src;
        }

        Src = Src->Next;
    }

    return NULL;
}

PSOURCE_ENTRY
FindPrevSourceEntryForFile(
    PMODULE_ENTRY mi,
    LPSTR FileStr,
    PSOURCE_ENTRY SearchFrom
    )
{
    PSOURCE_ENTRY Src;

    Src = SearchFrom != NULL ? SearchFrom->Prev : mi->SourceFilesTail;
    while (Src != NULL)
    {
        if (SymMatchFileName(Src->File, FileStr, NULL, NULL))
        {
            return Src;
        }

        Src = Src->Prev;
    }

    return NULL;
}

BOOL
FindLineByName(
    PMODULE_ENTRY mi,
    LPSTR FileName,
    DWORD LineNumber,
    PLONG Displacement,
    PIMAGEHLP_LINE64 Line
    )
{
    PSOURCE_ENTRY Src;
    BOOL TryLoad;
    BOOL AtOrGreater;

    if (mi == NULL)
    {
        return FALSE;
    }

    if (FileName == NULL)
    {
        // If no file was given then it's assumed that the file
        // is the same as for the line information passed in.
        FileName = Line->FileName;
    }

    // If the high bit of the line number is set
    // it means that the caller only wants lines at
    // or greater than the given line.
    AtOrGreater = (LineNumber & 0x80000000) != 0;
    LineNumber &= 0x7fffffff;
    
    if (mi->dia)
        if (diaGetLineFromName(mi, FileName, LineNumber, Displacement, Line))
            return TRUE;

    // We only lazy load here for symbols, and only if we're allowed to.
    TryLoad = mi->SymType == SymDia &&
        (g.SymOptions & SYMOPT_LOAD_LINES) != 0;

    for (;;)
    {
        ULONG Disp;
        ULONG BestDisp;
        PSOURCE_ENTRY BestSrc;
        PSOURCE_LINE BestSrcLine;

        //
        // Search existing source information for a filename match.
        // There can be multiple SOURCE_ENTRYs with the same filename,
        // so make sure and search them all for an exact match
        // before settling on an approximate match.
        //

        Src = NULL;
        BestDisp = 0x7fffffff;
        BestSrcLine = NULL;
        while (Src = FindNextSourceEntryForFile(mi, FileName, Src))
        {
            PSOURCE_LINE SrcLine;
            ULONG i;

            // Found a matching source entry, so look up the closest
            // line.  Line number info is sorted by address so the actual
            // line numbers can be in any order so we can't optimize
            // this linear search.

            SrcLine = Src->LineInfo;
            for (i = 0; i < Src->Lines; i++)
            {
                if (LineNumber > SrcLine->Line)
                {
                    if (AtOrGreater)
                    {
                        Disp = 0x7fffffff;
                    }
                    else
                    {
                        Disp = LineNumber-SrcLine->Line;
                    }
                }
                else
                {
                    Disp = SrcLine->Line-LineNumber;
                }

                if (Disp < BestDisp)
                {
                    BestDisp = Disp;
                    BestSrc = Src;
                    BestSrcLine = SrcLine;
                    if (Disp == 0)
                    {
                        break;
                    }
                }

                SrcLine++;
            }

            // If we found an exact match we can stop.
            if (BestDisp == 0)
            {
                break;
            }
        }

        // Only accept displaced answers if there's no more symbol
        // information to load.
        if (BestSrcLine != NULL && (BestDisp == 0 || !TryLoad))
        {
            FillLineInfo(BestSrc, BestSrcLine, Line);
            *Displacement = (LONG)(LineNumber-BestSrcLine->Line);
            return TRUE;
        }
        if (!TryLoad)
        {
            // There's no more line information to try and load so
            // we're out of luck.
            return FALSE;
        }

        TryLoad = FALSE;

        // There doesn't seem to be an easy way to look up a module by
        // filename.  It's possible to query by object filename, but
        // that can be much different from the source filename.
        // Just load the info all PDB modules.

        if (!diaAddLinesForAllMod(mi))
        {
            return FALSE;
        }

    }

    return FALSE;
}

#define LINE_ERROR 0xffffffff

ULONG
GetFileLineOffsets(
    PMODULE_ENTRY mi,
    LPSTR FileName,
    PDWORD64 Buffer,
    ULONG BufferLines
    )
{
    PSOURCE_ENTRY Src;
    ULONG HighestLine = 0;
    
    // This routine collects all line information in one pass so
    // there's no opportunity for lazy loading.  We have to
    // force lines to be loaded up front.
    if (mi->dia && (g.SymOptions & SYMOPT_LOAD_LINES) != 0) {
        if (!diaAddLinesForAllMod(mi)) {
            return LINE_ERROR;
        }
    }


    Src = NULL;
    while (Src = FindNextSourceEntryForFile(mi, FileName, Src)) {
        PSOURCE_LINE Line;
        ULONG i;
        ULONG Num;

        Line = Src->LineInfo;
        for (i = 0; i < Src->Lines; i++) {
            if (Line->Line > HighestLine) {
                HighestLine = Line->Line;
            }

            Num = Line->Line - 1;
            if (Num < BufferLines) {
                Buffer[Num] = Line->Addr;
            }

            Line++;
        }
    }

    return HighestLine;
}

ULONG
IMAGEAPI
SymGetFileLineOffsets64(
    IN  HANDLE                  hProcess,
    IN  LPSTR                   ModuleName,
    IN  LPSTR                   FileName,
    OUT PDWORD64                Buffer,
    IN  ULONG                   BufferLines
    )

/*++

Routine Description:

    This function locates the given file's line information
    and fills the given buffer with offsets for each
    line.  Buffer[Line - 1] is set to the offset for
    Line.  Buffer entries for lines that do not have information
    are left unchanged.

Arguments:

    hProcess            - Process handle, must have been previously registered
                          with SymInitialize.

    ModuleName          - Module name or NULL.

    FileName            - File name.

    Buffer              - Array of offsets to fill.

    BufferLines         - Number of entries in the Buffer array.

Return Value:

    0                   - No information was found.

    LINE_ERROR          - Failure during operation.  Call GetLastError to
                          discover the cause of the failure.

    Otherwise the return value is the highest line number found.

--*/

{
    PPROCESS_ENTRY      ProcessEntry;
    PMODULE_ENTRY       mi;
    ULONG               HighestLine = 0;
    PLIST_ENTRY         Next;

    __try {
        ProcessEntry = FindProcessEntry( hProcess );
        if (!ProcessEntry) {
            SetLastError( ERROR_INVALID_HANDLE );
            return LINE_ERROR;
        }

        if (ModuleName != NULL) {

            mi = FindModule(hProcess, ProcessEntry, ModuleName, TRUE);
            if (mi != NULL) {
                return GetFileLineOffsets(mi, FileName, Buffer, BufferLines);
            }

            SetLastError( ERROR_MOD_NOT_FOUND );
            return LINE_ERROR;
        }

        Next = ProcessEntry->ModuleList.Flink;
        if (Next) {
            while (Next != &ProcessEntry->ModuleList) {

                mi = CONTAINING_RECORD( Next, MODULE_ENTRY, ListEntry );
                Next = mi->ListEntry.Flink;

                if (!LoadSymbols(hProcess, mi, LS_QUALIFIED | LS_LOAD_LINES)) {
                    continue;
                }

                HighestLine = GetFileLineOffsets(mi, FileName, Buffer,
                                                 BufferLines);
                // This will break on lines found or LINE_ERROR.
                if (HighestLine > 0) {
                    break;
                }
            }
        }

    } __except (EXCEPTION_EXECUTE_HANDLER) {

        ImagepSetLastErrorFromStatus( GetExceptionCode() );
        return LINE_ERROR;

    }

    return HighestLine;
}

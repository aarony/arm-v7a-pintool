/*BEGIN_LEGAL 
INTEL CONFIDENTIAL
Copyright 2002-2005 Intel Corporation All Rights Reserved.

The source code contained or described herein and all documents
related to the source code (Material) are owned by Intel Corporation
or its suppliers or licensors. Title to the Material remains with
Intel Corporation or its suppliers and licensors. The Material may
contain trade secrets and proprietary and confidential information of
Intel Corporation and its suppliers and licensors, and is protected by
worldwide copyright and trade secret laws and treaty provisions. No
part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed, or disclosed in
any way without Intels prior express written permission.  No license
under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or
delivery of the Materials, either expressly, by implication,
inducement, estoppel or otherwise. Any license under such intellectual
property rights must be express and approved by Intel in writing.

Unless otherwise agreed by Intel in writing, you may not remove or
alter this notice or any other notice embedded in Materials by Intel
or Intels suppliers or licensors in any way.
END_LEGAL */
/* ===================================================================== */
/*
  @ORIGINAL_AUTHOR: Artur Klauser
*/

/* ===================================================================== */
/*! @file
 *  This file contains a guard tool against inadvertent or adversary
 *  attempts to modify program text.
 *  Note that self-modifying code will not run under this tool, as its
 *  attempts to execute modified program text will be aborted.
 *
 *  The tool works by making a backup copy of all text pages belonging to
 *  the application (and shared libraries) before the first instruction of
 *  the application is executed. Before jitting of every trace, we check
 *  (compare against backup copy) that none of the instructions belonging
 *  to the trace have been tampered with. If a trace is used out of the
 *  code cache, no such checking is done - the assumption is that
 *  only original code needs to be checked.
 *
 *  In Fedora core2, the kernel creates come code that is not part of an
 *  image. To make this tool work, we would also have to find that code
 *
 */

#include <iostream>
#include <string>
#include <map>

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>

#include "pin.H"

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<BOOL> KnobHelp(KNOB_MODE_WRITEONCE,         "pintool",
    "help", "0", "this help text");


/* ===================================================================== */

INT32 Usage()
{
    cerr <<
        "\n"
        "This pin tool guards against runtime program text modifications\n"
        "such as buffer overflows.\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();
    cerr << endl;

    return 1;
}

/* ===================================================================== */

class SANDBOX
{
  private:
    // types
    typedef map<const char *, const char *> AddrMap;

    // constants
    static const ADDRINT Kilo = 1024;
    static const ADDRINT Mega = Kilo * Kilo;
    static const ADDRINT PageSize = 4 * Kilo;
    static const ADDRINT PageMask = PageSize - 1;

    // members
    AddrMap _pages;

    // functions
    const char * Addr2Page(const char * addr)
    {
        return reinterpret_cast<const char *>(reinterpret_cast<ADDRINT>(addr) & ~PageMask);
    }
    ADDRINT Addr2Offset(const char * addr)
    {
        return reinterpret_cast<ADDRINT>(addr) & PageMask;
    }

    const char * AllocatePage(const char * page);
    VOID ProtectPage(const char * page);
    VOID RecordPage(const char * page);
    VOID RecordPageRange(const char * beginPage, const char * endPage);
    VOID RecordAddressRange(const char * beginAddr, const char * endAddr);

    VOID Error(string msg);

  public:
    VOID RecordIns(INS ins);
    VOID CheckAddressRange(const char * beginAddr, const char * endAddr);
};

static SANDBOX sandbox;

// report error
VOID SANDBOX::Error(string msg)
{
    if (msg.length() > 0 && msg[0] != '\n')
    {
        msg = "\n" + msg;
    }

    string::size_type pos = msg.length() > 1 ? msg.length() - 2 : 0;

    // prefix every line
    for (pos = msg.find_last_of('\n', pos);
         pos != string::npos;
         pos = (pos == 0 ? string::npos : msg.find_last_of('\n', pos-1)))
    {
        if (pos == msg.length() - 1)
        {
            msg += "Fence: \n";
        }
        else
        {
            msg.insert(pos + 1, "Fence: ");
        }
    }

    cerr << msg << endl << flush;
    fflush(NULL);

    exit(1);
}

// allocate a new page
const char * SANDBOX::AllocatePage(const char * page)
{
    
    const char * pageFrameStart = reinterpret_cast<const char *>
#if defined(TARGET_MAC)
    // Mac OS
        (mmap(0, PageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0));
#else
    // linux
        (mmap(0, PageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
#endif


    if (pageFrameStart == MAP_FAILED)
    {
        Error("Can't get page frame for page " + hexstr(page) + "\n" + strerror(errno));
    }

    _pages[page] = pageFrameStart;

    return pageFrameStart;
}

// protect a page read-only
VOID SANDBOX::ProtectPage(const char * page)
{
    int result = mprotect(const_cast<char *>(page), PageSize, PROT_READ);

    if (result != 0)
    {
        Error("Can't read-protect page " + hexstr(page) + "\n" + strerror(errno));
    }
}

// record one page
VOID SANDBOX::RecordPage(const char * page)
{
    const char * pageFrameStart = _pages[page];

    if (pageFrameStart != NULL)
    {
        return; // already recorded
    }

    pageFrameStart = AllocatePage(page);
    memcpy(const_cast<char *>(pageFrameStart), page, PageSize);
    ProtectPage(pageFrameStart);
}

// record pages in given range
VOID SANDBOX::RecordPageRange(const char * beginPage, const char * endPage)
{
    for (const char * page = beginPage; page <= endPage; page += PageSize)
    {
        RecordPage(page);
    }
}

// record bytes in given address range
VOID SANDBOX::RecordAddressRange(const char * beginAddr, const char * endAddr)
{
    const char * beginPage = Addr2Page(beginAddr);
    const char * endPage = Addr2Page(endAddr);

    RecordPageRange(beginPage, endPage);
}

// record the page(s) occupied by the instruction
VOID SANDBOX::RecordIns(INS ins)
{
    const ADDRINT beginAddr = INS_Address(ins);
    const ADDRINT endAddr = beginAddr + INS_Size(ins) - 1;

    RecordAddressRange(reinterpret_cast<const char *>(beginAddr), reinterpret_cast<const char *>(endAddr));
}

/* ===================================================================== */
// check bytes in given address range
VOID SANDBOX::CheckAddressRange(const char * beginAddr, const char * endAddr)
{
    const char * beginPage = Addr2Page(beginAddr);
    const char * endPage = Addr2Page(endAddr);

    for (const char * page = beginPage; page <= endPage; page += PageSize)
    {
        const char * pageEnd = page + PageSize - 1;
        const char * beginCheckAddr = beginAddr > page ? beginAddr : page;
        const char * endCheckAddr = endAddr < pageEnd ? endAddr : pageEnd;
        size_t size = endCheckAddr - beginCheckAddr + 1;

        const char * pageFrameStart = _pages[page];
        if (pageFrameStart == NULL)
        {
            // note: this might also trigger if PIN has not found all the code
            // of a routine
            Error("Instruction address range " + hexstr(beginCheckAddr) +
                    " - " + hexstr(endCheckAddr) + " was not found\n"
                  "during initial code discovery.\n" +
                  "\n"
                  "The application is trying to execute instructions which were not in\n"
                  "the original application.\n"
                  "This might be due to a buffer overflow.\n"
                  "\n"
                  "Terminating the application!\n");
        }
        const char * pageFrameBeginCheckAddr = pageFrameStart + Addr2Offset(beginCheckAddr);

        if (memcmp(beginCheckAddr, pageFrameBeginCheckAddr, size) != 0)
        {
            Error("Instruction address range " + hexstr(beginCheckAddr) +
                          " - " + hexstr(endCheckAddr) + " is corrupted.\n" +
                  "\n"
                  "The application code has been corrupted during execution.\n"
                  "This might be due to a buffer overflow.\n"
                  "\n"
                  "Terminating the application!\n");
        }
    }
}

/* ===================================================================== */

VOID Trace(TRACE trace, VOID *v)
{
    const INS beginIns = BBL_InsHead(TRACE_BblHead(trace));
    const INS endIns = BBL_InsTail(TRACE_BblTail(trace));
    const ADDRINT beginAddr = INS_Address(beginIns);
    const ADDRINT endAddr = INS_Address(endIns) + INS_Size(endIns) - 1;

    sandbox.CheckAddressRange(reinterpret_cast<const char *>(beginAddr), reinterpret_cast<const char *>(endAddr));
}

/* ===================================================================== */

VOID Image(IMG img, VOID * v)
{
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
            RTN_Open(rtn);
            
            for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
            {
                sandbox.RecordIns(ins);
            }

            RTN_Close(rtn);
        }
    }
}

/* ===================================================================== */

int main(int argc, CHAR *argv[])
{
    if (PIN_Init(argc,argv))
    {
        return Usage();
    }

    if (KnobHelp)
    {
        Usage();
        return 0;
    }
    
    TRACE_AddInstrumentFunction(Trace, 0);
    IMG_AddInstrumentFunction(Image, 0);

    PIN_StartProgram(); // Never returns
    
    return 0;
}

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
#include <signal.h>
#include <iostream>
#include "pin.H"

static BOOL HandleSig(INT32, CONTEXT *, BOOL hndlr, VOID *);
static BOOL ParseCmdLine(int, char **);
static void Usage();


// These parameters are set from the tool's command line

int Signal = SIGINT;    // Signal to intercept
unsigned Count = 1;     // Tool exits after intercepting signal this many times
BOOL PassToApp = FALSE; // Whether intercepted signals should be passed to application handler


int main(int argc, char **argv)
{
    PIN_Init(argc, argv);
    if (!ParseCmdLine(argc, argv))
        return 1;

    PIN_AddSignalInterceptFunction(Signal, HandleSig, 0);

    PIN_StartProgram();
    return 0;
}


static BOOL HandleSig(INT32 sig, CONTEXT *ctxt, BOOL hndlr, VOID *v)
{
    cerr << "Intercepting signal " << sig << endl;
    if (--Count == 0)
        exit(0);
    return PassToApp;
}


static BOOL ParseCmdLine(int argc, char **argv)
{
    // Skip over the Pin arguments.
    //
    int i;
    for (i = 0;  i < argc;  i++)
    {
        if (strcmp(argv[i], "-t") == 0 && (i+2 < argc))
        {
            i += 2;
            break;
        }
    }

    bool done = false;
    for (; i < argc && !done;  i++)
    {
        if (argv[i][0] != '-')
        {
            Usage();
            return FALSE;
        }

        switch (argv[i][1])
        {
          case 's':
          {
            if (i+1 >= argc)
            {
                Usage();
                return FALSE;
            }
            Signal = strtol(argv[i+1], 0, 10);
            if (Signal < 1 || Signal > 64)
            {
                cerr << "Invalid signal number: " << argv[i+i] << endl;
                Usage();
                return FALSE;
            }
            i++;
            break;
          }

          case 'c':
          {
            if (i+1 >= argc)
            {
                Usage();
                return FALSE;
            }
            unsigned long count = strtoul(argv[i+1], 0, 10);
            if (count == 0 || count > UINT_MAX)
            {
                cerr << "Invalid count, must be greater than zero: " << argv[i+1] << endl;
                Usage();
                return FALSE;
            }
            Count = static_cast<unsigned>(count);
            i++;
            break;
          }

          case 'p':
          {
            PassToApp = TRUE;
            break;
          }

          case '-':
          {
            done = TRUE;
            break;
          }

          default:
          {
            cerr << "Invalid argument: " << argv[i] << endl;
            Usage();
            return FALSE;
          }
        }
    }

    return TRUE;
}


static void Usage()
{
    cerr << endl;
    cerr << "pin -t intercepttool [-s <signal>] [-c <count>] [-p]" << endl;
    cerr << "    -s <signal>  - Specifies signal to intercept (default is SIGINT)" << endl;
    cerr << "    -c <count>   - Tool exits after intercepting signal this many times (default 1)" << endl;
    cerr << "    -p           - Tool forwards signal to application after intercepting" << endl;
    cerr << endl;
}

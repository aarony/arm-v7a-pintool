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
#include <iostream>

#include "pin.H"
#include "instlib.H"
#include "bimodal.H"
#include "time_warp.H"

#define MAXPP 10
LOCALVAR BIMODAL bimodal;

using namespace INSTLIB; 

#if defined(TARGET_IA32) || defined(TARGET_IA32E)
TIME_WARP tw;
#endif



class PPSTAT{
    public:
        // FIXME: Avoiding floating point arithmatic in analysis routines
        // double ppWeight;
        UINT32 ppWeightTimesThousand;
        UINT64 ppMispredicts;
        UINT64 ppInstructions;
};

class PPINFO
{
    public:
        UINT64 startIcount, startMispredicts;
        UINT32 currentpp;
        PPSTAT ppstats[MAXPP+1];
};

LOCALVAR PPINFO ppinfo;
LOCALVAR ofstream *outfile;


using namespace INSTLIB;

// Track the number of instructions executed
ICOUNT icount;

// Contains knobs and instrumentation to recognize start/stop points
CONTROL control;
/* ===================================================================== */

VOID Handler(CONTROL_EVENT ev, VOID * v, CONTEXT * ctxt, VOID * ip, VOID * tid)
{
    std::cout << "ip: " << ip << " Instructions: "  << icount.Count() << " ";

    switch(ev)
    {
      case CONTROL_START:
        std::cout << "Start" << endl;
        if(control.PinPointsActive())
        {
            std::cout << "PinPoint: " << control.CurrentPp() << endl;
            UINT32 pp = control.CurrentPp();
            ASSERTX( pp <= MAXPP);
            ppinfo.ppstats[pp].ppWeightTimesThousand = control.CurrentPpWeightTimesThousand();
            ppinfo.currentpp = pp; 
        }
        break;

      case CONTROL_STOP:
        std::cout << "Stop" << endl;
        if(control.PinPointsActive())
        {
            std::cout << "PinPoint: " << control.CurrentPp() << endl;
            UINT64 mispredicts = bimodal.Mispredicts() - ppinfo.startMispredicts;
            UINT64 instructions = icount.Count() - ppinfo.startIcount;
    
            UINT32 pp = ppinfo.currentpp;
            ppinfo.ppstats[pp].ppMispredicts = mispredicts;
            ppinfo.ppstats[pp].ppInstructions = instructions;
        }
        break;

      default:
        ASSERTX(false);
        break;
    }
}
    
INT32 Usage()
{
    cerr <<
        "This pin tool is a simple branch predictor \n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

LOCALFUN VOID Fini(int n, void *v)
{
    *outfile << endl;
    double whole_MPKI = 1000.0 * (double)bimodal.Mispredicts()/icount.Count();
    *outfile << "Whole-program MPKI = " << whole_MPKI << dec << endl;
    if (control.PinPointsActive())
    {
        UINT32 NumPp = control.NumPp();
        double predicted_MPKI = 0.0;
        *outfile << "PP #," << " %Weight," << " MPKI" << endl;
        for (UINT32 p = 1; p <= NumPp ; p++)
        {
            double  weight = (double) ppinfo.ppstats[p].ppWeightTimesThousand/1000.0;
            double  mpki = (double)ppinfo.ppstats[p].ppMispredicts*1000/ppinfo.ppstats[p].ppInstructions;
            *outfile << dec << p << ", "  << weight << ", " << mpki << endl;
            predicted_MPKI +=  (double) weight*mpki/100.0;
        }
        *outfile << "Predicted MPKI = " << predicted_MPKI << dec << endl;
    }
}

int main(int argc, char *argv[])
{
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    icount.Activate();

#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    tw.CheckKnobs(0);
#endif

    // Activate alarm, must be done before PIN_StartProgram
    control.CheckKnobs(Handler, 0);


    outfile = new ofstream("bimodal.out");

    PIN_AddFiniFunction(Fini, 0);
    PIN_StartProgram();
}

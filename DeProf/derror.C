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
#include <pin.H>
#include <map>
#include <fstream>
#include <iostream>

class REF
{
  public:
    REF(UINT64 global, UINT64 nonglobal) 
    {
        _global = global;
        _nonglobal = nonglobal;
    };
    BOOL PredictNonGlobal() const;
    UINT64 Count() const 
    {
        return _global + _nonglobal;
    };
    
    
    UINT64 _global;
    UINT64 _nonglobal;
};

class PROFILE
{
  private:
    typedef map<string, REF> REFS;
    
  public:
    VOID ReadFile(char * filename);
    UINT64 SumRefs();
    VOID NonGlobalPredicts(PROFILE * fullProfile, UINT64 * correct, UINT64 * incorrect, UINT64 * predicted);
    
    REFS _refs;
    UINT32 _tracesCodeCacheSize;
    UINT32 _expiredTracesCodeCacheSize;
    float _cpuTime;
};

BOOL REF::PredictNonGlobal() const
{
    return (float)_global/(_global + _nonglobal) < .01;
}

VOID PROFILE::ReadFile(CHAR * filename)
{
    ifstream infile(filename);

    if (!infile)
    {
        cerr << "Could not open " << filename << endl;
        exit(1);
    }

    while(true)
    {
        string command;
        
        infile >> command;

        if (command == "BeginProfile")
        {
            break;
        }
        else if (command == "TracesCodeCacheSize")
        {
            infile >> _tracesCodeCacheSize;
        }
        else if (command == "ExpiredTracesCodeCacheSize")
        {
            infile >> _expiredTracesCodeCacheSize;
        }
        else if (command == "CpuTime")
        {
            infile >> _cpuTime;
        }
        else
        {
            cerr << "Unknown command " << command << endl;
            exit(1);
        }
    }

    while(true)
    {
        string address;
        UINT64 global;
        UINT64 nonglobal;
        
        infile >> address;

        if (infile.eof())
            break;
        
        infile >> global;
        infile >> nonglobal;

        _refs.insert(pair<string,REF>(address,REF(global,nonglobal)));
    }
}

UINT64 PROFILE::SumRefs()
{
    UINT64 numRefs = 0;
    
    for (REFS::const_iterator ri = _refs.begin(); ri != _refs.end(); ri++)
    {
        numRefs += ri->second.Count();
    }

    return numRefs;
}

    
// The number of dynamic times we predicted non-global and were wrong
// Use the fullprofile for the correct/incorrect counts
// also compute number of global predicts using our count
VOID PROFILE::NonGlobalPredicts(PROFILE * fullProfile, UINT64 * correct, UINT64 * incorrect, UINT64 * predicted)
{
    *correct = 0;
    *incorrect = 0;
    *predicted = 0;
    
    for (REFS::const_iterator ri = _refs.begin(); ri != _refs.end(); ri++)
    {
        string address = ri->first;
        const REF & partialRef = ri->second;
        
        if (partialRef.PredictNonGlobal())
        {
            *predicted += partialRef.Count();
            
            REFS::iterator ri = fullProfile->_refs.find(address);

            // Nothing in the reference? It is not a problem.
            if (ri == fullProfile->_refs.end())
                continue;

            REF & fullRef = ri->second;
            
            if (fullRef.PredictNonGlobal())
                *correct += fullRef.Count();
            else
                *incorrect += fullRef.Count();
        }
    }
}

    
int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        fprintf(stderr,"Usage: %s <fullprofile> <partialprofile>\n",argv[0]);
        exit(1);
    }

    PROFILE fullProfile, partialProfile;
    fullProfile.ReadFile(argv[1]);
    partialProfile.ReadFile(argv[2]);

    //UINT64 fullNumRefs = fullProfile.SumRefs();

    UINT64 partialCorrect, partialIncorrect, partialPredicts;
    partialProfile.NonGlobalPredicts(&fullProfile, &partialCorrect, &partialIncorrect, &partialPredicts);

    UINT64 fullCorrect, fullIncorrect, fullPredicts;
    fullProfile.NonGlobalPredicts(&fullProfile, &fullCorrect, &fullIncorrect, &fullPredicts);

    cout << "CpuTime " << partialProfile._cpuTime << endl;
    cout << "ExpiredTraces " << partialProfile._expiredTracesCodeCacheSize << " " << partialProfile._tracesCodeCacheSize
         << " " << (float)partialProfile._expiredTracesCodeCacheSize/(float)partialProfile._tracesCodeCacheSize << endl;
    cout << "NonGlobalCoverage " << partialCorrect << " " << fullPredicts << " " << (float)partialCorrect/(float)fullPredicts << endl;
    cout << "NonGlobalMispredict " << partialIncorrect << " " << fullPredicts << " " << (float)partialIncorrect/(float)fullPredicts << endl;
}

    

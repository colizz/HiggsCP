#ifndef _HiggsTruthAnaTool_hh_
#define _HiggsTruthAnaTool_hh_

#include <Utils.hh>
#include <marlin/Processor.h>
#include <iostream>
#include <vector>
#include <utility>
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/MCParticle.h>

using namespace std;
using namespace lcio;
using namespace marlin;

int IsTruthHtt (LCCollection* col_MCP) ;
std::pair<enum TauMode, enum TauMode> GetTruthTauDecayMode (LCCollection* col_MCP);

#endif



#ifndef _Utils_hh_
#define _Utils_hh_

#include <marlin/Processor.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <string>

char* TLorentzVector2char(TVector3 &t, double t4);
char* TLorentzVector2char(TLorentzVector &t);
std::string PdgId2string(int pdgId);

#endif
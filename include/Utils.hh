#ifndef _Utils_hh_
#define _Utils_hh_

#include <marlin/Processor.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <string>

enum TauMode {E=-1, Mu=-2, Pi=1, PiP0=2, Pi2P0=3, Pi3P0=4, PiPiPi=11, PiPiPiP0=12, PiPiPi2P0=13, PiPiPi3P0=14, Else=100, UndefMode=-99};

char* TLorentzVector2char(TVector3 &t, double t4);
char* TLorentzVector2char(TLorentzVector &t);
std::string PdgId2string(int pdgId);

#endif
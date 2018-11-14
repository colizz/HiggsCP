#include <HiggsTruthAnaTool.hh>
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

int IsTruthHtt (LCCollection* col_MCP) {
    
    int nMCP = col_MCP->getNumberOfElements();
    int HDecayTo = -99;
    for (int i=0; i<nMCP; i++) {
        MCParticle* mp = dynamic_cast<MCParticle*>(col_MCP->getElementAt(i));
        if ( mp->getPDG()==25 ){
            if (mp->getDaughters()[0]->getPDG()==25)  continue;
            HDecayTo = abs( mp->getDaughters()[0]->getPDG() );
            break;
        }
    }
    return (HDecayTo==15);
    
}

std::pair<enum TauMode, enum TauMode> GetTruthTauDecayMode (LCCollection* col_MCP) {
    
    int nMCP = col_MCP->getNumberOfElements();
    MCParticle *TauP=nullptr, *TauM=nullptr;
    for (int i=nMCP>50?50:(nMCP-1) ; i >= 0; i--) { 
    // find htt event from bottom to top, find where tau+/- appears the last time
        MCParticle* mp = dynamic_cast<MCParticle*>(col_MCP->getElementAt(i));
        if (abs(mp->getPDG()) == 15 && mp->getDaughters().size() > 1) {
            if ( mp->getPDG() == 15 && TauM==nullptr )
                TauM = mp;
            if ( mp->getPDG() == -15 && TauP==nullptr )
                TauP = mp;
        }
        if(TauM!=nullptr && TauP!=nullptr)  break;
    }
    if(TauM==nullptr || TauP==nullptr)  return make_pair(UndefMode, UndefMode);
    // find tau+/- decay mode. For each mode, identify muon/elec/pions... as decay product
    TauMode modeTauP = UndefMode,  modeTauM = UndefMode;
    vector<int> tdecay; // save PDG of decay objects
    vector<MCParticle*> tptr; // save pointer
    for (int l=0; l<=1; l++){
        MCParticle** Tau;
        TauMode* modeTau;
        if(l==0){  Tau=&TauM; modeTau=&modeTauM; }
        else    {  Tau=&TauP; modeTau=&modeTauP; }
        
        tdecay.clear();
        tptr.clear();
        for (unsigned ii=0; ii < (*Tau)->getDaughters().size(); ii++) {
            MCParticle* daup = (*Tau)->getDaughters()[ii]; // ii-th decay object
            if ( abs( daup->getPDG() )==213 || abs( daup->getPDG() )==113 || abs( daup->getPDG() )==223 || abs( daup->getPDG() )==20213){// rho+-0 omega(223) a1+(20213)
                for(unsigned iii=0; iii<daup->getDaughters().size(); iii++){
                    if(daup->getDaughters()[iii]->getPDG()!=22) {// not gamma
                        tdecay.push_back( abs( daup->getDaughters()[iii]->getPDG() ) );
                        tptr.push_back( daup->getDaughters()[iii] );
                    }
                }
            }
            else if ( daup->getPDG()!=22 ) {
                tdecay.push_back( abs( daup->getPDG() ) );
                tptr.push_back( daup );
            }
        }
        
        if (tdecay.size() > 1) // sort tdecay and tptr in increasing order
            for (unsigned ii=0; ii<tdecay.size()-1; ii++)
                for (unsigned jj=tdecay.size()-1; jj>ii; jj--)
                    if (tdecay[jj] < tdecay[jj-1]){
                        swap(tdecay[jj], tdecay[jj-1]);
                        swap(tptr[jj], tptr[jj-1]);
                    }
        
        // 1 prong
        if(tdecay.size()==3 && tdecay[0]==11 && tdecay[1]==12 && tdecay[2]==16 ){
            *modeTau = E; // ta- > vt e- ve
        }
        else if(tdecay.size()==3 && tdecay[0]==13 && tdecay[1]==14 && tdecay[2]==16 ){
            *modeTau = Mu; // ta- > vt mu- vm-
        }
        else if(tdecay.size()==2 && tdecay[0]==16 && tdecay[1]==211){
            *modeTau = Pi; // ta- > vt pi-
        }
        else if(tdecay.size()==3 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==211){
            *modeTau = PiP0; // ta- > vt pi- pi0
        }
        else if(tdecay.size()==4 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==111 && tdecay[3]==211){
            *modeTau = Pi2P0; // ta- > vt pi- pi0 pi0
        }
        else if(tdecay.size()==5 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==111 && tdecay[3]==111 && tdecay[4]==211){
            *modeTau = Pi3P0; // ta- > vt pi- pi0 pi0 pi0
        }
        
        // 3 prong
        else if(tdecay.size()==4 && tdecay[0]==16 && tdecay[1]==211 && tdecay[2]==211 && tdecay[3]==211){
            *modeTau = PiPiPi; // ta- > 3h+-
        }
        else if(tdecay.size()==5 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==211 && tdecay[3]==211 && tdecay[4]==211){
            *modeTau = PiPiPiP0; // ta- > vt 3h+- pi0
        }
        else if(tdecay.size()==6 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==111 && tdecay[3]==211 && tdecay[4]==211 && tdecay[5]==211){
            *modeTau = PiPiPi2P0; // ta- > vt 3h+- pi0 pi0
        }
        else if(tdecay.size()==7 && tdecay[0]==16 && tdecay[1]==111 && tdecay[2]==111 && tdecay[3]==111 && tdecay[4]==211 && tdecay[5]==211 && tdecay[6]==211){
            *modeTau = PiPiPi3P0; // ta- > vt 3h+- pi0 pi0 pi0
        }
        else  *modeTau = Else; //others (kaon)
    }
    return  make_pair(modeTauM, modeTauP); 

}
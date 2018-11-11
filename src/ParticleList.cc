#include <ParticleList.hh>
#include <Utils.hh>
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCFloatVec.h>
#include <EVENT/MCParticle.h>
#include <EVENT/ReconstructedParticle.h>
#include <IMPL/MCParticleImpl.h>
#include <values.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <EVENT/LCFloatVec.h>
#include <EVENT/LCParameters.h>
#include <stdexcept>
#include <TFile.h> 
#include <TTree.h>
#include <TVector3.h>
#include <TRandom.h>
#include <Rtypes.h> 
#include <sstream>        
#include <cmath>
#include <vector>
#include <TMath.h>
#include "TLorentzVector.h"
#include <fstream>

using namespace std;

ParticleList a_ParticleList_instance;

ParticleList::ParticleList()
    : Processor("ParticleList"),
    _output(0)
{
    _description = "Print particlelist" ;

    registerProcessorParameter( "MCCollectionName" , 
            "The name of the MC collection" ,
            _MCCollectionName ,
            _MCCollectionName);

    registerProcessorParameter( "RecoCollectionName" , 
            "The name of reco collection" ,
            _recoCollectionName ,
            _recoCollectionName);
}

void ParticleList::init() {

    printParameters();
}


void skip(int i, ofstream* fout){
    fout->width(i);fout->setf(std::ios::left);
}
void line(int i, ofstream* fout){
    for(int j=0;j<i;j++) *fout<<"-";
    *fout<<endl;
}
void ParticleList::processEvent( LCEvent * evtP ) 
{        

    if (evtP) {
        try{            
            LCCollection* col_MCP = evtP->getCollection( _MCCollectionName.c_str() ) ;
            LCCollection* col_RecoP = evtP->getCollection( _recoCollectionName.c_str() );
            
            int _nMCP = col_MCP->getNumberOfElements();
            int _nRecoP=col_RecoP->getNumberOfElements();    
            
            _eventNr=evtP->getEventNumber();
            
            //fout<<"++++++++++++++++++++ MCP "<<_eventNr<<"th event ++++++++++++++++++++"<<endl;
            
            ofstream fout("out.particle_list.txt");
            cout<<"++++++ Evt "<<_eventNr<<"\t"<<_nMCP<<":"<<_nRecoP<<endl;
            fout<<"++++++ Evt "<<_eventNr<<"\t"<<_nMCP<<":"<<_nRecoP<<endl;
            
            std::vector<ReconstructedParticle *> ptc_reco;
            std::vector<MCParticle *> ptc;
            for(int i=0;i<_nRecoP;i++)
                ptc_reco.push_back( dynamic_cast<ReconstructedParticle*>(col_RecoP->getElementAt(i)) );
            for(int i=0;i<_nMCP;i++)
                ptc.push_back( dynamic_cast<MCParticle*>(col_MCP->getElementAt(i)) );
             
            
            /////////// Reconstructed particle
            line(145,&fout);
            skip(6,&fout); fout<<"j";
            //skip(6); fout<<"->Ptc";
            skip(14,&fout); fout<<"PID";
            skip(12,&fout); fout<<"M";
            skip(12,&fout); fout<<"E";
            skip(12,&fout); fout<<"PT";
            skip(12,&fout); fout<<"Pz";
            skip(12,&fout); fout<<"M_cal";
            skip(12,&fout); fout<<"TrkNum";
            skip(12,&fout); fout<<"D0";
            skip(12,&fout); fout<<"Z0";
            // skip(12,&fout); fout<<"X";
            // skip(12,&fout); fout<<"Y";
            // skip(12,&fout); fout<<"Z";
            fout<<endl;
            line(145,&fout);
            for(int j = 0; j < _nRecoP; ++j){
                
                skip(6,&fout); fout<<j;
                // for(int i = 0; i < branchParticle->GetEntriesFast(); ++i)
                    // if((GenParticle *) trk->Particle.GetObject() == branchParticle->At(i)) {
                        // skip(6,&fout); fout<<i; 
                        // trk_ptc.push_back(i);
                        // break;
                    // }
                skip(6,&fout); fout<<ptc_reco[j]->getType();
                skip(8,&fout); fout<<PdgId2string(ptc_reco[j]->getType());
                
                skip(12,&fout); fout<<std::setprecision(4)<<ptc_reco[j]->getMass();
                skip(12,&fout); fout<<ptc_reco[j]->getEnergy();
                //momentum
                TVector3 P=ptc_reco[j]->getMomentum();
                skip(12,&fout); fout<<sqrt(P.x()*P.x()+P.y()*P.y());
                skip(12,&fout); fout<<P.z();
                skip(12,&fout); fout<<sqrt (pow(ptc_reco[j]->getEnergy(),2)-P.Mag2() );
                skip(12,&fout); fout<<ptc_reco[j]->getTracks().size();
                if(ptc_reco[j]->getTracks().size()>0)
                    for(unsigned k=0; k<ptc_reco[j]->getTracks().size(); k++){
                        skip(12,&fout); fout<<ptc_reco[j]->getTracks()[k]->getD0();
                        skip(12,&fout); fout<<ptc_reco[j]->getTracks()[k]->getZ0();
                        fout<<"//";
                    }
                //start vertex
                // EVENT::Vertex* vtx=ptc_reco[j]->getStartVertex();
                // skip(12,&fout); fout<<vtx->getPosition().x();
                // skip(12,&fout); fout<<vtx->getPosition().y();
                // skip(12,&fout); fout<<vtx->getPosition().z();
                fout<<endl;
            }
        
            /////////////MC particle
            
            line(145,&fout);
            skip(6,&fout); fout<<"i";
            skip(14,&fout); fout<<"PID";
            skip(6,&fout); fout<<"Status";
            skip(10,&fout); fout<<"M";
            skip(8,&fout); fout<<"Ch";
            skip(10,&fout); fout<<"E";
            skip(10,&fout); fout<<"PT";
            skip(10,&fout); fout<<"Pz";
            skip(10,&fout); fout<<"Px";
            skip(10,&fout); fout<<"Py";
            skip(10,&fout); fout<<"Rapidity";
            skip(10,&fout); fout<<"T";
            skip(10,&fout); fout<<"St.r";
            skip(10,&fout); fout<<"St.z";
            skip(10,&fout); fout<<"End.r";
            skip(10,&fout); fout<<"End.z";
            skip(10,&fout); fout<<"Parents // Daus";
            fout<<endl;
            line(145,&fout);
            for(int i = 0; i < _nMCP; ++i){
                // GenParticle * ptc = branchParticle->At(i);
                // for(int k=0; k<trk_ptc.size(); k++){
                    // if(i==trk_ptc[k]) {fout<<"*"; break;}
                    // if(k==trk_ptc.size()-1){ //no track :(
                        // if(ptc->Status==1) fout<<"?";
                        // else fout<<" ";
                    // }
                // }
                skip(6,&fout); fout<<i;
                skip(6,&fout); fout<<ptc[i]->getPDG();
                skip(8,&fout); fout<<PdgId2string(ptc[i]->getPDG());
                skip(6,&fout); fout<<ptc[i]->getGeneratorStatus();
                skip(10,&fout); fout<<std::setprecision(4)<<ptc[i]->getMass();
                skip(8,&fout); fout<<ptc[i]->getCharge();
                skip(10,&fout); fout<<ptc[i]->getEnergy();
                
                //momentum
                TVector3 P=ptc[i]->getMomentum();
                // float P0=sqrt(P.x()*P.x()+P.y()*P.y()+P.z()*P.z());
                skip(10,&fout); fout<<sqrt(P.x()*P.x()+P.y()*P.y());
                skip(10,&fout); fout<<P.z();
                skip(10,&fout); fout<<P.x();
                skip(10,&fout); fout<<P.y();
                skip(10,&fout); fout<<P.PseudoRapidity();
                
                //vertex
                TVector3 vtx=ptc[i]->getVertex(), endp=ptc[i]->getEndpoint();
                skip(10,&fout); fout<<ptc[i]->getTime();
                skip(10,&fout); fout<<vtx.Perp();
                skip(10,&fout); fout<<vtx.Z();
                skip(10,&fout); fout<<endp.Perp();
                skip(10,&fout); fout<<endp.Z();
                
                //write parents and daus
                if(ptc[i]->getParents().size()>0){
                    int npa=ptc[i]->getParents().size();
                    for(int j=0;j<npa;j++)
                        for(int k=0;k<_nMCP;k++)
                            if(ptc[i]->getParents()[j]==ptc[k]){
                                fout<<k<<" ";
                                break;
                            }
                }
                fout<<"// ";
                if(ptc[i]->getDaughters().size()>0){
                    int ndau=ptc[i]->getDaughters().size();
                    for(int j=0;j<ndau;j++)
                        for(int k=0;k<_nMCP;k++)
                            if(ptc[i]->getDaughters()[j]==ptc[k]){
                                fout<<k<<" ";
                                break;
                            }
                }
                fout<<endl;
            }
            
            ////////////////write f_graph/////////////
            fout<<endl<<"\\"<<endl<<_nMCP<<endl;
            for(int i = 0; i < _nMCP; ++i){
                fout<<i<<" "<<ptc[i]->getPDG()<<" ";
                fout<<ptc[i]->getDaughters().size()<<" ";
                if(ptc[i]->getDaughters().size()>0){
                    int ndau=ptc[i]->getDaughters().size();
                    for(int j=0;j<ndau;j++)
                        for(int k=0;k<_nMCP;k++)
                            if(ptc[i]->getDaughters()[j]==ptc[k]){
                                fout<<k<<" ";
                                break;
                            }
                }
                fout<<endl;
            }
            fout.close();

        }        
        catch (lcio::DataNotAvailableException err) { }
    } 
}    

void ParticleList::end() { }




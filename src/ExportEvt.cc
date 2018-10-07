#include <ExportEvt.hh>
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
#include <TRandom.h>
#include <Rtypes.h> 
#include <sstream>		
#include <cmath>
#include <vector>
#include <TMath.h>
#include <fstream>
#include <TSystem.h>

using namespace std;

ExportEvt a_ExportEvt_instance;

ExportEvt::ExportEvt()
	: Processor("ExportEvt"),
	_output(0)
{
	_description = "Print MC Truth" ;

	_treeFileName="MCTruth.root";
	registerProcessorParameter( "TreeOutputFile" , 
			"The name of the file to which the ROOT tree will be written" ,
			_treeFileName ,
			_treeFileName);

	_treeName="Marlin";
	registerProcessorParameter( "TreeName" , 
			"The name of the ROOT tree" ,
			_treeName ,
			_treeName);

	_overwrite=0;
	registerProcessorParameter( "OverwriteFile" , 
			"If zero an already existing file will not be overwritten." ,
			_overwrite ,
			_overwrite);

}

void ExportEvt::init() {

	printParameters();

	TFile *tree_file=new TFile(_treeFileName.c_str(),(_overwrite ? "RECREATE" : "UPDATE"));

	if (!tree_file->IsOpen()) {
		delete tree_file;
		tree_file=new TFile(_treeFileName.c_str(),"NEW");
	}

	_outputTree = new TTree(_treeName.c_str(),_treeName.c_str());
	_outputTree->Branch("nFile", &_nfile, "nFile/I");
	_outputTree->Branch("nEvt", &_eventNr, "nEvt/I");
	_outputTree->Branch("nMCP", &_nMCP, "nMCP/I");
	_outputTree->Branch("nRecoP", &_nRecoP, "nRecoP/I");
	
	
	//_outputTree->Branch("m_i", "vector<int>", &m_i);
	_outputTree->Branch("m_PDG", "vector<int>", &m_PDG);
	_outputTree->Branch("m_Status", "vector<int>", &m_Status);
	_outputTree->Branch("m_Charge", "vector<int>", &m_Charge);
	_outputTree->Branch("m_M", "vector<float>", &m_M);
	_outputTree->Branch("m_E", "vector<float>", &m_E);
	_outputTree->Branch("m_P", "vector<TVector3>", &m_P); //TVector3
	// _outputTree->Branch("m_PT", "vector<float>", &m_PT);
	// _outputTree->Branch("m_Eta", "vector<float>", &m_Eta);
	_outputTree->Branch("m_Vertex", "vector<TVector3>", &m_Vertex); //TVector3
	_outputTree->Branch("m_Endpoint", "vector<TVector3>", &m_Endpoint); //TVector3
	_outputTree->Branch("m_T", "vector<float>", &m_T);
	_outputTree->Branch("m_Daughter", "vector< vector<int> >", &m_Daughter); 
	_outputTree->Branch("m_Parent", "vector< vector<int> >", &m_Parent); 
	
	
	//_outputTree->Branch("r_j", "vector<int>", &r_j);
	_outputTree->Branch("r_PDG", "vector<int>", &r_PDG);
	_outputTree->Branch("r_Charge", "vector<int>", &r_Charge);
	_outputTree->Branch("r_M", "vector<float>", &r_M);
	_outputTree->Branch("r_E", "vector<float>", &r_E);
	_outputTree->Branch("r_P", "vector<TVector3>", &r_P); //TVector3
	// _outputTree->Branch("r_PT", "vector<float>", &r_PT);
	// _outputTree->Branch("r_Eta", "vector<float>", &r_Eta);
	_outputTree->Branch("r_iMC", "vector< vector<int> >", &r_iMC);
	//track
	_outputTree->Branch("r_D0", "vector< vector<float> >", &r_D0);
	_outputTree->Branch("r_Phi", "vector< vector<float> >", &r_Phi);
	_outputTree->Branch("r_Z0", "vector< vector<float> >", &r_Z0);
	_outputTree->Branch("r_Chi2", "vector< vector<float> >", &r_Chi2);
	_outputTree->Branch("r_sigmaD0", "vector< vector<float> >", &r_sigmaD0);
	_outputTree->Branch("r_sigmaZ0", "vector< vector<float> >", &r_sigmaZ0);
	
}

string pdgcode(int x)
{
	string bar="";
	string type="?";
	if(x<0) bar="(-)";
	switch(abs(x)){
		case 1: type="d";break;
		case 2: type="u";break;
		case 3: type="s";break;
		case 4: type="c";break;
		case 5: type="b";break;
		case 6: type="t";break;
		case 7: type="b'";break;
		case 8: type="t'";break;
		case 11: type="e-";break;
		case 12: type="ve";break;
		case 13: type="mu-";break;
		case 14: type="vm";break;
		case 15: type="ta-";break;
		case 16: type="vt";break;
		case 17: type="ta-'";break;
		case 18: type="vt'";break;
		case 21: type="g";break;
		case 22: type="gm";break;
		case 23: type="Z0";break;
		case 24: type="W+";break;
		case 25: type="h0";break;
		case 211: type="pi+";break;
		case 311: type="K0";break;
		case 321: type="K+";break;
		case 411: type="D+";break;
		case 421: type="D0";break;
		case 511: type="B0";break;
		case 521: type="B+";break;
		case 111: type="pi0";break;
		case 221: type="eta";break;
		case 213: type="rho+";break;
		case 113: type="rho0";break;
		case 2112: type="n0";break;
		case 2212: type="p+";break;
		default: break;
	}
	return bar+type;
}

void ExportEvt::processEvent( LCEvent * evtP ) 
{		

	if (evtP) {		
		try{			
			//gSystem->Load("../lib/loader_C.so");
			LCCollection* col_MCP = evtP->getCollection( "MCParticle" ) ;
			LCCollection* col_RecoP = evtP->getCollection( "ArborPFOs" );
			_nMCP = col_MCP->getNumberOfElements();
			_nRecoP=col_RecoP->getNumberOfElements();	
			_eventNr=evtP->getEventNumber();
			if(_eventNr%10 == 0) cout<<"Evt "<<_eventNr<<endl;
			
			
			std::vector<ReconstructedParticle *> ptc_reco;
			std::vector<MCParticle *> ptc;
			for(int i=0;i<_nRecoP;i++)
				ptc_reco.push_back( dynamic_cast<ReconstructedParticle*>(col_RecoP->getElementAt(i)) );
			for(int i=0;i<_nMCP;i++)
				ptc.push_back( dynamic_cast<MCParticle*>(col_MCP->getElementAt(i)) );
			
			/* /////////////////if muon exist?
			Bool_t flagMuM = 0, flagMuP = 0;
			for(int j=0; j<_nRecoP; j++){
				if(flagMuM && flagMuP) break;
				if(ptc_reco[j]->getType()==13){
					flagMuM = 1;
				}
				else if(ptc_reco[j]->getType()==-13){
					flagMuP = 1;
				}
			} */
			/////////////////begin MC particles
			// if(flagMuM && flagMuP)
			// {
			
			m_i = new std::vector<int>();
			m_PDG = new std::vector<int>();
			m_Status = new std::vector<int>();
			m_Charge = new std::vector<int>();
			m_M = new std::vector<float>();
			m_E = new std::vector<float>();
			m_P = new std::vector<TVector3>();
			// m_PT = new std::vector<float>();
			// m_Eta = new std::vector<float>();
			m_Vertex = new std::vector<TVector3>();
			m_Endpoint = new std::vector<TVector3>();
			m_T = new std::vector<float>();
			m_Daughter = new std::vector< vector<int> >();
			m_Parent = new std::vector< vector<int> >();
			
			for(int i=0; i<_nMCP; i++){
				m_i->push_back( i );
				m_PDG->push_back( ptc[i]->getPDG() );
				m_Status->push_back( ptc[i]->getGeneratorStatus() );
				m_Charge->push_back( ptc[i]->getCharge()  );
				m_M->push_back( ptc[i]->getMass()  );
				m_E->push_back( ptc[i]->getEnergy()  );
				TVector3 P = ptc[i]->getMomentum();
				m_P->push_back( P ); //TVector3
				// m_PT->push_back( sqrt(P.X()*P.X() + P.Y()*P.Y()) );
				// m_Eta->push_back( P.PseudoRapidity() );
				m_Vertex->push_back( ptc[i]->getVertex() ); //TVector3
				m_Endpoint->push_back( ptc[i]->getEndpoint() ); //TVector3
				m_T->push_back( ptc[i]->getTime() );
				
				//parents and daughters
				std::vector<int> pa, dau;
				if(ptc[i]->getParents().size()>0){
					int npa=ptc[i]->getParents().size();
					for(int j=0; j<npa; j++)
						for(int k=0; k<_nMCP; k++)
							if(ptc[i]->getParents()[j]==ptc[k])
								pa.push_back( k ); //push back npa times
				}
				if(ptc[i]->getDaughters().size()>0){
					int ndau=ptc[i]->getDaughters().size();
					for(int j=0; j<ndau; j++)
						for(int k=0; k<_nMCP; k++)
							if(ptc[i]->getDaughters()[j]==ptc[k])
								dau.push_back( k );
				}
				m_Parent->push_back( pa );
				m_Daughter->push_back( dau );
			}
			
			///////////begin reco particles
			
			r_j = new std::vector<int>();
			r_PDG = new std::vector<int>();
			r_Charge = new std::vector<int>();
			r_M = new std::vector<float>();
			r_E = new std::vector<float>();
			r_P = new std::vector<TVector3>();
			// r_PT = new std::vector<float>();
			// r_Eta = new std::vector<float>();
			r_iMC = new std::vector< std::vector<int> >();
			r_D0 = new std::vector< std::vector<float> >();
			r_Phi = new std::vector< std::vector<float> >();
			r_Z0 = new std::vector< std::vector<float> >();
			r_Chi2 = new std::vector< std::vector<float> >();
			r_sigmaD0 = new std::vector< std::vector<float> >();
			r_sigmaZ0 = new std::vector< std::vector<float> >();
			
			
			for(int j=0; j<_nRecoP; j++){
				r_j->push_back( j );
				r_PDG->push_back( ptc_reco[j]->getType() ); //can be 0??
				r_E->push_back( ptc_reco[j]->getEnergy() );
				TVector3 P = ptc_reco[j]->getMomentum();
				r_P->push_back( P ); //TVector3
				// r_PT->push_back( sqrt(P.X()*P.X() + P.Y()*P.Y()) );
				// r_Eta->push_back( P.PseudoRapidity() );
				
				//Track
				std::vector<float> d0, phi, z0, chi2, sigd0, sigz0; //push in a certain particle
				std::vector<float*> pcov;
				if(ptc_reco[j]->getTracks().size()>0){
					for(int k=0; k<ptc_reco[j]->getTracks().size(); k++){
						d0.push_back( ptc_reco[j]->getTracks()[k]->getD0() );
						phi.push_back( ptc_reco[j]->getTracks()[k]->getPhi() );
						z0.push_back( ptc_reco[j]->getTracks()[k]->getZ0() );
						chi2.push_back( ptc_reco[j]->getTracks()[k]->getChi2() );
						sigd0.push_back( sqrt(ptc_reco[j]->getTracks()[k]->getCovMatrix()[0]) );
						sigz0.push_back( sqrt(ptc_reco[j]->getTracks()[k]->getCovMatrix()[9]) );
					}
				}
				r_D0->push_back( d0 );
				r_Phi->push_back( phi );
				r_Z0->push_back( z0 );
				r_Chi2->push_back( chi2 );
				r_sigmaD0->push_back( sigd0 );
				r_sigmaZ0->push_back( sigz0 ); 
				
				//Matching iMC
				std::vector<int> imc;
				if(r_PDG->back() != 0) {// can match
					for(int i=0; i<_nMCP; i++){
						TVector3 Pi = ptc[i]->getMomentum();
						// cout<<j<<" "<<r_E->back()<<" ";
						if(r_PDG->back() == ptc[i]->getPDG() && fabs(r_E->back() - ptc[i]->getEnergy()) < fabs(ptc[i]->getEnergy())/10 && fabs(P.Z() - Pi.Z()) < fabs(Pi.Z())/10 && fabs(P.X() - Pi.X()) < fabs(Pi.X())/10 && fabs(P.Y() - Pi.Y()) < fabs(Pi.Y())/10)
							imc.push_back( i );
					}
				}
				r_iMC->push_back( imc );
				
				r_Charge->push_back( -1000 );
				r_M->push_back( -1000. );
				if(r_iMC->back().size() > 0){
					r_Charge->back() = ptc[ r_iMC->back()[0] ]->getCharge();
					r_M->back() = ptc[ r_iMC->back()[0] ]->getMass();
				}
			}
			
			ifstream fin("file.txt");
			fin>>_nfile;
			fin.close();
			_outputTree->Fill();
			
			// }
		}		
		catch (lcio::DataNotAvailableException err) { }
	} 
}	

void ExportEvt::end()
{

	if (_outputTree) {

		TFile *tree_file = _outputTree->GetCurrentFile(); //just in case we switched to a new file
		tree_file->cd();
		tree_file->Write();
		tree_file->Close();
	}

}
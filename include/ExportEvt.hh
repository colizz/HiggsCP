#ifndef _ExportEvt_hh_
#define _ExportEvt_hh_

#include <string>
#include <iostream>
#include <fstream>
#include <marlin/Processor.h>
#include <TNtuple.h>
#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TVector3.h>

class TTree;

class ExportEvt : public marlin::Processor
{
	public:

		Processor*  newProcessor() { return new ExportEvt ; }

		ExportEvt();

		~ExportEvt() {};

		void init();

		void processEvent( LCEvent * evtP );

		void end();

	protected:
		std::string _treeFileName;
		std::string _treeName;
		std::string _colName;
		std::string _colAdcVals;
//		TFile *tree_file;

		int _overwrite;
		TTree *_outputTree;
		unsigned int _eventNr;
		int _Num, _evtN, _nfile, _nMCP, _nRecoP; 
		// Bool_t _TauFlag;
		// int _TauMmode, _TauPmode;
		
		std::vector<int> *m_i, *m_PDG, *m_Status, *m_Charge;
		std::vector<float> *m_M, *m_E, *m_PT, *m_Eta, *m_T;
		std::vector<TVector3> *m_P, *m_Vertex, *m_Endpoint;
		std::vector< std::vector<int> > *m_Daughter, *m_Parent;
		
		std::vector<int> *r_j, *r_PDG, *r_Charge;
		std::vector<float> *r_M, *r_E, *r_PT, *r_Eta;
		std::vector<TVector3> *r_P;
		std::vector< std::vector<int> > *r_iMC;
		std::vector< std::vector<float> > *r_D0, *r_Phi, *r_Z0, *r_Chi2, *r_sigmaD0, *r_sigmaZ0;
		
		
		std::string _fileName;
		std::ostream *_output;
		std::string _histFileName;
};

#endif



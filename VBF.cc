// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/WFinder.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Tools/ParticleIdUtils.hh"
#include "Rivet/Projections/VetoedFinalState.hh"
#include "Rivet/Projections/MissingMomentum.hh"

namespace Rivet {


  class VBF : public Analysis {
  public:

    /// Constructor
    VBF()
      : Analysis("VBF")
    {    }



    /// Book histograms and initialise projections before the run
    void init() {

      FinalState fs;
      Cut cuts = (Cuts::etaIn(-2.5,2.5) 
		  && (Cuts::pT > 25.0*GeV));

      // wp electron
      WFinder w1finder(fs, cuts, PID::POSITRON, 
			70.0*GeV, 100.0*GeV, 10.0*GeV,
			0.1);
      // wp muon
      WFinder w2finder(fs, cuts, PID::ANTIMUON, 
			70.0*GeV, 100.0*GeV, 10.0*GeV,
			0.1);
      
      addProjection(w1finder, "WFpnu");
      addProjection(w2finder, "WFmnu");

      FinalState jet_FS(Cuts::etaIn(-4.5,4.5));
      VetoedFinalState jetfs(jet_FS);
      // Do not include the following particles in the jet definition
      jetfs.addVetoPairId(6);
      jetfs.addVetoPairId(11);
      jetfs.addVetoPairId(12);
      jetfs.addVetoPairId(13);
      jetfs.addVetoPairId(14);
      jetfs.addVetoPairId(15);
      jetfs.addVetoPairId(16);

      // Find jets!
      FastJets jet_FJ_RCUT04(jetfs, FastJets::ANTIKT, 0.4);
      addProjection(jet_FJ_RCUT04, "Jets_FS_RCUT04");
      _histos["WW_pT"]                 = bookHisto1D("WW_pT", 50, 0., 2000.);
      _histos["Delta_eta_jets"]        = bookHisto1D("Delta_eta_jets",50,0.,5.);
      _histos["m_jj"]                  = bookHisto1D("m_jj",100,20.,2000.);
      _histos["N_jets"]                = bookHisto1D("N_jets",7,0,7);
      _histos["pT_1j"]                 = bookHisto1D("pT_1j",50,20.,1000.);
      _histos["pT_2j"]                 = bookHisto1D("pT_2j",50,20.,1000.);
      _histos["Centrality"]            = bookHisto1D("Centrality",20,-4.,4.);
      _histos["Delta_Phi_ll"]          = bookHisto1D("Delta_Phi_ll",20,0.,7.);
      _histos["m_ll"]                  = bookHisto1D("m_ll",100,0.,2000.);
      _histos["Delta_eta_jj"]          = bookHisto1D("Delta_eta_jj",20,-5.,5.);
      // no j-cuts
      _histos["WW_pT_nocut"]                 = bookHisto1D("WW_pT_nocut", 50, 0., 2000.);
      _histos["Delta_eta_jets_nocut"]        = bookHisto1D("Delta_eta_jets_nocut",50,0.,5.);
      _histos["m_jj_nocut"]                  = bookHisto1D("m_jj_nocut",100,20.,2000.);
      _histos["N_jets_nocut"]                = bookHisto1D("N_jets_nocut",7,0,7);
      _histos["pT_1j_nocut"]                 = bookHisto1D("pT_1j_nocut",50,20.,1000.);
      _histos["pT_2j_nocut"]                 = bookHisto1D("pT_2j_nocut",50,20.,1000.);
      _histos["Centrality_nocut"]            = bookHisto1D("Centrality_nocut",20,-4.,4.);
      _histos["Delta_Phi_ll_nocut"]          = bookHisto1D("Delta_Phi_ll_nocut",20,0.,7.);
      _histos["m_ll_nocut"]                  = bookHisto1D("m_ll_nocut",100,0.,2000.);
      _histos["Delta_eta_jj_nocut"]          = bookHisto1D("Delta_eta_jj_nocut",20,0.,5.);

      MissingMomentum meT(fs);
      addProjection(MissingMomentum(fs),"MET");
      
    }


    /// Perform the per-event analysis
    void analyze(const Event& event) {
      const double weight = event.weight();
      const WFinder& Wppnu = applyProjection<WFinder>(event, "WFpnu");
      const WFinder& Wpmnu = applyProjection<WFinder>(event, "WFmnu");
      if(Wppnu.bosons().size()!=1) vetoEvent;
      if(Wpmnu.bosons().size()!=1) vetoEvent;

      const Jets jets = 
	applyProjection<FastJets>(event, "Jets_FS_RCUT04").jetsByPt(30.0);

      if(jets.size()<2) vetoEvent;
      
      const Particles lepts1 = Wppnu.constituentLeptons();
      const Particles lepts2 = Wpmnu.constituentLeptons();
      const FourMomentum l1  = lepts1[0].momentum();
      const FourMomentum l2  = lepts2[0].momentum();

      // Lepton cuts
      double deltaRll        = std::sqrt(sqr(l1.phi()-l2.phi())+
					 sqr(l1.eta()-l2.eta()));
      double mll             = (l1+l2).mass();
      bool leptcut           = deltaRll > 0.3 && mll > 20.0;
      if(!leptcut) vetoEvent;

      int n=0;

      foreach(const Jet& jet_iter, jets){
	const FourMomentum ji   = jet_iter.momentum();
	double deltaRl1j      = std::sqrt(sqr(l1.phi()-ji.phi())+
					  sqr(l1.eta()-ji.eta()));
	double deltaRl2j      = std::sqrt(sqr(l2.phi()-ji.phi())+
					  sqr(l2.eta()-ji.eta()));
	if(!(deltaRl1j > 0.3 && deltaRl2j > 0.3)) ++n;
      }
      if(n!=0) vetoEvent;
    
      const FourMomentum Wpsmom = (Wppnu.bosons()[0].momentum() + 
				   Wpmnu.bosons()[0].momentum());

      const FourMomentum jmom1   = jets[0].momentum();
      const FourMomentum jmom2   = jets[1].momentum();

      // Missing Et
      const MissingMomentum& meT = applyProjection<MissingMomentum>(event, "MET");
      double misseT = meT.scalarEt();

      if(!(misseT >= 40.)) vetoEvent;
      bool jetcuts = (jmom1+jmom2).mass() > 500. && (jmom1.rapidity()-jmom2.rapidity())> 2.4;
      
      // Centrality
      double a    = std::min(l1.eta(),l2.eta());
      double b    = std::min(jmom1.eta(),jmom2.eta());
      double c    = std::max(jmom1.eta(),jmom2.eta());
      double d    = std::max(l1.eta(),l2.eta());
      double zeta = std::min(a-b, c-d);

      
      ////////////////// (jetcuts)
      if(jetcuts){
	_histos["WW_pT"]->fill(Wpsmom.pT()/GeV,weight);
	_histos["Delta_eta_jets"]->fill(std::abs(jmom1.eta()-jmom2.eta()),
					weight);
	_histos["m_jj"]->fill((jmom1+jmom2).mass()/GeV,weight);
	_histos["N_jets"]->fill(jets.size()+0.5,weight);
	_histos["pT_1j"]->fill(jmom1.pT()/GeV,weight);
	_histos["pT_2j"]->fill(jmom2.pT()/GeV,weight);
	_histos["Centrality"]->fill(zeta,weight);
	_histos["Delta_Phi_ll"]->fill(std::abs(l1.phi()-l2.phi()),
				      weight);
	_histos["m_ll"]->fill(mll/GeV,weight);
	_histos["Delta_eta_jj"]->fill(std::abs(jmom1.eta()-jmom2.eta()),weight);
      }
      else if(!(jetcuts)){
	_histos["WW_pT_nocut"]->fill(Wpsmom.pT()/GeV,weight);
	_histos["Delta_eta_jets_nocut"]->fill(std::abs(jmom1.eta()-jmom2.eta()),
					weight);
	_histos["m_jj_nocut"]->fill((jmom1+jmom2).mass()/GeV,weight);
	_histos["N_jets_nocut"]->fill(jets.size()+0.5,weight);
	_histos["pT_1j_nocut"]->fill(jmom1.pT()/GeV,weight);
	_histos["pT_2j_nocut"]->fill(jmom2.pT()/GeV,weight);
	_histos["Centrality_nocut"]->fill(zeta,weight);
	_histos["Delta_Phi_ll_nocut"]->fill(std::abs(jmom1.phi()-jmom2.phi()),
					weight);
	_histos["m_ll_nocut"]->fill(mll/GeV,weight);
	_histos["Delta_eta_jj_nocut"]->fill(std::abs(jmom1.eta()-jmom2.eta()),weight);
      }
    }

    
    /// Normalise histograms etc., after the run
    void finalize() {

    }

    //@}


  private:

    // Data members like post-cuts event weight counters go here
    // {@
    std::map<string,Histo1DPtr> _histos;
    //@}
  };

  // The hook for the plugin system
  DECLARE_RIVET_PLUGIN(VBF);


}

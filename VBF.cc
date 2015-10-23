// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/WFinder.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Tools/ParticleIdUtils.hh"
#include "Rivet/Projections/VetoedFinalState.hh"

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
      Cut cuts = (Cuts::etaIn(-2.0,2.0) 
		  && (Cuts::pT > 25.0*GeV));

      // wp electron
      WFinder wp1finder(fs, cuts, PID::POSITRON, 
			70.0*GeV, 100.0*GeV, 25.0*GeV,
			0.1);
      // wp muon
      WFinder wp2finder(fs, cuts, PID::ANTIMUON, 
			70.0*GeV, 100.0*GeV, 25.0*GeV,
			0.1);
      
      addProjection(wp1finder, "WFpnu");
      addProjection(wp2finder, "WFmnu");

      FinalState jet_FS(Cuts::etaIn(-5.0,5.0));
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
      FastJets jet_FJ_RCUT04(jetfs, FastJets::ANTIKT, 0.5);
      addProjection(jet_FJ_RCUT04, "Jets_FS_RCUT04");
      _histos["WW_pT"]           = bookHisto1D("WW_pT", 50, 0., 2000.);
      _histos["Delta_eta_jets"]  = bookHisto1D("Delta_eta_jets",50,0.,5.);
      _histos["m_jj"]            = bookHisto1D("m_jj",100,20.,2000.);
    }


    /// Perform the per-event analysis
    void analyze(const Event& event) {
      const double weight = event.weight();
      const WFinder& Wppnu = applyProjection<WFinder>(event, "WFpnu");
      const WFinder& Wpmnu = applyProjection<WFinder>(event, "WFmnu");
      if(Wppnu.bosons().size()!=1) vetoEvent;
      if(Wpmnu.bosons().size()!=1) vetoEvent;

      const Jets jets = 
	applyProjection<FastJets>(event, "Jets_FS_RCUT04").jetsByPt(20.0);

      const FourMomentum Wpsmom = (Wppnu.bosons()[0].momentum() + 
				   Wpmnu.bosons()[0].momentum());
      _histos["WW_pT"]->fill(Wpsmom.pT()/GeV,weight);
      const FourMomentum jmom1   = jets[1].momentum();
      const FourMomentum jmom2   = jets[1].momentum();      

      _histos["Delta_eta_jets"]->fill(std::abs(jmom1.eta()-jmom2.eta()),
				      weight);
      _histos["m_jj"]->fill((jmom1+jmom2).mass()/GeV,weight);
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

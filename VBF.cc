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
      Cut cuts = (Cuts::etaIn(-5.0,5.0) 
		  && (Cuts::pT > 25.0*GeV));

      // wp electron
      WFinder wp1finder(fs, cuts, PID::POSITRON, 
			70.0*GeV, 100.0*GeV, 25.0*GeV,
			0.2);
      // wp muon
      WFinder wp2finder(fs, cuts, PID::ANTIMUON, 
			70.0*GeV, 100.0*GeV, 25.0*GeV,
			0.2);
      
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

//      addProjection(jet_FS, "HadFSsss");
     // addProjection(jetfs, "HadFS");

      // Find jets!
      FastJets jet_FJ_RCUT04(jetfs, FastJets::ANTIKT, 0.5);
      addProjection(jet_FJ_RCUT04, "Jets_FS_RCUT04");
      _h_pT = bookHisto1D("WpT", 50, 0., 2000.);
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
      _h_pT->fill(Wpsmom.pT()/GeV,weight);
    }


    /// Normalise histograms etc., after the run
    void finalize() {

      /// @todo Normalise, scale and otherwise manipulate histograms here

      // scale(_h_YYYY, crossSection()/sumOfWeights()); // norm to cross section
      // normalize(_h_YYYY); // normalize to unity

    }

    //@}


  private:

    // Data members like post-cuts event weight counters go here


    /// @name Histograms
    //@{
    Profile1DPtr _h_XXXX;
    Histo1DPtr _h_pT;
    //@}


  };



  // The hook for the plugin system
  DECLARE_RIVET_PLUGIN(VBF);


}

// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/ChargedFinalState.hh"
#include "Rivet/Projections/ZFinder.hh"

#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <TRandom.h>
#include <unistd.h>

namespace Rivet {


  class ATLAS_2020_I1315949 : public Analysis {
  public:

    /// Constructor
    DEFAULT_RIVET_ANALYSIS_CTOR(ATLAS_2020_I1315949);

    void init() {

      FinalState fs;

      ZFinder zfinder(fs, Cuts::abseta<2.4 && Cuts::pT>20.0*GeV, PID::MUON, 66*GeV, 116*GeV, 0.1, ZFinder::ClusterPhotons::NODECAY);
      declare(zfinder, "ZFinder");

      ChargedFinalState cfs( zfinder.remainingFinalState() );
      declare(cfs, "cfs");


      book(_h_pTsum_tow    , 67, 1, 1);
      book(_h_pTsum_trv    , 68, 1, 1);
      book(_h_pTsum_away   , 69, 1, 1);
      book(_h_pTsum_tmin   , 70, 1, 1);
      book(_h_pTsum_tmax   , 71, 1, 1);
      book(_h_pTsum_tdif   ,125, 1, 1);

      book(_h_Nchg_tow     , 72, 1, 1);
      book(_h_Nchg_trv     , 73, 1, 1);
      book(_h_Nchg_away    , 74, 1, 1);
      book(_h_Nchg_tmin    , 75, 1, 1);
      book(_h_Nchg_tmax    , 82, 1, 1);
      book(_h_Nchg_tdif    ,126, 1, 1);

      book(_h_pTavg_tow    ,113, 1, 1);
      book(_h_pTavg_trv    ,114, 1, 1);
      book(_h_pTavg_away   ,115, 1, 1);

      book(_h_pTavgvsmult_tow , 116, 1, 1);
      book(_h_pTavgvsmult_trv , 117, 1, 1);
      book(_h_pTavgvsmult_away, 118, 1, 1);


      // Book sumpt and nch histos
      for (size_t id = 0; id < 6.; ++id) {
        book(_h_ptSum_1D[0][id], 76 + id, 1, 1);
        book(_h_ptSum_1D[1][id],107 + id, 1, 1);
        book(_h_ptSum_1D[2][id],119 + id, 1, 1);
        book(_h_ptSum_1D[3][id],127 + id, 1, 1);
        book(_h_Nchg_1D[0][id],  83 + id, 1, 1);
        book(_h_Nchg_1D[1][id],  89 + id, 1, 1);
        book(_h_Nchg_1D[2][id],  95 + id, 1, 1);
        book(_h_Nchg_1D[3][id], 101 + id, 1, 1);
      }

      // create ROOT file
      char buffer[512];
      sprintf(buffer, "Analysis_%d.root", getpid());
      string out_filename(buffer);
      file_handle = TFile::Open(out_filename.c_str(), "RECREATE");
      tree = new TTree("output", "ATLAS_2014_I1315949 output");
      tree->Branch("Zpt", &br_Zpt, "Zpt/D");
      tree->Branch("ptSumTowards", &br_ptSumTowards, "ptSumTowards/D");
      tree->Branch("ptSumTransvers", &br_ptSumTransverse, "ptSumTransvers/D");
      tree->Branch("ptSumAway", &br_ptSumAway, "ptSumAway/D");
      tree->Branch("ptSumTrmin", &br_ptSumTrmin, "ptSumTrmin/D");
      tree->Branch("ptSumTrmax", &br_ptSumTrmax, "ptSumTrmax/D");

      tree->Branch("nTowards", &br_nTowards, "nTowards/I");
      tree->Branch("nTransverse", &br_nTransverse, "nTransverse/I");
      tree->Branch("nAway", &br_nAway, "nAway/I");
      tree->Branch("nTrmin", &br_nTrmin, "nTrmin/I");
      tree->Branch("nTrmax", &br_nTrmax, "nTrmax/I");

      tree->Branch("ibin", &br_ibin, "ibin/I");
      nEvts = nEvtsSelected = 0;
    }


    /// Perform the per-event analysis
    void analyze(const Event& event) {

      nEvts ++;
      const ZFinder& zfinder = apply<ZFinder>(event, "ZFinder");

      if (zfinder.bosons().size() != 1) vetoEvent;

      double  Zpt   = zfinder.bosons()[0].momentum().pT()/GeV;
      double  Zphi  = zfinder.bosons()[0].momentum().phi(MINUSPI_PLUSPI);
      double  Zmass = zfinder.bosons()[0].momentum().mass()/GeV;
      if(Zmass < 66. || Zmass > 116.) vetoEvent;

      nEvtsSelected ++;

      // Initialise counters for Nch and sumPt for all regions
      int nTowards(0), nTransverse(0), nLeft(0), nRight(0), nTrmin(0), nTrmax(0), nAway(0);
      double ptSumTowards(0.0), ptSumTransverse(0.0), ptSumLeft(0.0), ptSumRight(0.0),
             ptSumTrmin(0.0), ptSumTrmax(0.0), ptSumAway(0.0);

      // The charged particles
      Particles particles = apply<ChargedFinalState>(event, "cfs").particlesByPt(
          Cuts::pT > 0.5*GeV && Cuts::abseta <2.5);

      // Loop over charged particles with pT>500 MeV and |eta|<2.5
      for (const Particle& p : particles) {
        double dphi = p.momentum().phi(MINUSPI_PLUSPI) - Zphi;
        double pT   = p.momentum().pT();

        // Get multiples of 2pi right
        for(; std::fabs(dphi) > M_PI; dphi += (dphi > 0. ? -2.*M_PI : 2.*M_PI) );

        // Towards region
        if( std::fabs(dphi) < M_PI/3. ) {
          nTowards++;
          ptSumTowards += pT;
        }
        // Transverse region
        else if( std::fabs(dphi) < 2.*M_PI/3. ) {
          nTransverse++;
          ptSumTransverse += pT;
          if(dphi > 0.) {
            nRight++;
            ptSumRight += pT;
          }
          else {
            nLeft++;
            ptSumLeft += pT;
          }
        }
        // Away region
        else {
          nAway++;
          ptSumAway += pT;
        }
      }

      // TransMAX, TransMIN regions
      if (ptSumLeft > ptSumRight) {
        ptSumTrmax = ptSumLeft;
        ptSumTrmin = ptSumRight;
        nTrmax     = nLeft;
        nTrmin     = nRight;
      }
      else {
	ptSumTrmax = ptSumRight;
	ptSumTrmin = ptSumLeft;
	nTrmax     = nRight;
	nTrmin     = nLeft;
      }

      // min max regions have difference are than all other regions
      const double area = 5.*2./3.*M_PI;

      // Fill sumPt vs. Zpt region profiles
      _h_pTsum_tow->fill( Zpt, ptSumTowards/area);
      _h_pTsum_trv->fill( Zpt, ptSumTransverse/area);
      _h_pTsum_away->fill(Zpt, ptSumAway/area);
      _h_pTsum_tmin->fill(Zpt, ptSumTrmin/(0.5*area));
      _h_pTsum_tmax->fill(Zpt, ptSumTrmax/(0.5*area));
      _h_pTsum_tdif->fill(Zpt, (ptSumTrmax - ptSumTrmin)/(0.5*area));

      // Fill Nch vs. Zpt region profiles
      _h_Nchg_tow->fill( Zpt, nTowards/area);
      _h_Nchg_trv->fill( Zpt, nTransverse/area);
      _h_Nchg_away->fill(Zpt, nAway/area);
      _h_Nchg_tmin->fill(Zpt, nTrmin/(0.5*area));
      _h_Nchg_tmax->fill(Zpt, nTrmax/(0.5*area));
      _h_Nchg_tdif->fill(Zpt, (nTrmax - nTrmin)/(0.5*area));


      // Fill <pT> vs. ZpT profiles
      _h_pTavg_tow->fill( Zpt, nTowards    > 0.? ptSumTowards/nTowards       : 0.);
      _h_pTavg_trv->fill( Zpt, nTransverse > 0.? ptSumTransverse/nTransverse : 0.);
      _h_pTavg_away->fill(Zpt, nAway       > 0.? ptSumAway/nAway             : 0.);

      // Fill <Nch> vs. ZpT profiles
      _h_pTavgvsmult_tow->fill( nTowards,    nTowards    > 0.? ptSumTowards/nTowards       : 0.);
      _h_pTavgvsmult_trv->fill( nTransverse, nTransverse > 0.? ptSumTransverse/nTransverse : 0.);
      _h_pTavgvsmult_away->fill(nAway,       nAway       > 0.? ptSumAway/nAway             : 0.);

      // Determine Zpt region histo to fill
      int i_bin(0);
      if (inRange(Zpt,0,5)        ) i_bin=0;
      if (inRange(Zpt,5,10)       ) i_bin=1;
      if (inRange(Zpt,10,20)      ) i_bin=2;
      if (inRange(Zpt,20,50)      ) i_bin=3;
      if (inRange(Zpt,50,110)     ) i_bin=4;
      if (Zpt>110) i_bin=5;

      // SumPt histos for Zpt region
      _h_ptSum_1D[0][i_bin]->fill(ptSumTowards/area);
      _h_ptSum_1D[1][i_bin]->fill(ptSumTransverse/area);
      _h_ptSum_1D[2][i_bin]->fill(ptSumTrmin/(0.5*area));
      _h_ptSum_1D[3][i_bin]->fill(ptSumTrmax/(0.5*area));

      // Nch histos for Zpt region
      _h_Nchg_1D[0][i_bin]->fill(nTowards/area);
      _h_Nchg_1D[1][i_bin]->fill(nTransverse/area);
      _h_Nchg_1D[2][i_bin]->fill(nTrmin/(0.5*area));
      _h_Nchg_1D[3][i_bin]->fill(nTrmax/(0.5*area));

      // Fill tree info
      br_Zpt = Zpt;
      br_ptSumTowards = ptSumTowards;
      br_ptSumTransverse = ptSumTransverse;
      br_ptSumAway = ptSumAway;
      br_ptSumTrmin = ptSumTrmin;
      br_ptSumTrmax = ptSumTrmax;
      br_nTowards = nTowards;
      br_nAway = nAway;
      br_nTransverse = nTransverse;
      br_nTrmin = nTrmin;
      br_nTrmax = nTrmax;
      br_ibin = i_bin;
      tree->Fill();
    }


    /// Normalise histograms etc., after the run
    // the finalize and analyze function will be called
    // serval times depending on the number of variations in the HEPMC data
    // the root file only saves the first one.
    // Hopefully it is the baseline.
    void finalize() {
      for(int i_reg = 0; i_reg < 4; i_reg++) {
        for(int i_bin = 0; i_bin < 6; i_bin++) {
          normalize( _h_ptSum_1D[i_reg][i_bin] );
          normalize( _h_Nchg_1D[ i_reg][i_bin] );
        }
      }
      // cout << "Total Events: " << nEvts << ". Selected: " << nEvtsSelected << endl;
      if (file_handle && tree) {
        file_handle->cd();
        tree->Write();
        file_handle->Close();
        tree = nullptr;
        file_handle = nullptr;
      }
      // cout << "END of Finalized" << endl;
    }


  private:

    Profile1DPtr _h_pTsum_tow,
                 _h_pTsum_trv,
                 _h_pTsum_away,
                 _h_pTsum_tmin,
                 _h_pTsum_tmax,
                 _h_pTsum_tdif,

                 _h_Nchg_tow,
                 _h_Nchg_trv,
                 _h_Nchg_away,
                 _h_Nchg_tmin,
                 _h_Nchg_tmax,
                 _h_Nchg_tdif,

                 _h_pTavg_tow,
                 _h_pTavg_trv,
                 _h_pTavg_away,
                 _h_pTavgvsmult_tow,
                 _h_pTavgvsmult_trv,
                 _h_pTavgvsmult_away;

    Histo1DPtr   _h_ptSum_1D[4][6], _h_Nchg_1D[4][6];

    TFile* file_handle;
    TTree* tree;

    double br_Zpt;
    double br_ptSumTowards;
    double br_ptSumTransverse;
    double br_ptSumAway;
    double br_ptSumTrmin;
    double br_ptSumTrmax;
    int br_nTowards;
    int br_nTransverse;
    int br_nAway;
    int br_nTrmin;
    int br_nTrmax;
    int br_ibin;

    long int nEvts;
    long int nEvtsSelected;
  };


  DECLARE_RIVET_PLUGIN(ATLAS_2020_I1315949);

}

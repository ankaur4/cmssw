#include "RecoParticleFlow/Benchmark/interface/PFBenchmarkAna.h"

#include "DQMServices/Core/interface/DaqMonitorBEInterface.h"
#include "DQMServices/Daemon/interface/MonitorDaemon.h"
#include "DQMServices/CoreROOT/interface/CollateMERoot.h"
//#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace reco;
using namespace std;

// preprocessor macro for booking 1d histos with DaqMonitorBEInterface -or- bare Root
#define BOOK1D(name,title,nbinsx,lowx,highx) { \
  if (dbe_) { \
    MonitorElement *me = dbe_->book1D(#name,#title,nbinsx,lowx,highx); \
    MonitorElementT<TNamed> *ob = dynamic_cast<MonitorElementT<TNamed>* >(me); \
    if (ob) h##name = dynamic_cast<TH1F *>(ob->operator->()); \
  } else \
    h##name = new TH1F(#name,#title,nbinsx,lowx,highx); \
}

// preprocessor macro for booking 2d histos with DaqMonitorBEInterface -or- bare Root
#define BOOK2D(name,title,nbinsx,lowx,highx,nbinsy,lowy,highy) { \
  if (dbe_) { \
    MonitorElement *me = dbe_->book2D(#name,#title,nbinsx,lowx,highx,nbinsy,lowy,highy); \
    MonitorElementT<TNamed> *ob = dynamic_cast<MonitorElementT<TNamed>* >(me); \
    if (ob) h##name = dynamic_cast<TH2F *>(ob->operator->()); \
  } else \
    h##name = new TH2F(#name,#title,nbinsx,lowx,highx,nbinsy,lowy,highy); \
}

PFBenchmarkAna::PFBenchmarkAna() {}

PFBenchmarkAna::~PFBenchmarkAna() {}

void PFBenchmarkAna::setup(DaqMonitorBEInterface *DQM) {

  if (DQM) dbe_ = DQM;
  else file_ = new TFile();

  // delta et quantities
  BOOK1D(DeltaEt,DeltaEt,1000,-100,100);
  BOOK2D(DeltaEtvsEt,DeltaEtvsEt,1000,0,1000,1000,-100,100);
  BOOK2D(DeltaEtOverEtvsEt,DeltaEtOverEtvsEt,1000,0,1000,100,-1,1);
  BOOK2D(DeltaEtvsEta,DeltaEtvsEta,200,-5,5,1000,-100,100);
  BOOK2D(DeltaEtOverEtvsEta,DeltaEtOverEtvsEta,200,-5,5,100,-1,1);
  BOOK2D(DeltaEtvsPhi,DeltaEtvsPhi,200,-M_PI,M_PI,1000,-100,100);
  BOOK2D(DeltaEtOverEtvsPhi,DeltaEtOverEtvsPhi,200,-M_PI,M_PI,100,-1,1);
  BOOK2D(DeltaEtvsDeltaR,DeltaEtvsDeltaR,100,0,1,1000,-100,100);
  BOOK2D(DeltaEtOverEtvsDeltaR,DeltaEtOverEtvsDeltaR,100,0,1,100,-1,1);

  // delta eta quantities
  BOOK1D(DeltaEta,DeltaEta,100,-3,3);
  BOOK2D(DeltaEtavsEt,DeltaEtavsEt,1000,0,1000,100,-3,3);
  BOOK2D(DeltaEtaOverEtavsEt,DeltaEtaOverEtavsEt,1000,0,1000,100,-1,1);
  BOOK2D(DeltaEtavsEta,DeltaEtavsEta,200,-5,5,100,-3,3);
  BOOK2D(DeltaEtaOverEtavsEta,DeltaEtaOverEtvsEta,200,-5,5,100,-1,1);
  BOOK2D(DeltaEtavsPhi,DeltaEtavsPhi,200,-M_PI,M_PI,200,-M_PI,M_PI);
  BOOK2D(DeltaEtaOverEtavsPhi,DeltaEtaOverEtavsPhi,200,-M_PI,M_PI,100,-1,1);

  // delta phi quantities
  BOOK1D(DeltaPhi,DeltaPhi,100,-M_PI_2,M_PI_2);
  BOOK2D(DeltaPhivsEt,DeltaPhivsEt,1000,0,1000,100,-M_PI_2,M_PI_2);
  BOOK2D(DeltaPhiOverPhivsEt,DeltaPhiOverPhivsEt,1000,0,1000,100,-1,1);
  BOOK2D(DeltaPhivsEta,DeltaPhivsEta,200,-5,5,100,-M_PI_2,M_PI_2);
  BOOK2D(DeltaPhiOverPhivsEta,DeltaPhiOverPhivsEta,200,-5,5,100,-1,1);
  BOOK2D(DeltaPhivsPhi,DeltaPhivsPhi,200,-M_PI,M_PI,200,-M_PI,M_PI);
  BOOK2D(DeltaPhiOverPhivsPhi,DeltaPhiOverPhivsPhi,200,-M_PI,M_PI,100,-1,1);

  // delta R quantities
  BOOK1D(DeltaR,DeltaR,100,0,1);
  BOOK2D(DeltaRvsEt,DeltaRvsEt,1000,0,1000,100,0,1);
  BOOK2D(DeltaRvsEta,DeltaRvsEta,200,-5,5,100,0,1);
  BOOK2D(DeltaRvsPhi,DeltaRvsPhi,200,-M_PI,M_PI,100,0,1);

  // number of truth particles found within given cone radius of reco
  //BOOK2D(NumInConeVsConeSize,NumInConeVsConeSize,100,0,1,25,0,25);

}

void PFBenchmarkAna::fill(const CandidateCollection *Reco, const CandidateCollection *Gen, bool PlotAgainstReco) {

  // loop over reco particles
  CandidateCollection::const_iterator reco;
  for (reco = Reco->begin(); reco != Reco->end(); reco++) {

    // generate histograms comparing the reco and truth candidate (truth = closest in delta-R)
    const Candidate *particle = &(*reco);
    const Candidate *gen_particle = algo_->matchByDeltaR(particle,Gen);

    // get the quantities to place on the denominator and/or divide by
    double et, eta, phi;
    if (PlotAgainstReco) { et = particle->et(); eta = particle->eta(); phi = particle->phi(); }
    else { et = gen_particle->et(); eta = gen_particle->eta(); phi = gen_particle->phi(); }

    // get the delta quantities
    double deltaEt = algo_->deltaEt(particle,gen_particle);
    double deltaR = algo_->deltaR(particle,gen_particle);
    double deltaEta = algo_->deltaEta(particle,gen_particle);
    double deltaPhi = algo_->deltaPhi(particle,gen_particle);

    // fill histograms
    hDeltaEt->Fill(deltaEt);
    hDeltaEtvsEt->Fill(et,deltaEt);
    hDeltaEtOverEtvsEt->Fill(et,deltaEt/et);
    hDeltaEtvsEta->Fill(eta,deltaEt);
    hDeltaEtOverEtvsEta->Fill(eta,deltaEt/et);
    hDeltaEtvsPhi->Fill(phi,deltaEt);
    hDeltaEtOverEtvsPhi->Fill(phi,deltaEt/et);
    hDeltaEtvsDeltaR->Fill(deltaR,deltaEt);
    hDeltaEtOverEtvsDeltaR->Fill(deltaR,deltaEt/et);

    hDeltaEta->Fill(deltaEta);
    hDeltaEtavsEt->Fill(et,deltaEta/eta);
    hDeltaEtaOverEtavsEt->Fill(et,deltaEta/eta);
    hDeltaEtavsEta->Fill(eta,deltaEta);
    hDeltaEtaOverEtavsEta->Fill(eta,deltaEta/eta);
    hDeltaEtavsPhi->Fill(phi,deltaEta);
    hDeltaEtaOverEtavsPhi->Fill(phi,deltaEta/eta);

    hDeltaPhi->Fill(deltaPhi);
    hDeltaPhivsEt->Fill(et,deltaPhi);
    hDeltaPhiOverPhivsEt->Fill(et,deltaPhi/phi);
    hDeltaPhivsEta->Fill(eta,deltaPhi);
    hDeltaPhiOverPhivsEta->Fill(eta,deltaPhi/phi);
    hDeltaPhivsPhi->Fill(phi,deltaPhi);
    hDeltaPhiOverPhivsPhi->Fill(phi,deltaPhi/phi);

    hDeltaR->Fill(deltaR);
    hDeltaRvsEt->Fill(et,deltaR);
    hDeltaRvsEta->Fill(eta,deltaR);

    /* still a work in progress....
    // find all truth candidate matches within the given cone
    CandidateCollection match_candidates = algo_->findAllInCone(particle,truth_candidates,MAXCONE);

    // variables for filling out the 'NumMatches' histogram
    int nmatches = match_candidates.size();
    static int nbins = me["NumMatchesVsDeltaR"]->getNbinsX();
    static double mincone = 0, maxcone = 1;
    static double binwidth = (maxcone - mincone) / (2 * nbins);
    int lastbin = 1;

    // loop over matching candidates to find the delta-R's
    CandidateCollection::iterator match;
    for (match = match_candidates.begin(); match != match_candidates.end(); match++) {

      // calculate this match's delta-R
      const Candidate *match_particle = &(*match);
      double deltaR = algo_->deltaR(particle,match_particle);

      // identify the bin number associated with this delta-R
      int upperbin = (int)ceil(deltaR / binwidth) + 1;

      // fill the histogram
      for (int bin = lastbin; bin < upperbin; bin++)
        me["NumMatchesVsDeltaR"]->

      // adjust the variables for the next pass
      nmatches--; lastbin = bin;

    }*/

  }

}

void PFBenchmarkAna::write(string Filename) {

  if (Filename.size() != 0 && file_)
    file_->Write(Filename.c_str());

}

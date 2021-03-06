// Author: Arabella Martelli, Felice Pantaleo, Marco Rovere
// arabella.martelli@cern.ch, felice.pantaleo@cern.ch, marco.rovere@cern.ch
// Date: 06/2019
#include <algorithm>
#include <set>
#include <vector>

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "SeedingRegionByTracks.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "DataFormats/GeometrySurface/interface/BoundDisk.h"

using namespace ticl;

SeedingRegionByTracks::SeedingRegionByTracks(const edm::ParameterSet &conf, edm::ConsumesCollector &sumes)
    : SeedingRegionAlgoBase(conf, sumes),
      tracks_token_(sumes.consumes<reco::TrackCollection>(conf.getParameter<edm::InputTag>("tracks"))),
      cutTk_(conf.getParameter<std::string>("cutTk")),
      propName_(conf.getParameter<std::string>("propagator")) {}

SeedingRegionByTracks::~SeedingRegionByTracks() {}

void SeedingRegionByTracks::initialize(const edm::EventSetup &es) {
  edm::ESHandle<HGCalDDDConstants> hdc;
  es.get<IdealGeometryRecord>().get(detectorName_, hdc);
  hgcons_ = hdc.product();

  buildFirstLayers();

  es.get<IdealMagneticFieldRecord>().get(bfield_);
  es.get<TrackingComponentsRecord>().get(propName_, propagator_);
}

void SeedingRegionByTracks::makeRegions(const edm::Event &ev,
                                        const edm::EventSetup &es,
                                        std::vector<TICLSeedingRegion> &result) {
  edm::Handle<reco::TrackCollection> tracks_h;
  ev.getByToken(tracks_token_, tracks_h);
  edm::ProductID trkId = tracks_h.id();
  auto bFieldProd = bfield_.product();
  const Propagator &prop = (*propagator_);

  int nTracks = tracks_h->size();
  for (int i = 0; i < nTracks; ++i) {
    const reco::Track &tk = (*tracks_h)[i];
    if (!cutTk_((tk))) {
      continue;
    }

    FreeTrajectoryState fts = trajectoryStateTransform::outerFreeState((tk), bFieldProd);
    int iSide = int(tk.eta() > 0);
    TrajectoryStateOnSurface tsos = prop.propagate(fts, firstDisk_[iSide]->surface());
    if (tsos.isValid()) {
      result.emplace_back(tsos.globalPosition(), tsos.globalMomentum(), iSide, i, trkId);
    }
  }
}

void SeedingRegionByTracks::buildFirstLayers() {
  float zVal = hgcons_->waferZ(1, true);
  std::pair<double, double> rMinMax = hgcons_->rangeR(zVal, true);

  for (int iSide = 0; iSide < 2; ++iSide) {
    float zSide = (iSide == 0) ? (-1. * zVal) : zVal;
    firstDisk_[iSide] =
        std::make_unique<GeomDet>(Disk::build(Disk::PositionType(0, 0, zSide),
                                              Disk::RotationType(),
                                              SimpleDiskBounds(rMinMax.first, rMinMax.second, zSide - 0.5, zSide + 0.5))
                                      .get());
  }
}

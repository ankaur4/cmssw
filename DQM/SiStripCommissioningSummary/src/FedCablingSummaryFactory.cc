#include "DQM/SiStripCommissioningSummary/interface/FedCablingSummaryFactory.h"
#include "DQM/SiStripCommissioningSummary/interface/SummaryGenerator.h"
#include "DQM/SiStripCommon/interface/SiStripHistoNamingScheme.h"
#include <iostream>
#include <sstream>

using namespace std;

// -----------------------------------------------------------------------------
//
SummaryHistogramFactory<FedCablingAnalysis>::SummaryHistogramFactory() :
  histo_(sistrip::UNKNOWN_SUMMARY_HISTO),
  type_(sistrip::UNKNOWN_SUMMARY_TYPE),
  view_(sistrip::UNKNOWN_VIEW),
  level_(sistrip::root_),
  gran_(sistrip::UNKNOWN_GRAN),
  generator_(0) 
{
} 


// -----------------------------------------------------------------------------
//
SummaryHistogramFactory<FedCablingAnalysis>::~SummaryHistogramFactory() {
  if ( generator_ ) { delete generator_; }
}

// -----------------------------------------------------------------------------
//
void SummaryHistogramFactory<FedCablingAnalysis>::init( const sistrip::SummaryHisto& histo, 
							const sistrip::SummaryType& type,
							const sistrip::View& view, 
							const string& top_level_dir, 
							const sistrip::Granularity& gran ) {
  histo_ = histo;
  type_ = type;
  view_ = view;
  level_ = top_level_dir;
  gran_ = gran;

  // Retrieve utility class used to generate summary histograms
  if ( generator_ ) { delete generator_; generator_ = 0; }
  generator_ = SummaryGenerator::instance( view );
  
}

//------------------------------------------------------------------------------
//
uint32_t SummaryHistogramFactory<FedCablingAnalysis>::extract( const map<uint32_t,FedCablingAnalysis>& data  ) {
  
  // Check if data are present
  if ( data.empty() ) { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " No data to histogram!" << endl;
    return 0; 
  } 
  
  // Check if instance of generator class exists
  if ( !generator_ ) { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to SummaryGenerator object!" << endl;
    return 0;  
  }

  // Transfer appropriate monitorables info to generator object
  generator_->clearMap();
  map<uint32_t,FedCablingAnalysis>::const_iterator iter = data.begin();
  for ( ; iter != data.end(); iter++ ) {
    if ( histo_ == sistrip::FED_CABLING_FED_ID ) {
      generator_->fillMap( level_, gran_, iter->first, iter->second.fedId() ); 
    } else if ( histo_ == sistrip::FED_CABLING_FED_ID ) { 
      generator_->fillMap( level_, gran_, iter->first, iter->second.fedCh() ); 
    } else { 
      cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	   << " Unexpected SummaryHisto value:"
	   << SiStripHistoNamingScheme::summaryHisto( histo_ ) 
	   << endl;
      continue;
    }
  }
  return generator_->size();
}

//------------------------------------------------------------------------------
//
void SummaryHistogramFactory<FedCablingAnalysis>::fill( TH1& summary_histo ) {

  // Check if instance of generator class exists
  if ( !generator_ ) { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to SummaryGenerator object!" << endl;
    return;
  }

  // Check if instance of generator class exists
  if ( !(&summary_histo) ) { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to SummaryGenerator object!" << endl;
    return;
  }

  // Check if map is filled
  if ( !generator_->size() ) { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " No data in the monitorables map!" << endl;
    return; 
  } 

  // Generate appropriate summary histogram 
  if ( type_ == sistrip::SUMMARY_DISTR ) {
    generator_->summaryDistr( summary_histo );
  } else if ( type_ == sistrip::SUMMARY_1D ) {
    generator_->summary1D( summary_histo );
  } else if ( type_ == sistrip::SUMMARY_2D ) {
    generator_->summary2D( summary_histo );
  } else if ( type_ == sistrip::SUMMARY_PROF ) {
    generator_->summaryProf( summary_histo );
  } else { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " Unexpected SummaryType value:"
	 << SiStripHistoNamingScheme::summaryType( type_ ) 
	 << endl;
    return; 
  }
  
  // Histogram formatting
  generator_->format( histo_, type_, view_, level_, summary_histo );
  if ( histo_ == sistrip::FED_CABLING_FED_ID ) {
  } else if ( histo_ == sistrip::FED_CABLING_FED_ID ) { 
  } else { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " Unexpected SummaryHisto value:"
	 << SiStripHistoNamingScheme::summaryHisto( histo_ ) 
	 << endl;
    return; 
  } 
  
}

// -----------------------------------------------------------------------------
//
template class SummaryHistogramFactory<FedCablingAnalysis>;


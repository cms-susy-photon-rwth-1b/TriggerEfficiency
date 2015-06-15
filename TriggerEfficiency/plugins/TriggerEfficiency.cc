// -*- C++ -*-
//
// Package:    TriggerEfficiency/TriggerEfficiency
// Class:      TriggerEfficiency
//
/**\class TriggerEfficiency TriggerEfficiency.cc TriggerEfficiency/TriggerEfficiency/plugins/TriggerEfficiency.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//
// Original Author:  Johannes Lange
//         Created:  Tue, 24 Feb 2015 14:13:05 GMT
//
//


// system include files
#include <memory>
#include <limits>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// ROOT
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>


//
// class declaration
//

class TriggerEfficiency : public edm::EDAnalyzer {
public:
   explicit TriggerEfficiency(const edm::ParameterSet&);
   ~TriggerEfficiency();

   static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


private:
   virtual void beginJob() override;
   virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
   virtual void endJob() override;

   //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
   //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
   //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
   //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

   // ----------member data ---------------------------
   std::string m_sSignalTriggerPath;
   std::string m_sControlTriggerPath;

   int m_iSignalTriggerIndex;
   int m_iControlTriggerIndex;

   edm::EDGetTokenT<edm::TriggerResults> m_triggerBits;
   edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> m_triggerObjects;
   edm::Service<TFileService> m_fs;

   // actual analysis data
   TEfficiency *m_eff;
   // TH1F m_eff;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
TriggerEfficiency::TriggerEfficiency(const edm::ParameterSet& iConfig)
   : m_sSignalTriggerPath (iConfig.getUntrackedParameter<std::string>( "signalTriggerPath"))
   , m_sControlTriggerPath(iConfig.getUntrackedParameter<std::string>("controlTriggerPath"))
   , m_triggerBits   (consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("bits")))
   , m_triggerObjects(consumes<pat::TriggerObjectStandAloneCollection>(iConfig.getParameter<edm::InputTag>("objects")))
{
   edm::LogInfo("Debug") <<   "signal  trigger: " << m_sSignalTriggerPath
                         << "\ncontrol trigger: " << m_sControlTriggerPath;
   m_eff=m_fs->make<TEfficiency>("eff","efficiency",200,0,500);
}


TriggerEfficiency::~TriggerEfficiency()
{
}


//
// member functions
//

// ------------ method called for each event  ------------
void
TriggerEfficiency::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   static bool s_bFirstEvent = true;

   edm::Handle<edm::TriggerResults> triggerBits;
   edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
   iEvent.getByToken(m_triggerBits, triggerBits);
   iEvent.getByToken(m_triggerObjects, triggerObjects);

   const edm::TriggerNames &names = iEvent.triggerNames(*triggerBits);
      
   // on first run: find trigger indices
   if (s_bFirstEvent){
      m_iControlTriggerIndex = m_iSignalTriggerIndex = -1;
      s_bFirstEvent=false;
      for (unsigned int i = 0; i < names.size(); ++i) {
         // if (names.triggerName(i).find("HLT_Photon")==0)edm::LogInfo("Debug")  << names.triggerName(i);
         if (names.triggerName(i).find(m_sSignalTriggerPath) == 0){ // starts with name
            m_iSignalTriggerIndex=i;
         }
         if (names.triggerName(i).find(m_sControlTriggerPath) == 0){ // starts with name
            m_iControlTriggerIndex=i;
         }
         if (m_iControlTriggerIndex!=-1 && m_iSignalTriggerIndex!=-1) break; // both set already
      }
   }

   if (!triggerBits->wasrun(m_iControlTriggerIndex) || !triggerBits->wasrun(m_iControlTriggerIndex)) return;

   if (triggerBits->accept(m_iControlTriggerIndex)){
      bool bSignalRun = triggerBits->accept(m_iSignalTriggerIndex);

      double dPhotonPt=std::numeric_limits<double>::min();
      for (pat::TriggerObjectStandAlone trObj : *triggerObjects){
	 trObj.unpackPathNames(names);
	 if (trObj.hasPathName(m_sControlTriggerPath+"*")){
	    dPhotonPt=std::max(dPhotonPt,double(trObj.pt())); // take pt of leading photon object
	 }
      }
      m_eff->Fill(bSignalRun,dPhotonPt);
   }

}


// ------------ method called once each job just before starting event loop  ------------
void
TriggerEfficiency::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
TriggerEfficiency::endJob()
{
   TGraphAsymmErrors* gr = m_fs->make<TGraphAsymmErrors>(*m_eff->CreateGraph());
   TString title="ST="+m_sSignalTriggerPath+", CT="+m_sControlTriggerPath;
   gr->SetTitle(title+";leading #gamma p_{T};efficiency");
   (void)gr; // suppress unused warning...
}

// ------------ method called when starting to processes a run  ------------
/*
  void
  TriggerEfficiency::beginRun(edm::Run const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when ending the processing of a run  ------------
/*
  void
  TriggerEfficiency::endRun(edm::Run const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
  void
  TriggerEfficiency::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
  void
  TriggerEfficiency::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TriggerEfficiency::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
   //The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TriggerEfficiency);

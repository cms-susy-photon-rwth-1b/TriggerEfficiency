import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:/user/lange/data/tmp/GR_R_72_V2_RelVal_photon2012D-v1/36C2ECB7-9230-E411-9C67-002618943933.root'
    )
)

process.demo = cms.EDAnalyzer('TriggerEfficiency',
                              signalTriggerPath=cms.untracked.string("HLT_Photon135_v"),
                              controlTriggerPath=cms.untracked.string("HLT_Photon30_v"),
                              rangeLow=cms.untracked.double(0.),
                              rangeUp=cms.untracked.double(500.),
                              nBins=cms.untracked.int32(200),
                              bits = cms.InputTag("TriggerResults","","HLT"),
                              objects = cms.InputTag("selectedPatTrigger")
)

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.MessageLogger.categories.append('Debug')

process.TFileService = cms.Service("TFileService",
  fileName = cms.string("trigger_efficiency.root")
)



process.p = cms.Path(process.demo)

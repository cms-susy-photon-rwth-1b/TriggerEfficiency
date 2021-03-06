# TriggerEfficiency
Measure Trigger Efficiency


## Building and Running ##
Get CMSSW environment 74X, source and build.

```
cmsrel CMSSW_7_4_0
cd CMSSW_7_4_0/src
cmsenv
git clone git@github.com:cms-susy-photon-rwth-1b/TriggerEfficiency.git
# or https://github.com/cms-susy-photon-rwth-1b/TriggerEfficiency.git
cd TriggerEfficiency
scram b
```

Run

```
cmsRun TriggerEfficiency/python/ConfFile_cfg.py
```

## Configure ##
* The triggerpaths given in the config file can be the full paths or truncated for wildcarding.
E.g. "HLT_Photon135_v" accpets any version.
* The binning and ranges of the histograms can be set via `rangeLow,rangeUp,nBins`

import FWCore.ParameterSet.Config as cms

from PhysicsTools.PFCandProducer.pfAllElectrons_cfi  import *
from PhysicsTools.PFCandProducer.pfElectronsPtGt5_cfi import *
from PhysicsTools.PFCandProducer.pfElectrons_cfi import *

pfElectronSequence = cms.Sequence(
    pfAllElectrons +
    pfElectronsPtGt5 +
    pfElectrons
    )





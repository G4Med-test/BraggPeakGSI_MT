//MACG 2018-09-01: Physics List version for G4MSBG regression tests:
//                 Considering reference physics list only.
//                 Particular production cuts for the ROI.


//==============================================================================
// The physics list must be activated by means of a *Reference Physics List*.
//    These provide a full set of models (both electromagnetic and hadronic).
//    Further information can be found whithin the Geant4 source code at:
//       $G4INSTALL/source/physics_lists/lists
//
//    A reference physics list can be set by just adding the line
//         /physics/referencePL <nameOfPackage>
//    in a macro file. No other action is required.
//    Examples: QGSP_BIC_HP or QGSP_BERT_HP
//
//==============================================================================

#include "PhysicsList.hh"

#include "PhysicsListMessenger.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysListFactory.hh"
#include "G4ProductionCuts.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4VPhysicsConstructor.hh"

// Particles
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "G4EmConfigurator.hh"
#include "G4LossTableManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"


//-------1---------2---------3---------4---------5---------6---------7---------8

PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
  G4LossTableManager::Instance();
  defaultCutValue = 0.5*mm;
  cutForGamma    = defaultCutValue;
  cutForElectron = defaultCutValue;
  cutForPositron = defaultCutValue;

  cutForGammaROI    = defaultCutValue;
  cutForElectronROI = defaultCutValue;
  cutForPositronROI = defaultCutValue;

  refPhysListIsRegistered = false;

  pMessenger = new PhysicsListMessenger(this);

  SetVerboseLevel(1);
}


//-----------------------------------------------------------------------------
PhysicsList::~PhysicsList()
{
  delete pMessenger;
}


//-------1---------2---------3---------4---------5---------6---------7---------8
void PhysicsList::AddPhysicsList(const G4String& name)
{
  if (!refPhysListIsRegistered) {
    G4PhysListFactory factory;
    G4VModularPhysicsList* phys =factory.GetReferencePhysList(name);
    G4int i=0;
    const G4VPhysicsConstructor* elem= phys->GetPhysics(i);

    G4VPhysicsConstructor* tmp = const_cast<G4VPhysicsConstructor*> (elem);
    while (elem !=0) {
      RegisterPhysics(tmp);
      G4cout << "PhysicsList Type: " << elem->GetPhysicsType() << G4endl;
      G4cout << "PhysicsList Name: " << elem->GetPhysicsName() << G4endl;
      elem= phys->GetPhysics(++i) ;
      tmp = const_cast<G4VPhysicsConstructor*> (elem);
    }

    G4cout << name << " Reference Physics List has been ACTIVATED."
	   << G4endl;

    // Update the flag to register this refPL's processes
    refPhysListIsRegistered = true;
  }
}


//-------1---------2---------3---------4---------5---------6---------7---------8
void PhysicsList::ConstructParticle()
{
  // This method is invoked when the Geant4 application starts
  // (do not mix with run initialization).

  // (Taken from G4DecayPhysics)
  G4BosonConstructor  pBosonConstructor;
  pBosonConstructor.ConstructParticle();

  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();

  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();
}


//-----------------------------------------------------------------------------
void PhysicsList::ConstructProcess()
{
  // If a Reference PL has been registered, use the method from
  // G4VModularPhysicsList (mother class)
  if (refPhysListIsRegistered) {
    G4VModularPhysicsList::ConstructProcess();
    G4cout << "Using G4VModularPhysicsList::ConstructProcess()" << G4endl;
  }

  else {
    // If not, throw a G4Exception
    G4Exception("PhysicsList::ConstructProcess()", "PhysList001",
		FatalException, "No PHYSICS LIST has been registered!");
  }
}


//-------1---------2---------3---------4---------5---------6---------7---------8
void PhysicsList::SetCuts()
{
  if (verboseLevel >0) {
    G4cout << "PhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }

  // Set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");

  //MAC  G4double lowlimit=250*eV;
  //MAC  G4ProductionCutsTable::GetProductionCutsTable()
  //                            ->SetEnergyRange(lowlimit, 100.*GeV);

  // ==================================================================
  // Definition of a smaller threshold of production in the ROI
  // where high accuracy is required in the energy deposit calculation
  // ==================================================================

  G4String regName;
  G4Region* region = 0;
  G4ProductionCuts* cuts = 0;

  regName = "ROI";
  region = G4RegionStore::GetInstance()->GetRegion(regName);
  cuts = new G4ProductionCuts;
  cuts->SetProductionCut(cutForGammaROI, G4ProductionCuts::GetIndex("gamma"));
  cuts->SetProductionCut(cutForElectronROI, G4ProductionCuts::GetIndex("e-"));
  cuts->SetProductionCut(cutForPositronROI, G4ProductionCuts::GetIndex("e+"));
  cuts->SetProductionCut(20.*um, G4ProductionCuts::GetIndex("proton"));
  region->SetProductionCuts(cuts);


  // ===================================
  // Dump table with all the cut values
  // ===================================

  if (verboseLevel>0) DumpCutValuesTable();
}


//-----------------------------------------------------------------------------
void PhysicsList::SetCutForGamma(G4double cut)
{
  cutForGamma = cut;
  SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}


//-----------------------------------------------------------------------------
void PhysicsList::SetCutForElectron(G4double cut)
{
  cutForElectron = cut;
  SetParticleCuts(cutForElectron, G4Electron::Electron());
}


//-----------------------------------------------------------------------------
void PhysicsList::SetCutForPositron(G4double cut)
{
  cutForPositron = cut;
  SetParticleCuts(cutForPositron, G4Positron::Positron());
}

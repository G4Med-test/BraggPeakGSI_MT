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

#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class PhysicsListMessenger;

class PhysicsList: public G4VModularPhysicsList
{
public:

  PhysicsList();
  virtual ~PhysicsList();

  void ConstructParticle();
  void ConstructProcess();

  void AddPhysicsList(const G4String& name);

  void SetCuts();

  void SetCutForGamma(G4double);
  void SetCutForElectron(G4double);
  void SetCutForPositron(G4double);

  void SetCutForGammaROI(G4double cut)    { cutForGammaROI = cut; }
  void SetCutForElectronROI(G4double cut) { cutForElectronROI = cut; }
  void SetCutForPositronROI(G4double cut) { cutForPositronROI = cut; }



private:

  PhysicsListMessenger* pMessenger;

  G4double cutForGamma;
  G4double cutForElectron;
  G4double cutForPositron;
  G4double cutForGammaROI;
  G4double cutForElectronROI;
  G4double cutForPositronROI;
  G4bool refPhysListIsRegistered;   // Reference physics list

};

#endif

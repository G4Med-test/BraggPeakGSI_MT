//MACG 2018-09-01: BraggPeakGSI_MT messenger class for regression tests.


#include "PhysicsListMessenger.hh"

#include "PhysicsList.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"


//-------1---------2---------3---------4---------5---------6---------7---------8

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* phys)
:thePL(phys)
{
  physDir = new G4UIdirectory("/physics/");
  physDir->SetGuidance("Commands to set physics models and production cuts");
   
  gammaCutCmd = new G4UIcmdWithADoubleAndUnit("/physics/gammaCut",this);  
  gammaCutCmd->SetGuidance("Set gamma cut.");
  gammaCutCmd->SetParameterName("GammaCut",false);
  gammaCutCmd->SetUnitCategory("Length");
  gammaCutCmd->SetRange("GammaCut > 0.0");
  gammaCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  electCutCmd = new G4UIcmdWithADoubleAndUnit("/physics/electronCut",this);  
  electCutCmd->SetGuidance("Set electron cut.");
  electCutCmd->SetParameterName("ElectronCut",false);
  electCutCmd->SetUnitCategory("Length");
  electCutCmd->SetRange("ElectronCut > 0.0");
  electCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  positCutCmd = new G4UIcmdWithADoubleAndUnit("/physics/positronCut",this);  
  positCutCmd->SetGuidance("Set positron cut.");
  positCutCmd->SetParameterName("PositronCut",false);
  positCutCmd->SetUnitCategory("Length");
  positCutCmd->SetRange("PositronCut > 0.0");
  positCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  

  gammaCutROICmd = 
    new G4UIcmdWithADoubleAndUnit("/physics/gammaCutInROI",this);
  gammaCutROICmd->SetGuidance("Set gamma cut within the ROI.");
  gammaCutROICmd->SetParameterName("GammaCutROI",false);
  gammaCutROICmd->SetUnitCategory("Length");
  gammaCutROICmd->SetRange("GammaCutROI > 0.0");
  gammaCutROICmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  electCutROICmd = 
    new G4UIcmdWithADoubleAndUnit("/physics/electronCutInROI",this);
  electCutROICmd->SetGuidance("Set electron cut within the ROI.");
  electCutROICmd->SetParameterName("ElectronCutROI",false);
  electCutROICmd->SetUnitCategory("Length");
  electCutROICmd->SetRange("ElectronCutROI > 0.0");
  electCutROICmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  positCutROICmd = 
    new G4UIcmdWithADoubleAndUnit("/physics/positronCutInROI",this);
  positCutROICmd->SetGuidance("Set positron cut within the ROI.");
  positCutROICmd->SetParameterName("PositronCutROI",false);
  positCutROICmd->SetUnitCategory("Length");
  positCutROICmd->SetRange("PositronCutROI > 0.0");
  positCutROICmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  physicsListCmd = new G4UIcmdWithAString("/physics/referencePL",this);  
  physicsListCmd->SetGuidance("Set the physics list.");
  physicsListCmd->SetParameterName("PList",false);
  physicsListCmd->AvailableForStates(G4State_PreInit);  

}


//-------1---------2---------3---------4---------5---------6---------7---------8

PhysicsListMessenger::~PhysicsListMessenger()
{
  delete physDir;    

  delete gammaCutCmd;
  delete electCutCmd;
  delete positCutCmd;
  delete gammaCutROICmd;
  delete electCutROICmd;
  delete positCutROICmd;
  delete physicsListCmd;
}


//-------1---------2---------3---------4---------5---------6---------7---------8

void PhysicsListMessenger::SetNewValue(G4UIcommand* command,
				       G4String newVal)
{
  if( command == gammaCutCmd )
   { thePL->SetCutForGamma(gammaCutCmd->GetNewDoubleValue(newVal));}
     
  else if( command == electCutCmd )
   { thePL->SetCutForElectron(electCutCmd->GetNewDoubleValue(newVal));}

  else if( command == positCutCmd )
   { thePL->SetCutForPositron(positCutCmd->GetNewDoubleValue(newVal));}

  else if( command == gammaCutROICmd )
   { thePL->SetCutForGammaROI(gammaCutROICmd->GetNewDoubleValue(newVal));}
     
  else if( command == electCutROICmd )
   { thePL->SetCutForElectronROI(electCutROICmd->GetNewDoubleValue(newVal));}

  else if( command == positCutROICmd )
   { thePL->SetCutForPositronROI(positCutROICmd->GetNewDoubleValue(newVal));}

  else if( command == physicsListCmd )
   { thePL->AddPhysicsList(newVal);}

}

//MACG 2012-10-18 : created

#include "globals.hh"

#include "MaterialsMessenger.hh"
#include "Materials.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"


//-------1---------2---------3---------4---------5---------6---------7---------8
// ----------------------------------------------------------------------------

MaterialsMessenger::MaterialsMessenger( Materials* myMat )
  : theMaterials( myMat )
{ 
  theMaterialsDir = new G4UIdirectory("/mymat/");
  theMaterialsDir->SetGuidance("Material settings.");

  theWaterDir = new G4UIdirectory("/mymat/water/");
  theWaterDir->SetGuidance("Choice of the water properties.");

  theWaterDensityCmd =
    new G4UIcmdWithADoubleAndUnit("/mymat/water/density", this);
  theWaterDensityCmd->SetGuidance("Set the density of water.");
  theWaterDensityCmd->SetParameterName("density", false);
  theWaterDensityCmd->SetUnitCategory("Volumic Mass");
  theWaterDensityCmd->SetDefaultUnit("kg/m3");
  theWaterDensityCmd->SetUnitCandidates("g/cm3 mg/cm3 kg/m3");
  theWaterDensityCmd->AvailableForStates(G4State_PreInit);

  theWaterIonPotCmd =
    new G4UIcmdWithADoubleAndUnit("/mymat/water/ionPot", this);
  theWaterIonPotCmd->SetGuidance("Set the mean ionization potential of water.");
  theWaterIonPotCmd->SetParameterName("ionPot", false);
  theWaterIonPotCmd->SetUnitCategory("Energy");
  theWaterIonPotCmd->SetDefaultUnit("eV");
  theWaterIonPotCmd->SetUnitCandidates("eV keV");
  theWaterIonPotCmd->AvailableForStates(G4State_PreInit);
}

// ----------------------------------------------------------------------------

MaterialsMessenger::~MaterialsMessenger()
{
  delete theMaterialsDir;
  delete theWaterDir;
  delete theWaterDensityCmd;
  delete theWaterIonPotCmd;
}

// ----------------------------------------------------------------------------

void MaterialsMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
  if (command == theWaterDensityCmd)
    { 
      theMaterials->SetWaterDensity(theWaterDensityCmd->GetNewDoubleValue(newValue));
    }

  else if (command == theWaterIonPotCmd)
    { 
      theMaterials->SetWaterIonPotential(theWaterIonPotCmd->GetNewDoubleValue(newValue));
    }
}

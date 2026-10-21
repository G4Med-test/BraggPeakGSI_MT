//MACG 2012-07-25 : my template class
//MACG 2012-10-19 : Geometry option command added.
//                    Cell commands added.
//MACG 2012-10-29 : Command to register ROIs added.
//MACG 2012-14-12 : Removed cellFirstZpos and nCells commands.
//MACG 2013-06-10: Adding commands to define the detailed scoring region.
//MACG 2013-06-11: Command added to set the spacing of cells out of detailed
//                  scoring region.


#include "globals.hh"

#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"


//-------1---------2---------3---------4---------5---------6---------7---------8
// ----------------------------------------------------------------------------

DetectorMessenger::DetectorMessenger( DetectorConstruction* myDet )
  : theDet( myDet )
{ 
  detectorDir = new G4UIdirectory( "/mygeom/" );
  detectorDir->SetGuidance("Detector control.");

  sensitiveLVsCmd = new G4UIcmdWithAString("/mygeom/addSensitiveLVs", this);
  sensitiveLVsCmd->SetGuidance("Type the string common to all the LVs");
  sensitiveLVsCmd->SetGuidance("to whom a MFD is going to be associated.");
  sensitiveLVsCmd->SetParameterName("CommonName", false);
  sensitiveLVsCmd->AvailableForStates(G4State_PreInit);

  roiCmd = new G4UIcmdWithAString("/mygeom/addROIs", this);
  roiCmd->SetGuidance("Type the string common to all the LVs to be registered");
  roiCmd->SetGuidance("in the Region Of Interest (ROI).");
  roiCmd->SetParameterName("CommonName", false);
  roiCmd->AvailableForStates(G4State_PreInit);

  geomOptionCmd = new G4UIcmdWithAnInteger("/mygeom/setup", this);
  geomOptionCmd->SetGuidance("Select the experimental setup. Please, use");
  geomOptionCmd->SetGuidance("an integer with any of these values:");
  geomOptionCmd->SetGuidance("  0: WaterTank setup.");
  geomOptionCmd->SetParameterName("choice", false);
  geomOptionCmd->AvailableForStates(G4State_PreInit);

  waterTankXYCmd = new G4UIcmdWithADoubleAndUnit("/mygeom/waterTankXY", this);
  waterTankXYCmd->SetGuidance("Set the transversal dimens. of the water tank.");
  waterTankXYCmd->SetParameterName("radius", false);
  waterTankXYCmd->SetDefaultUnit("mm");
  waterTankXYCmd->SetUnitCandidates("mm cm");
  waterTankXYCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  waterTankZCmd = new G4UIcmdWithADoubleAndUnit("/mygeom/waterTankZ", this);
  waterTankZCmd->SetGuidance("Set the transversal dimens. of the water tank.");
  waterTankZCmd->SetParameterName("radius", false);
  waterTankZCmd->SetDefaultUnit("mm");
  waterTankZCmd->SetUnitCandidates("mm cm");
  waterTankZCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  cellDir = new G4UIdirectory("/mygeom/cell/");
  cellDir->SetGuidance("Commands to define the cell properties");
  cellDir->SetGuidance("in the water tank.");

  cellRadiusCmd = new G4UIcmdWithADoubleAndUnit("/mygeom/cell/radius", this);
  cellRadiusCmd->SetGuidance("Set the cell radius (beam transverse direction)");
  cellRadiusCmd->SetParameterName("radius", false);
  cellRadiusCmd->SetDefaultUnit("mm");
  cellRadiusCmd->SetUnitCandidates("mm cm");
  cellRadiusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  cellHeightCmd = new G4UIcmdWithADoubleAndUnit("/mygeom/cell/height", this);
  cellHeightCmd->SetGuidance("Set the cell thickness along the beam direction");
  cellHeightCmd->SetParameterName("thickness", false);
  cellHeightCmd->SetDefaultUnit("mm");
  cellHeightCmd->SetUnitCandidates("mm cm");
  cellHeightCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  detailLowZCmd = 
    new G4UIcmdWithADoubleAndUnit("/mygeom/cell/detailedLowDepth", this);
  detailLowZCmd->SetGuidance("Set the lowest depth for the");
  detailLowZCmd->SetGuidance("detailed scoring region.");
  detailLowZCmd->SetParameterName("detailedLowDepth", false);
  detailLowZCmd->SetDefaultUnit("mm");
  detailLowZCmd->SetUnitCandidates("mm cm");
  detailLowZCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  detailHighZCmd = 
    new G4UIcmdWithADoubleAndUnit("/mygeom/cell/detailedHighDepth", this);
  detailHighZCmd->SetGuidance("Set the highest depth for the");
  detailHighZCmd->SetGuidance("detailed scoring region.");
  detailHighZCmd->SetParameterName("detailedHighDepth", false);
  detailHighZCmd->SetDefaultUnit("mm");
  detailHighZCmd->SetUnitCandidates("mm cm");
  detailHighZCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  cellSpacingCmd = new G4UIcmdWithAnInteger("/mygeom/cell/spacing", this);
  cellSpacingCmd->SetGuidance("Set the spacing between cells out of the");
  cellSpacingCmd->SetGuidance("detailed scoring region in units of cell.");
  cellSpacingCmd->SetParameterName("Spacing", false);
  cellSpacingCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
}

// ----------------------------------------------------------------------------

DetectorMessenger::~DetectorMessenger()
{
  delete detectorDir;
  delete sensitiveLVsCmd;
  delete roiCmd;
  delete geomOptionCmd;
  delete cellRadiusCmd;
  delete cellHeightCmd;
  delete detailLowZCmd;
  delete detailHighZCmd;
  delete cellSpacingCmd;
}

// ----------------------------------------------------------------------------

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
  if (command == sensitiveLVsCmd)
    { theDet->AddSensitiveLVs(newValue); }

  else if (command == roiCmd)
    { theDet->AddROIs(newValue); }

  else if (command == geomOptionCmd)
    { theDet->SetGeomOption(geomOptionCmd->GetNewIntValue(newValue)); }

  else if (command == waterTankXYCmd)
    { theDet->SetWaterTankXY(waterTankXYCmd->GetNewDoubleValue(newValue)); }

  else if (command == waterTankZCmd)
    { theDet->SetWaterTankZ(waterTankZCmd->GetNewDoubleValue(newValue)); }

  else if (command == cellRadiusCmd)
    { theDet->SetCellRadius(cellRadiusCmd->GetNewDoubleValue(newValue)); }

  else if (command == cellHeightCmd)
    { theDet->SetCellHeight(cellHeightCmd->GetNewDoubleValue(newValue)); }

  else if (command == detailLowZCmd)
    { theDet->SetDetailLowDepth(detailLowZCmd->GetNewDoubleValue(newValue)); }

  else if (command == detailHighZCmd)
    { theDet->SetDetailHighDepth(detailHighZCmd->GetNewDoubleValue(newValue)); }

  else if (command == cellSpacingCmd)
    { theDet->SetCellSpacing(cellSpacingCmd->GetNewIntValue(newValue)); }

}

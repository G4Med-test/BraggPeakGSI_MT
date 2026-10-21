//MACG 2018-09-05: DetectorConstruction class for regression test analysis.
//                 If possible, <SDname> = <LVname> + "SD".
//                 Geometry can be defined via theGeomOption.
//                 Scoring cells (tallies) are defined in WaterTank geometry.
//                 LVs are registered into ROI with AddROIs() method.


#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"

#include "globals.hh"

#include <vector>

class G4LogicalVolume;
class G4VPhysicalVolume;

class DetectorMessenger;
class Materials;

// ----------------------------------------------------------------------------

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:

  // Constructor and destructor
  DetectorConstruction();
  virtual ~DetectorConstruction();

  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();	

private:
  void DefineMaterials();
  void ConstructGeometry();
  void DefineRegions();
  void ConstructWaterTank();

public:

  void SetGeomOption(G4int choice) { theGeomOption = choice; }
  void SetWaterTankXY(G4double x) { theWaterTankXY = x;}
  void SetWaterTankZ(G4double z) { theWaterTankZ = z; }
  void SetCellRadius(G4double r) { theCellRadius = r; }
  void SetCellHeight(G4double h) { theCellHeight = h; }
  void SetDetailLowDepth(G4double ld) { theDetailLowDepth = ld; }
  void SetDetailHighDepth(G4double hd) { theDetailHighDepth = hd; }
  void SetCellSpacing(G4int Dz) { theCellSpacing = Dz; }

  G4int GetGeomOption() { return theGeomOption; }
  G4double GetCellRadius() { return theCellRadius; }
  G4double GetCellHeight() { return theCellHeight; }

  const std::vector<G4String>* GetMFDnames() const
  {return (const_cast<const std::vector<G4String>*>(theMFDnames));}

  void AddSensitiveLVs( const G4String& lvCommonName );
  void AddROIs( const G4String& lvCommonName );
  
 

private:

  // the World volume
  G4VPhysicalVolume* theWorld;

  // Messenger
  DetectorMessenger* theDetectorMessenger;

  // Materials
  Materials* theMaterials;

  // Vector with the MFDs' names
  std::vector<G4String>* theMFDnames;

  // Vector with LVs' common names for SD
  std::vector<G4String>* theSensitiveLVs;

  // Vector with LVs' common names for the Region Of Interest (ROI)
  std::vector<G4String>* theROIs;

  // The option to choose geometry setup
  G4int theGeomOption;

  //==== Option 0 ====
  // Water tank dimensions
  G4double theWaterTankXY;
  G4double theWaterTankZ;
  // Radius and thickness of scoring cells
  G4double theCellRadius;
  G4double theCellHeight;
  // Proximal and distal depths for detailed scoring,
  // cell spacing (in units of cells) out of detailed scoring region
  G4double theDetailLowDepth;
  G4double theDetailHighDepth;
  G4int theCellSpacing;
  // The cell LV
  G4LogicalVolume* lCell;

};

// ----------------------------------------------------------------------------

#endif

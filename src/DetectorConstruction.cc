//MACG 2018-09-05: DetectorConstruction class for regression test analysis.
//                 If possible, <SDname> = <LVname> + "SD".
//                 Geometry can be defined via theGeomOption.
//                 Scoring cells (tallies) are defined in WaterTank geometry.
//                 LVs are registered into ROI with AddROIs() method.


#include "DetectorConstruction.hh"

#include <vector>

// Application classes
#include "DetectorMessenger.hh"
#include "Materials.hh"
#
// Geant4 classes
// Globals
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
// Shapes
#include "G4Box.hh"
#include "G4Tubs.hh"
// Materials and volumes
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
// Regions
#include "G4Region.hh"
// MFD and SD
#include "G4MultiFunctionalDetector.hh"
#include "G4PSEnergyDeposit.hh"   // it's more logical for microdosim."
#include "G4SDManager.hh"

#include "SDParticleFilter.hh"   // My customized particle filter


//-------1---------2---------3---------4---------5---------6---------7---------8
// ----------------------------------------------------------------------------
//
// Constructor
//
DetectorConstruction::DetectorConstruction()
  : theWorld(0), 
    lCell(0)
{
  theMaterials = new Materials();
  theDetectorMessenger = new DetectorMessenger( this );
  theMFDnames = new std::vector<G4String>;
  theSensitiveLVs = new std::vector<G4String>;
  theROIs = new std::vector<G4String>;
  theGeomOption = 0;
  theWaterTankXY = 20.0*cm;
  theWaterTankZ = 40.0*cm;
  theCellRadius = 1.0*cm;
  theCellHeight = 1.0*mm;
  // By default, the detailed scoring region covers all the water tank
  theDetailLowDepth = 0.0;
  theDetailHighDepth = theWaterTankZ;
  theCellSpacing = 1;
}


// ----------------------------------------------------------------------------
//
// Destructor
//
DetectorConstruction::~DetectorConstruction()
{
  delete theDetectorMessenger;

  theMFDnames->clear();      delete theMFDnames;
  theSensitiveLVs->clear();  delete theSensitiveLVs;
  theROIs->clear();          delete theROIs;
}


// ----------------------------------------------------------------------------
//
// Constructs geometries, materials and MFDs
//
G4VPhysicalVolume* DetectorConstruction::Construct()
{ 
  // Template method
  DefineMaterials();
  ConstructGeometry();
  DefineRegions();
  //SetUpMFDetectors();

  return theWorld;
}

// ----------------------------------------------------------------------------
//
// Constructs geometries and materials
//
void DetectorConstruction::DefineMaterials()
{
  // Load the materials
  theMaterials->DefineMaterials();
}


// ----------------------------------------------------------------------------
//
// Construct materials and geometry according to 'theGeomOption'
//
void DetectorConstruction::ConstructGeometry()
{
  if (theGeomOption == 0) ConstructWaterTank();

  // Print the information of the materials
  G4cout << *(G4Material::GetMaterialTable() ) << G4endl;
}



// ----------------------------------------------------------------------------
//
// Construct Water Tank Setup
//
void DetectorConstruction::ConstructWaterTank()
{
  //-------------------------
  // Load each material here
  //-------------------------
  //  G4Material* air = theMaterials->GetMat("G4_AIR");
  G4Material* vacuum = theMaterials->GetMat("G4_Galactic");
  G4Material* water = theMaterials->GetMat("Water_RoomTemp");
  // G4Material* water = theMaterials->GetMat("G4_WATER");


  //-------------------------
  // Create the world volume
  //-------------------------
  G4double worldXY = 40.*cm;
  G4double worldZ = 1.*m;
  G4Box* sWorld = new G4Box("World", 
			       worldXY/2., worldXY/2., worldZ/2.);
  G4LogicalVolume* lWorld = new G4LogicalVolume(sWorld,
						vacuum,
						"World",
						0, 0, 0);
  theWorld = new G4PVPlacement(0,
			       G4ThreeVector(),
			       lWorld,
			       "World",
			       0, false, 0);


  //-----------------------------------
  // Construct the Water Tank with the
  // upper surface placed at Z=0 plane
  //-----------------------------------
  G4double waterTankXY = theWaterTankXY;
  G4double waterTankZ = theWaterTankZ;
  G4double waterTankZpos = +waterTankZ/2.;

  G4ThreeVector phWaterTankPos(0., 0., waterTankZpos);

  G4Box* sWaterTank = new G4Box("WaterTank",
				waterTankXY/2., waterTankXY/2., waterTankZ/2.);
  G4LogicalVolume* lWaterTank = new G4LogicalVolume(sWaterTank,
						    water,
						    "WaterTank",
						    0, 0, 0);
  //  G4VPhysicalVolume* phWaterTank = 0;
  //  phWaterTank =
  new G4PVPlacement(0, phWaterTankPos,
		    lWaterTank, "WaterTank", lWorld,
		    false, 0);

  //-----------------------//
  //  Creation of tallies  //
  //-----------------------//

  // Building the Region of Interest (ROI) (mother volume)
  // This is a container cylinder for all the cells. It covers all the
  // water tank along its Z axis. The fact of naming it ROI does not mean
  // than this volume is automatically defined as "ROI" region. Of course,
  // it can be defined so if desired.
  //-------------------------------------------------------
  G4double RoiRadius = theCellRadius;
  G4double RoiZ = waterTankZ;
  G4cout << "RoiZ = " << RoiZ/mm << " mm." << G4endl;

  G4double RoiXpos = 0.;
  G4double RoiYpos = 0.;
  G4double RoiZpos = 0.;
  G4cout << "RoiZpos = " << RoiZpos/mm << " mm." << G4endl;

  G4ThreeVector RoiPos(RoiXpos, RoiYpos, RoiZpos);

  G4Tubs* sRoi = new G4Tubs("Roi",
			    0., RoiRadius,
			    RoiZ/2.,
			    0., 360.*deg);
  G4LogicalVolume* lRoi = new G4LogicalVolume(sRoi,
					      water,
					      "Roi",
					      0, 0, 0);
  //  G4VPhysicalVolume* phRoi = 0;
  //  phRoi =
  new G4PVPlacement(0, RoiPos,
		    lRoi, "Roi", lWaterTank,
		    false, 0);

  // Building the cells into the ROI
  //---------------------------------

  // Print related information
  G4cout << "theCellRadius = " << theCellRadius/mm << " mm." << G4endl;
  G4cout << "theCellHeight = " << theCellHeight/mm << " mm." << G4endl;
  G4cout << "theDetailLowDepth = " << theDetailLowDepth/mm << " mm." 
	 << G4endl;
  G4cout << "theDetailHighDepth = " << theDetailHighDepth/mm << " mm." 
	 << G4endl;
  G4cout << "theCellSpacing = " << theCellSpacing << G4endl;

  // Copy the cell radius and height
  G4double cellRadius = theCellRadius;
  G4double cellHeight = theCellHeight;

  G4Tubs* sCell = new G4Tubs("Cell",
			     0., cellRadius,
			     cellHeight/2.,
			     0., 360.*deg);
  lCell = new G4LogicalVolume(sCell,
			      water,
			      "Cell",
			      0, 0, 0);

  G4int nOfCells = static_cast<G4int>(RoiZ/cellHeight);
  G4double cellZpos0 = -RoiZ/2. + cellHeight/2.;

  //  G4VPhysicalVolume* phCell = 0;
  for (G4int ii = 0; ii < nOfCells; ii++)
    {
      G4double cellZpos = cellZpos0 + static_cast<G4double>(ii)*cellHeight;

      // If 'cellZpos' is out of the detailed scoring region, create only
      // 1 out of 'theCellSpacing' cells.
      G4double lowRelativeDepth = -RoiZ/2. + theDetailLowDepth;
      G4double highRelativeDepth = -RoiZ/2. + theDetailHighDepth;

      if ( !(lowRelativeDepth < highRelativeDepth) )
	{
	  G4String msg = "theDetailLowDepth >= theDetailHighDepth";
	  G4Exception("DetectorConstruction::ConstructWaterTank()",
		      "DetConst001",FatalException,msg);
	}
      else if (theCellSpacing < 1)
	{
	  G4String msg = "theCellSpacing < 1";
	  G4Exception("DetectorConstruction::ConstructWaterTank()",
		      "DetConst002",FatalException,msg.c_str());
	}

      if ( ii % theCellSpacing == 0 ||
	   (cellZpos >= lowRelativeDepth && cellZpos <= highRelativeDepth) )
	{
	  G4ThreeVector cellPos(0., 0., cellZpos);

	  // IMPORTANT:
	  // Add the copy number to the PV name so that each position
	  // is automatically recorded in the output file.
	  std::stringstream sstr;
	  sstr << ii;
	  G4String strCopy(sstr.str());
	  G4String phCellName = lCell->GetName() + "_" + strCopy;

	  // phCell =
	  new G4PVPlacement(0, cellPos,
			    lCell, phCellName, lRoi,
			    false, ii);
	}
    }
}


// ----------------------------------------------------------------------------
//
// Define Regions
//
void DetectorConstruction::DefineRegions()
{
  // ==============================================
  //  Create the pointer to the Region Of Interest
  // ==============================================
  G4Region* regOfInterest = new G4Region("ROI");

  // =============
  //  Add the LVs
  // =============

  // Instatiation of the LV store to look for LV's defining the ROI
  //
  const G4LogicalVolumeStore* lvs = G4LogicalVolumeStore::GetInstance();
  std::vector<G4LogicalVolume*>::const_iterator lvciter;
  G4String lvName;

  // Using 'theROIs' to associate LV(s) to the ROI
  //
  std::vector<G4String>::iterator vsiter;
  G4String commonName;
  for (vsiter = theROIs->begin(); vsiter != theROIs->end(); vsiter++)
    {
      G4bool foundLV = false;
      commonName = (*vsiter);

      for (lvciter = lvs->begin(); lvciter != lvs->end(); lvciter++)
	{
	  lvName = (*lvciter)->GetName();
     
	  //if (lvName.G4StrUtil::contains(commonName))
    if (G4StrUtil::contains(lvName, commonName)) 
	    {
	      regOfInterest->AddRootLogicalVolume((*lvciter));
	      foundLV = true;
	      G4cout << "LV called \"" << lvName
		     << "\" is now registered within the ROI."
		     << G4endl;
	    }
	}

      // Warning in case of not finding LV's containing 'commonName'
      if ( !foundLV )
	{
	  G4String warnmsg = "NO LVs containing ";
	  warnmsg += commonName;
	  warnmsg += " were found.";
	  G4Exception("DetectorConstruction::DefineRegions()",
		      "Region001", JustWarning,
		      warnmsg.c_str() );
	}
    }
}


// ----------------------------------------------------------------------------
//
// Set up the MFDs
//
//void DetectorConstruction::SetUpMFDetectors()

//New method for the MT migration 12/07/2016 
void DetectorConstruction::ConstructSDandField()
{
  // =====================================================
  // MACG 28/11/09: once gotten the LV, I follow the
  // same instructions as given in examples RE02 and N07.
  // =====================================================

  // Sensitive Detector Manager
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  // Verbose level to see how the HCs are registered
  SDMan->SetVerboseLevel(1);


  // Instatiation of the LV store to look for LV's which are SD too
  //
  const G4LogicalVolumeStore* lvs = G4LogicalVolumeStore::GetInstance();
  std::vector<G4LogicalVolume*>::const_iterator lvciter;
  G4String lvName;

  // Using 'theSensitiveLVs' to associate MFD's with LV(s)
  //
  std::vector<G4String>::iterator vsiter;
  G4String commonName;
  for ( vsiter = theSensitiveLVs->begin(); vsiter != theSensitiveLVs->end(); 
	vsiter++ )
    {
      G4bool foundLV = false;
      commonName = (*vsiter);

      for( lvciter = lvs->begin(); lvciter != lvs->end(); lvciter++ )
	{
	  lvName = (*lvciter)->GetName();
	 // if (lvName.contains(commonName))
   if (G4StrUtil::contains(lvName, commonName)) 
	    {
	      // ----------------------------
	      // MultiFunctionalDetector
	      // ----------------------------

	      // Define the MFD name as the LV's name + "SD"
	      // This is important for my 'RunHitsGlobalPrinter' to identify
	      // the LV name, since it assumes that SDname = LVname + "SD".
	      G4String SDname = lvName + "SD";
	      G4MultiFunctionalDetector* aMFDet =  
		new G4MultiFunctionalDetector(SDname);
	     
              // Register the MFD in SDManager
	      SDMan->AddNewDetector( aMFDet );  

	      // Assign this MFD to this LV
	      (*lvciter)->SetSensitiveDetector( aMFDet );

	      // And register the MFD's name in 'theMFDnames'
	      //theMFDnames->push_back(SDname);
	     

	      G4cout << "SDname: " << SDname << G4endl;

	      // ----------------------------
	      // SD Filters
	      // ----------------------------

	      SDParticleFilter* primaryFilt = 
		new SDParticleFilter("primaryFilt");
	      primaryFilt->SetOnlyPrimaries(true);
	      
	      
	      // --------------------------
	      // PS : Primitive Scorers
	      // --------------------------

	      // Total energy deposited
	      // -----------------------
	      G4PSEnergyDeposit * energyDepPS
		= new G4PSEnergyDeposit("energyDep");
	      aMFDet->RegisterPrimitive(energyDepPS);
	      
	      

	      // Primaries
	      //
	      G4PSEnergyDeposit * energyDepPS_primary
		= new G4PSEnergyDeposit("energyDep_primary");
	      energyDepPS_primary->SetFilter(primaryFilt);
	      aMFDet->RegisterPrimitive(energyDepPS_primary);
	      

 	      foundLV = true;
	      G4cout << "LV called \"" << lvName
		     << "\" is now registered as a SD."
		     << G4endl;

	    }
	}

      // Warning in case of not finding LV's containing 'commonName'
      if ( !foundLV )
	{
	  G4String warnmsg = "NO LVs containing ";
	  warnmsg += commonName;
	  warnmsg += " were found.";
	  G4Exception("DetectorConstruction::SetUpMFDetectors()",
		      "SD001", JustWarning,
		      warnmsg.c_str() );
	}
    }

  SDMan->SetVerboseLevel(0);
}


// ----------------------------------------------------------------------------
//
// AddSensitiveLVs
//
void DetectorConstruction::AddSensitiveLVs( const G4String& commonName )
{
  // This method puts in 'theSensitiveLVs' the string which a LV name
  // must contain in order to be tagged as SD. Note that the LV name only
  // has to contain the string 'commonName' and not to be strictly equal.

  theSensitiveLVs->push_back(commonName);
}


// ----------------------------------------------------------------------------
//
// AddROIs
//
void DetectorConstruction::AddROIs( const G4String& commonName )
{
  // This method puts in 'theROIs' the string which a LV name
  // must contain in order to be registered within the Region of Interest. 
  // Note that the LV name only has to contain the string 'commonName' and 
  // not to be strictly equal.

  theROIs->push_back(commonName);
}

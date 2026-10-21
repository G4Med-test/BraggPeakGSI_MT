//MACG 2012-10-02: Materials class
//MACG 2013-07-15: Changed the name of water material.
//MACG 2018-09-17: Using G4_WATER from NIST for G4MSBG regression tests.


#include "Materials.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "MaterialsMessenger.hh"


//-------1---------2---------3---------4---------5---------6---------7---------8
// ----------------------------------------------------------------------------
Materials::Materials()
{
  theWaterDensity = 1.000*g/cm3;
  theWaterIonPotential = 78.0*eV;

  theMessenger = new MaterialsMessenger(this);
}


// ----------------------------------------------------------------------------
Materials::~Materials()
{
  delete theMessenger;
}


// ----------------------------------------------------------------------------
void Materials::DefineMaterials()
{
  // Define the material parameters required
  // ---------------------------------------
  G4String name;         // Material name
  //  G4double z;            // Atomic numebr
  //  G4double a;            // Atomic mass
  G4double d;            // Density
  G4int nComponents;     // Number of components 
  //  G4double fractionmass; // Fraction in mass of an element in a material
  //  G4int nAtoms;          // Number of atoms in a molecuule


  // NIST manager and void pointer to G4Material to be used below
  // to avoid warnings
  // -------------------------------------------------------------
  G4NistManager* man = G4NistManager::Instance();
  //  G4Material* materialPtr = 0;


  //  Definition of elements
  // ------------------------
  /* G4Element* H = */ man->FindOrBuildElement("H");   // Z = 1
  G4Element* C  = man->FindOrBuildElement("C");   // Z = 6
  G4Element* N  = man->FindOrBuildElement("N");   // Z = 7
  G4Element* O  = man->FindOrBuildElement("O");   // Z = 8
  G4Element* Si = man->FindOrBuildElement("Si");  // Z = 14
  G4Element* Ar = man->FindOrBuildElement("Ar");  // Z = 18


  // Definition of NIST materials
  // -----------------------------
  man->FindOrBuildMaterial("G4_Al");
  man->FindOrBuildMaterial("G4_AIR");
  man->FindOrBuildMaterial("G4_KAPTON");
  man->FindOrBuildMaterial("G4_WATER");  // standard water
  man->FindOrBuildMaterial("G4_Galactic");

  // Definition of "Water" with a different density and Ionization Potential
  //-------------------------------------------------------------------------

  // G4Material* H2O = new G4Material(name = "G4_WATER",
  // 				   d = theWaterDensity,
  // 				   nComponents = 2);
  // H2O->AddElement(H, 2);
  // H2O->AddElement(O, 1);
  // //  H2O->SetChemicalFormula("H_2O");
  // H2O->GetIonisation()->SetMeanExcitationEnergy(theWaterIonPotential);

  //
  // (Note: The following lines give actually the same result)
  //----------------------------------------------------------------------------
  
  man->BuildMaterialWithNewDensity("Water_RoomTemp","G4_WATER",
				   theWaterDensity);
  /* G4Material* Water = */ man->FindOrBuildMaterial("Water_RoomTemp");
  // Water->GetIonisation()->SetMeanExcitationEnergy(theWaterIonPotential);
  
  //----------------------------------------------------------------------------

  // Definition of materials via chemical composition
  // -------------------------------------------------
  G4Material* herasil = new G4Material(name = "HERASIL",
				       d = 2.20*g/cm3,
				       nComponents = 2);
  herasil->AddElement(Si, 1);
  herasil->AddElement( O, 2);

  G4Material* air = new G4Material(name = "Air",
				   d = 0.00120*g/cm3,
				   nComponents = 4,
				   kStateGas,
				   297.*kelvin);
  air->AddElement( C, 0.000124);
  air->AddElement( N, 0.755268);
  air->AddElement( O, 0.231781);
  air->AddElement(Ar, 0.012827);

  // Definition of materials as mixture of materials
  // ------------------------------------------------
  /* not needed */
}


// ----------------------------------------------------------------------------
G4Material* Materials::GetMat(G4String material)
{
  G4Material* pttoMaterial = G4Material::GetMaterial(material); 
  return pttoMaterial; 
}

//MACG 2018-09-05 : PrimaryGeneratorAction class for regression tests.
//                 This class assumes that the beam propagates along +Z axis.


#include "PrimaryGeneratorAction.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
//#include "G4ParticleGun.hh"
#include "G4IonTable.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"

#include "PrimaryGeneratorMessenger.hh"

#include <vector>


//-------1---------2---------3---------4---------5---------6---------7---------8
//-----------------------------------------------------------------------------
PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  // Define the messenger
  theMessenger = new PrimaryGeneratorMessenger(this);

  // Define particle generator
  theGPS = new G4GeneralParticleSource();
  //  particleGun  = new G4ParticleGun();

  // Default beam parameters
  SetDefaultParameters();
  SetDefaultPrimaryParticle();
}  


//-----------------------------------------------------------------------------
PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  //  delete particleGun;
  delete theGPS;
  delete theMessenger;
}


//-----------------------------------------------------------------------------
void PrimaryGeneratorAction::SetDefaultParameters()
{
  // Default source ID
  theSourceID = 0;
  theEneID = 0;
  thePosID = 0;
  theAngID = 0;

  // Default energy distribution: 1 MeV monoenergetic
  meanKineticEnergy = 1.*MeV;
  sigmaEnergy = 0.*MeV;

  // Default beam shape: Point source at origin of coordinates
  X0 = 0.0;
  Y0 = 0.0;
  Z0 = 0.0;
  sigmaRadiusX = 0.0;
  sigmaRadiusY = 0.0;
  radiusX = 0.0;
  radiusY = 0.0;
  dX = 0.0;
  dY = 0.0;

  // Default momentum direction distribution: only +Z axis,
  sigmaMomentumX = 0.0;
  sigmaMomentumY = 0.0;
}
  

//-----------------------------------------------------------------------------
void PrimaryGeneratorAction::SetDefaultPrimaryParticle()
{
  // Define primary particle: geantino
  //-----------------------------------
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("geantino");

 
  // General Particle Source default setup:
  //  Monoenergetic and plane wave (source ID 000)
  //-----------------------------------------------
  theGPS->SetParticleDefinition(particle);
  theGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
  theGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
  theGPS->GetCurrentSource()->GetAngDist()->SetAngDistType("planar");
  //  particleGun->SetParticleDefinition(particle); 
}


//-----------------------------------------------------------------------------
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
 
  
  // ====================
  // ENERGY DISTRIBUTION
  // ====================
  
  if (theEneID == 0) {
    // 
    // Default energy distribution:
    // Set the beam energy and Gaussian energy spread
    // -----------------------------------------------

    G4double kineticEnergy = meanKineticEnergy;

    if (sigmaEnergy > 0.0)
      {
	G4double randEne = 0.0;
	// This condition avoids too large deviations
	do {
	  randEne = G4RandGauss::shoot();
	} while ( randEne < -3. || randEne > +3. );
	  
	randEne *= sigmaEnergy;
	kineticEnergy += randEne;
      }
      
    theGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy( kineticEnergy );
    //      particleGun->SetParticleEnergy( kineticEnergy );
  }

  // ======================
  // POSITION DISTRIBUTION
  // ======================

  if ( thePosID == 0 ) {
    //
    // Default position distribution:
    // Set the beam spot size (Gaussian shape)
    // ----------------------------------------
      
    // Set the position of the primary particles
    G4double x = X0;
    G4double y = Y0;
    G4double z = Z0;
      
    if ( sigmaRadiusX > 0.0 ) {
      G4double randX = 0.0;
      // This condition avoids too large deviations
      do {
	randX = G4RandGauss::shoot();
      } while ( randX < -3. || randX > +3. );
	
      randX *= sigmaRadiusX;
      x += randX;
    }

    if ( sigmaRadiusY > 0.0 ) {
      G4double randY = 0.0;
      // This condition avoids too large deviations
      do {
	randY = G4RandGauss::shoot();
      } while ( randY < -3. || randY > +3. );

      randY *= sigmaRadiusY;
      y += randY;
    }
      
    theGPS->GetCurrentSource()->GetPosDist()->
      SetCentreCoords(G4ThreeVector(x, y, z));
    //      particleGun->SetParticlePosition(G4ThreeVector(x, y, z));
  }

  else if ( thePosID == 1 ) {
    //
    // Circular/elliptical uniformly distributed
    // ------------------------------------------
    // Set the position of the primary particles
    G4double x = X0;
    G4double y = Y0;
    G4double z = Z0;
      
    if ( radiusX > 0.0 || radiusY > 0.0) {
      G4double randX = 0.0;
      G4double randY = 0.0;

      do {
	randX = 2.*G4UniformRand()-1;
	randY = 2.*G4UniformRand()-1;
      } while ( (randX*randX + randY*randY) > 1);
	
      randX *= radiusX;
      randY *= radiusY;
      x += randX;
      y += randY;
    }

    theGPS->GetCurrentSource()->GetPosDist()
      ->SetCentreCoords(G4ThreeVector(x, y, z));
  }

  // =====================
  // ANGULAR DISTRIBUTION
  // =====================

  if (theAngID == 0) {
    //
    // Default angular distribution:
    // Set constant direction or gaussian opening
    // -------------------------------------------

    G4double momentumX = 0.0;
    G4double momentumY = 0.0;
    G4double momentumZ = 1.0;

    if ( sigmaMomentumX  > 0.0 )
      momentumX += G4RandGauss::shoot( 0., sigmaMomentumX );

    if ( sigmaMomentumY  > 0.0 )
      momentumY += G4RandGauss::shoot( 0., sigmaMomentumY );

    theGPS->GetCurrentSource()->
      GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(momentumX,
							       momentumY,
							       momentumZ));
    //particleGun->SetParticleMomentumDirection( G4ThreeVector(momentumX,
    //                					 momentumY,
    //							 momentumZ) );  
  }

  // ===================
  // GENERATE PRIMARIES
  // ===================
  theGPS->GeneratePrimaryVertex( anEvent );
  //    particleGun->GeneratePrimaryVertex( anEvent );


}



//-----------------------------------------------------------------------------
void PrimaryGeneratorAction::SetParticle(G4String name)
{
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle(name);
  theGPS->SetParticleDefinition(particle);
  //  particleGun->SetParticleDefinition(particle);
}


//-----------------------------------------------------------------------------
void PrimaryGeneratorAction::SetIon(G4int Z, G4int A, G4double E = 0.0)
{
  G4ParticleDefinition* ion = 0;
  G4IonTable* ionTable = G4IonTable::GetIonTable();
  ion = ionTable->GetIon(Z, A, E);

  if (ion == 0)
    {
      G4cout << "Ion with Z = " << Z << ", A = " << "is not defined!"; 
      G4cout << G4endl;
      G4Exception("PrimaryGeneratorAction::SetIon()",
		  "BadIonDef", FatalException,
		  "Unknown definition of ion.");
    }
  else
    {
      theGPS->SetParticleDefinition(ion);
    }
}



//-----------------------------------------------------------------------------
void PrimaryGeneratorAction::SetSource(G4int source)
{
  theSourceID = source;

  // Get the units, tens and hundreds of theSourceID
  // and store into the corresponding data members
  //-------------------------------------------------
  theAngID = (source % 10);
  thePosID = ((source/10) % 10);
  theEneID = source/100;          // 3-digit integer
  
  // Set the energy, position and angular distribution
  // of the radiation beam accordingly.
  //---------------------------------------------------
  
  SetEneDist(theEneID);
  SetPosDist(thePosID);
  SetAngDist(theAngID);
  
}


//-----------------------------------------------------------------------------
void PrimaryGeneratorAction::SetEneDist(G4int eneID)
{
  // Type of energy distribution
  //=============================

  if (eneID == 0) {
    // Default source using GPS.
    //===========================
    theGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
    // The logic is implemented in GeneratePrimaries()
  }

  else {
    // Energy distribution ID not foreseen.
    //======================================
	G4Exception("PrimaryGeneratorAction::SetEneDist()",
		    "UnknownEneID001",
		    FatalException,
		    "ERROR: Unknown energy source ID.");
  }

}


//-----------------------------------------------------------------------------
void PrimaryGeneratorAction::SetPosDist(G4int posID)
{
  // Type of position distribution
  //===============================

  if (posID == 0) {
    // Default source
    //================
    theGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    // The logic is implemented in GeneratePrimaries()
  }

  else if (posID == 1) {
    // Uniform distribution with circular/elliptical shape
    //=====================================================
    theGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point"); 
    // The logic is implemented in GeneratePrimaries()
  }

  else if (posID == 2) {
    // Square/Rectangular plane
    //==========================
    theGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Plane");
    theGPS->GetCurrentSource()->GetPosDist()->SetPosDisShape("Rectangle");
    G4ThreeVector centralP = G4ThreeVector(X0, Y0, Z0);
    theGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(centralP);
    theGPS->GetCurrentSource()->GetPosDist()->SetHalfX(dX/2);
    theGPS->GetCurrentSource()->GetPosDist()->SetHalfY(dY/2);
  }

  else {
    // Position distribution ID not foreseen
    //=======================================
	G4Exception("PrimaryGeneratorAction::SetPosDist()",
		    "UnknownPosID001",
		    FatalException,
		    "ERROR: Unknown position source ID.");
  }
}


//-----------------------------------------------------------------------------
void PrimaryGeneratorAction::SetAngDist(G4int angID)
{
  // Type of angular distribution
  //==============================

  if (angID == 0) {
    // Default source
    // ===============
    theGPS->GetCurrentSource()->GetAngDist()->SetAngDistType("planar");    
    // The logic is implemented in GeneratePrimaries()
  }

  else {
    // Angular distribution ID not foreseen
    //======================================
	G4Exception("PrimaryGeneratorAction::SetAngDist()",
		    "UnknownAngID001",
		    FatalException,
		    "ERROR: Unknown angular source ID.");
  }
}

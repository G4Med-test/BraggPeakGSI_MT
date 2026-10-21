//MACG 2012-07-25 : my 'RunHitsGlobalPrinter' template class.
//                  It assumes that a 'Run' class derived from G4Run has been
//                  created in order to sum the scorers throughout the run.
//                  It also retrieves the positioning information of each PV
//                  copied from a LV tagged as SD, but reffered to the Mother
//                  Volume Local Frame and not to the Global Frame.
//MACG 2012-11-28 : Assuming that always <PVname> = <LVname>+"_"+<copyID>
//                  even for cases where only one copy of the LV is present.
//MACG 2018-09-16: Added messenger class.
//                 RunID is NOT in file name if not using default ("runMaps").



#include "RunHitsGlobalPrinter.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Run.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4THitsMap.hh"

#include "Run.hh"
#include "RunHitsGlobalPrinterMessenger.hh"

#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <stdlib.h> 

//-------1---------2---------3---------4---------5---------6---------7---------8
RunHitsGlobalPrinter::RunHitsGlobalPrinter()
{
  theFileName = "runMaps";
  fIsDefaultFileName = true;
  theMessenger = new RunHitsGlobalPrinterMessenger(this);
}


RunHitsGlobalPrinter::RunHitsGlobalPrinter(G4String fileName)
{
  theFileName = fileName;
  fIsDefaultFileName = false;
  theMessenger = new RunHitsGlobalPrinterMessenger(this);
}


RunHitsGlobalPrinter::~RunHitsGlobalPrinter()
{
  delete theMessenger;
}


//-------1---------2---------3---------4---------5---------6---------7---------8
#ifdef G4MULTITHREADED
void RunHitsGlobalPrinter::DumpAll(G4int threadID)
#else
void RunHitsGlobalPrinter::DumpAll()
#endif
{
  // Template method
#ifdef G4MULTITHREADED
  OpenFile(threadID);
#else
  OpenFile();
#endif
  PrintHeader();
  PrintMaps();
#ifdef G4MULTITHREADED
  CloseFile(threadID);
#else
  CloseFile();
#endif
}


//-------1---------2---------3---------4---------5---------6---------7---------8
#ifdef G4MULTITHREADED
void RunHitsGlobalPrinter::OpenFile(G4int threadID)
#else
void RunHitsGlobalPrinter::OpenFile()
#endif
{
  G4String fullName;
#ifdef G4MULTITHREADED
  std::stringstream ssThread;
  ssThread << threadID;
#endif

  if (fIsDefaultFileName) {
    // The file has a suffix that indicates the run ID
#ifdef G4MULTITHREADED
    G4int runID = G4MTRunManager::GetRunManager()->GetCurrentRun()->GetRunID();
#else
    G4int runID = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
#endif
    std::stringstream ssRun;
    ssRun << runID;
#ifdef G4MULTITHREADED
    fullName = theFileName + "_" + ssRun.str() + ".out_t" + ssThread.str();
#else
    fullName = theFileName + "_" + ssRun.str() + ".out";
#endif
  }
  else {
#ifdef G4MULTITHREADED
    fullName = theFileName + ".out_t" + ssThread.str();
#else
    fullName = theFileName + ".out";
#endif
  }

  theOutputFile.open( fullName.c_str() );
}


//-------1---------2---------3---------4---------5---------6---------7---------8
void RunHitsGlobalPrinter::PrintHeader()
{
  // This class assumes that a 'Run' object stores the scoring information.

  const Run* extRun = reinterpret_cast<const Run*>
#ifdef G4MULTITHREADED
    (G4MTRunManager::GetRunManager()->GetCurrentRun());
#else
    (G4RunManager::GetRunManager()->GetCurrentRun());
#endif

  // Print the number of INDEPENDENT events.
  // ATTENTION: recycled particles share the same event, and in order 
  // to normalize properly they must be maken into account elsewhere.
  G4int nEvt = extRun->GetNumberOfEvent();
  G4int nMaps = extRun->GetNumberOfHitsMap();

  theOutputFile << "# nEvt = " << nEvt << std::endl;
  theOutputFile << "# nMaps = " << nMaps << std::endl;
  theOutputFile << "# ----------" << std::endl;
}


//-------1---------2---------3---------4---------5---------6---------7---------8
void RunHitsGlobalPrinter::PrintMaps()
{
  const Run* extRun = reinterpret_cast<const Run*>
#ifdef G4MULTITHREADED
    (G4MTRunManager::GetRunManager()->GetCurrentRun());
#else
    (G4RunManager::GetRunManager()->GetCurrentRun());
#endif
  G4int nMaps = extRun->GetNumberOfHitsMap();

  // Initialize hits maps copies
  G4THitsMap<G4double>* hitsMap = 0;
  G4THitsMap<G4double>* errorMap = 0;

  for (G4int ii = 0; ii < nMaps; ii++)
    {
      hitsMap = extRun->GetHitsMap(ii);
      if (hitsMap)
	{
	  errorMap = extRun->GetHitsErrorMap(ii);

	  G4String sdName = hitsMap->GetSDname();
	  G4String collName = hitsMap->GetName();
	  G4int nEntries = hitsMap->entries();

	  // Retrieving the LV to get the coordinates of each PV copy.
	  // NOTE: this strategy only works if SDname = LVname + "SD"
	  G4String lvName = sdName.substr(0, sdName.length()-2);
	  G4LogicalVolume* sdLV = 
	    G4LogicalVolumeStore::GetInstance()->GetVolume(lvName);

	  theOutputFile << "# runHitMapID = " << ii << std::endl;
	  theOutputFile << "# fullName = " << sdName << "/" << collName 
			<< std::endl;
	  theOutputFile << "# no. entries = " << nEntries << std::endl;
	  theOutputFile << "# copyID\tSumW\tSumW2\tx(mm)\ty(mm)\tz(mm)" 
			<< std::endl;

	  std::map<G4int, G4double*>::iterator mapit;
	  for (mapit = hitsMap->GetMap()->begin();
	       mapit != hitsMap->GetMap()->end(); mapit++)
	    {
	      G4int key = mapit->first;
	      G4double sumW = *(mapit->second);
	      G4double sumW2;
	      if (errorMap)
		sumW2 = *((*errorMap)[key]);
	      else
		sumW2 = 0;

	      // Obtaining coordinates from the corresponding PV
	      // NOTE: it assumes that 
	      //       PVname = LVname + "_" + copyNo (even with 1 LV copy!)
	      // This has to be respected strictly in DetectorConstruction!
	      // Otherwise, the positions will not be retrieved.
	      G4String pvName = lvName;
	      G4VPhysicalVolume* sdPV = 0;
	      if (sdLV)
		{
		  std::stringstream sstr;
		  sstr << key;
		  G4String strCopy(sstr.str());
		  pvName = lvName + "_" + strCopy;
		  sdPV =
		    G4PhysicalVolumeStore::GetInstance()->GetVolume(pvName);
		}

	      // Dump the information into the output file
	      theOutputFile << key << "\t"
			    << sumW << "\t" << sumW2 << "\t";

	      // if obtained the sdPV, get the distance from the
	      // MOTHER VOLUME upper surface (depth)!!
	      // NOTE: This part of the code assumes the concrete geometry 
	      // of this 'BraggPeakGSI' application, option 0.
	      if (sdPV)  {
		G4Tubs* mother =
		  reinterpret_cast<G4Tubs*>( sdPV->GetMotherLogical()
					     ->GetSolid());
	      G4ThreeVector pvPos = sdPV->GetTranslation();
	      G4double zPos = pvPos.z();
	      zPos += mother->GetZHalfLength();
	      theOutputFile << pvPos.x()/mm << "\t"
			    << pvPos.y()/mm << "\t"
			    << zPos/mm;
	      }
	      theOutputFile << std::endl;
	    }
	}
      else
	{
	  G4Exception("RunHitsGlobalPrinter::PrintMaps()",
		      "Exception", JustWarning, "No hitsMap obtained.");
	}
      // A double-line space to make a Gnuplot-compliant file
      theOutputFile << std::endl << std::endl;
    }
}


//-------1---------2---------3---------4---------5---------6---------7---------8
#ifdef G4MULTITHREADED
void RunHitsGlobalPrinter::CloseFile(G4int threadID)
#else
void RunHitsGlobalPrinter::CloseFile()
#endif
{
  theOutputFile.close();

  if (fIsDefaultFileName) {
#ifdef G4MULTITHREADED
    G4int runID = G4MTRunManager::GetRunManager()->GetCurrentRun()->GetRunID();
#else
    G4int runID = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
#endif
    G4cout << "SumW (and W2) run maps written in \"" << theFileName << "_"
#ifdef G4MULTITHREADED
	   << runID << ".out_t" << threadID << "\"." << G4endl;
#else
	   << runID << ".out" << "\"." << G4endl;
#endif
  }
  else {
    G4cout << "SumW (and W2) run maps written in \"" << theFileName
#ifdef G4MULTITHREADED
	   << ".out_t" << threadID << "\"." << G4endl;
#else
	   << ".out" << "\"." << G4endl;
#endif
  }
}

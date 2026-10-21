#include "RunAction.hh"

#include "G4Run.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"

#include "Run.hh"
#include "RunHitsGlobalPrinter.hh"
 
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>



//-------1---------2---------3---------4---------5---------6---------7---------8
RunAction::RunAction()
{
  theRunHitsGlobalPrinter = new RunHitsGlobalPrinter();
}


RunAction::~RunAction()
{
  // delete the printer
  delete theRunHitsGlobalPrinter;
}


//-----------------------------------------------------------------------------
G4Run* RunAction::GenerateRun()
{
  // Generate new RUN object, which is specially
  // dedicated for MultiFunctionalDetector scheme.
  //  Detailed description can be found in Run.hh/cc.

  return new Run();
}


//-----------------------------------------------------------------------------
void RunAction::BeginOfRunAction(const G4Run* /*aRun*/)
{
}


//-----------------------------------------------------------------------------
void RunAction::EndOfRunAction(const G4Run* /*aRun*/)
{
#ifdef G4MULTITHREADED
  if (IsMaster()) {
#else
  if (true) {
#endif
    G4String fileName = theRunHitsGlobalPrinter->GetFileName();
    G4bool isDefaultName = theRunHitsGlobalPrinter->GetIsDefaultFileName();

#ifdef G4MULTITHREADED
    G4int runID = G4MTRunManager::GetRunManager()->GetCurrentRun()->GetRunID();
#else
    G4int runID = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
#endif
    std::stringstream ssRun;
    ssRun << runID;

#ifdef G4MULTITHREADED
    G4int ncores = G4MTRunManager::GetMasterRunManager()->GetNumberOfThreads();
#else
    G4int ncores = 1; // G4RunManager::GetRunManager()->GetNumberOfThreads();
#endif

#ifndef G4MULTITHREADED
    theRunHitsGlobalPrinter->DumpAll();
#endif
    for (G4int ii = 0; ii < ncores ; ii++) {
      std::stringstream ssThread;
      ssThread << ii;
      G4String name;
#ifdef G4MULTITHREADED
      if (isDefaultName)
	name = fileName + "_" + ssRun.str() + ".out_t" + ssThread.str();
      else
	name = fileName + ".out_t" + ssThread.str();
#else
  name = fileName + ".out";
#endif
      theFileNames.push_back(name);
    }

    MergeGlobalFile();
    // GetRunMap_energy(fileName);

    // Clean 'theFileNames' once merge has been completed
    theFileNames.clear();

  }
  else {
#ifdef G4MULTITHREADED
    theRunHitsGlobalPrinter->DumpAll(G4Threading::G4GetThreadId());
#else
    theRunHitsGlobalPrinter->DumpAll();
#endif
  }

}


//Method for the global output file construction (runMaps_<runID>.out)
//------------------------------------------------------------------------------
void RunAction::MergeGlobalFile()
{
  G4String fileRootName = theRunHitsGlobalPrinter->GetFileName();
  G4bool isDefaultName = theRunHitsGlobalPrinter->GetIsDefaultFileName();
  G4String outFileName;

#ifdef G4MULTITHREADED
  if (isDefaultName) {
#else
  if (true) {
#endif
#ifdef G4MULTITHREADED
    G4int runID = G4MTRunManager::GetRunManager()->GetCurrentRun()->GetRunID();
#else
    G4int runID = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
#endif
    std::stringstream sstr;
    sstr << runID;
    outFileName = fileRootName + "_" + sstr.str() + ".out";
  }
  else {
    outFileName = fileRootName + ".out";
  }

  // ==================
  //  Global variables
  // ==================

  // Accumulative variables (i.e. updated by all files)
  G4double accumEvt = 0;
  std::vector< std::map<G4int,G4double>* > accumSumWmapVec, accumSumW2mapVec;
  std::map<G4int, std::map<G4int,G4String>* > infoMap2;
  // The info is associated per line

  // "Static" variables (i.e. set by the first file)
  size_t nMaps = 0;
  std::vector<G4int> runHitMapIdVec;
  std::vector<G4String> fullNameVec;
  std::vector<G4String> headerLineVec;


  // ===================================================
  //  Loop over all the files: Read and get information
  // ===================================================

  for (size_t ii = 0; ii < theFileNames.size(); ii++) {
    // ------------------------------
    // Global variables in this loop
    // ------------------------------
    G4double nEvt = 0;
    std::vector< std::map<G4int,G4double>* > sumWmapVec, sumW2mapVec;
    std::map<G4int, G4double>* sumWmap = 0;  // one per file.
    std::map<G4int, G4double>* sumW2map = 0; // one per file.
    std::map<G4int,G4String>* infoMap = 0;   // only created once.

    G4String inputFile = theFileNames[ii];

    std::ifstream fe( inputFile.c_str() );
    if ( fe.is_open() ) {
      G4cout << "\t Reading \"" << inputFile << "\"" << G4endl;

      // Global variables for this file
      // -------------------------------
      G4String str;
      G4int hitMapID = -1;      // Current hitMapID
      G4bool collFinished = true;

      // Reading all the file
      // ---------------------
      while (!fe.eof())  {
	getline(fe, str);

	if (str[0] == '#')  {
	  // if '#' is present, it is a commented data line.
	  // Now only some lines are needed.
	  // --------------------------------------------------
	  if (str.find("nEvt") != std::string::npos) {
	    size_t pos = str.rfind('=');
	    if (pos != std::string::npos) {
	      G4String str2;
	      str2 = str.substr(pos+1);
	      nEvt = static_cast<double>( atoi(str2.c_str()) ); 
	    }
	  }
	  else if (str.find("nMaps") != std::string::npos) {
	    size_t pos = str.rfind('=');
	    if (pos != std::string::npos) {
	      G4String str2;
	      str2 = str.substr(pos+1);
	      nMaps = atoi(str2.c_str());

	      // Once known how many maps there are,
	      // reserve space for them.
	      sumWmapVec.reserve(nMaps);
	      sumW2mapVec.reserve(nMaps);
	      if (ii == 0) {
		runHitMapIdVec.reserve(nMaps);
	      }
	    }
	  }
	  else if (str.find("runHitMapID") != std::string::npos) {
	    size_t pos = str.rfind('=');
	    if (pos != std::string::npos) {
	      G4String str2;
	      str2 = str.substr(pos+1);
	      hitMapID = atoi(str2.c_str());

	      if (ii == 0) {   // store this only once.
		runHitMapIdVec.push_back(hitMapID);
	      }

	      // Now a hit map is being read (flag false) and
	      // create its corresponding object dynamically.
	      collFinished = false;
	      sumWmap = new std::map<G4int, G4double>;
	      sumW2map = new std::map<G4int, G4double>;
	      if (ii == 0) {   // infoMap created only once.
		infoMap = new std::map<G4int, G4String>;
		infoMap2.insert(std::pair<G4int,std::map<G4int,G4String>* >
				(hitMapID, infoMap));
	      }
	    }
	  }
	  else if (str.find("fullName") != std::string::npos) {
	    // Read hit map name only from the 1st file.
	    if (ii == 0) {
	      size_t pos = str.rfind('=');
	      if (pos != std::string::npos) {
		G4String str2;
		str2 = str.substr(pos+1);
		fullNameVec.push_back(str2);
	      }
	    }
	  }
	  else if (str.find("SumW") != std::string::npos) {
	    // This is the header line
	    // Take only the info after last occurrence of "SumW"
	    if (ii == 0) {  // Only do this once
	      size_t pos = str.rfind("SumW");
	      G4String str2 = str.substr(pos+5); // Lenght of "SumW2"
	      headerLineVec.push_back(str2);
	    }

	    // // This is the header line describing columns
	    // // Just copy it.
	    // if (ii == 0) {  // Only do this once
	    //   headerLineVec.push_back(str);
	    // }
	  }
	}
	else if (str.size() > 0) {
	  // If no # present, we should have <ID sumW sumW2 [info]>
	  // ------------------------------------------------------
	  std::stringstream oss;
	  G4int copyID = -1;
	  G4double w = 0;
	  G4double w2 = 0;
	  G4String info;
 
	  // Reinterpret string to three numeric variables
	  oss << str;
	  oss >> copyID >> w >> w2;
	  getline(oss, info);
	  // cout << str << endl;
	  // cout << copyID << "\t" << w << "\t" << w2 << "\t" 
	  //      << info << "\t" << collFinished << endl;

	  // Store in current map
	  sumWmap->insert(std::pair<G4int,G4double>(copyID, w) );
	  sumW2map->insert(std::pair<G4int,G4double>(copyID, w2));
	  // cout << "sumWmap->size()=" << sumWmap->size()
	  //      << endl;

	  // The info is associated with copyID and mapID.
	  (infoMap2[hitMapID])->insert(std::pair<G4int,G4String>(copyID, info));
	}
	else if (!collFinished) {
	  // Blank line which indicates end of entries for current map
	  // ---------------------------------------------------------
	  sumWmapVec.push_back(sumWmap);
	  sumW2mapVec.push_back(sumW2map);
	  collFinished = true;             // there are two blank lines
	  // cout << "collFinished=" << collFinished
	  //      << endl;
	  // cout << "sumWmapVec.size()="
	  //      << sumWmapVec.size() << endl;
	}

      } // Matches "while (!fe.eof())"
    }
    else {
      std::cerr << "ERROR opening file " << inputFile << std::endl;
    }

    fe.close();

    // -----------------------------------------------
    //  Appending the variables for data accumulation
    // -----------------------------------------------

    accumEvt += nEvt;

    // When reading the 1st file, assign matrices and NOT delete elements!
    if (ii == 0) {
      accumSumWmapVec = sumWmapVec;
      accumSumW2mapVec = sumW2mapVec;
    }
    else {
      // Now accumSumWmapVec exists, just add maps.
      AddMaps( sumWmapVec, accumSumWmapVec);
      AddMaps( sumW2mapVec, accumSumW2mapVec);

      // Clear vectors and erase these maps
      std::vector< std::map<G4int,G4double>* >::iterator it;
      for (it = sumWmapVec.begin(); it != sumWmapVec.end(); it++) {
	//	    cout << "(*it)->size()=" << (*it)->size() << endl;
	delete (*it);
      }
      for (it = sumW2mapVec.begin(); it != sumW2mapVec.end(); it++) {
	delete (*it);
      }
    }

    sumWmapVec.clear();
    sumW2mapVec.clear();
    // cout << "sumWmapVec.size()=" << sumWmapVec.size() << endl;
    
  }  // Matches "for (int ii = 0; ii < nDir; ii++)"



  // =========================
  //  WRITING THE OUTPUT FILE
  // =========================

  G4cout << "Writing the output file with the complete hit map" << G4endl;
  std::ofstream theSumFile( outFileName.c_str(), std::ofstream::out );

  // -------------
  // Print header
  // -------------

  theSumFile << "# nEvt = " << accumEvt << G4endl;
  theSumFile << "# nMaps = " << nMaps << G4endl;
  theSumFile << "# ----------" << G4endl;

  // ----------------------------------
  // Loop to dump the total runHitMaps
  // ----------------------------------

  if (nMaps < accumSumWmapVec.size() || nMaps < accumSumW2mapVec.size() ) {
    // This case would be a weird error
    std::cerr << "ERROR: nMaps != vectors size!" << std::endl;
  }

  for (size_t ii = 0; ii < nMaps; ii++)  {
    // Run hit map identification header
    // ----------------------------------
    theSumFile << "# runHitMapID = " << runHitMapIdVec[ii] << G4endl;
    theSumFile << "# fullName =" << fullNameVec[ii] << G4endl;
    theSumFile << "# no. entries = " << accumSumWmapVec[ii]->size() << G4endl;
    //    theSumFile << headerLineVec[ii] << G4endl;
    theSumFile << "#    ID E_{dep}[MeV]     RelError " << headerLineVec[ii] 
	       << G4endl;

    // Run hit map data
    // -----------------
    std::map<G4int,G4double>::iterator mapit, mapit2;
    std::map<G4int,G4String>::iterator infoIt;
    mapit = accumSumWmapVec[ii]->begin();
    infoIt = infoMap2.operator[](runHitMapIdVec[ii])->begin();
    for (mapit2 = accumSumW2mapVec[ii]->begin();
	 mapit2 != accumSumW2mapVec[ii]->end(); mapit2++) {
      G4int key = mapit->first;
      G4double sumW = mapit->second;
      G4double sumW2 = mapit2->second;

      // Now getting energy deposition per event and relative error
      // (BRB Walters, I Kawrakow and DWO Rogers, Med.Phys. 29 (2002) 2745-2752)
      G4double energyPerEvent = sumW/accumEvt;
      G4double energyRelError 
	= sqrt((accumEvt*sumW2-sumW*sumW)/(accumEvt-1))/sumW;

      // Print it to the file with the suitable units
      theSumFile << std::scientific;
      theSumFile << std::setw(7)  << key
		 << std::setw(13) << std::setprecision(5) << energyPerEvent/MeV
		 << std::setw(13) << std::setprecision(5) << energyRelError
		 << infoIt->second
		 << G4endl;
      mapit++;
      infoIt++;
    }

    // Two blank lines between run hit maps (GNUplot index separator)
    // --------------------------------------------------------------
    theSumFile << G4endl << G4endl;
  }

  theSumFile.close();


  // ===================
  //  DELETING POINTERS
  // ===================

  std::vector< std::map<G4int,G4double>* >::iterator vecit;

  for (vecit = accumSumWmapVec.begin(); vecit != accumSumWmapVec.end();
       vecit++)
    {
      (*vecit)->clear();
      delete (*vecit);
    }

  for (vecit = accumSumW2mapVec.begin(); vecit != accumSumW2mapVec.end();
       vecit++)
    {
      (*vecit)->clear();
      delete (*vecit);
    }

  std::map<G4int, std::map<G4int,G4String>* >::iterator infoMap2It;
  for (infoMap2It = infoMap2.begin(); infoMap2It != infoMap2.end();
       infoMap2It++)
    {
      infoMap2It->second->clear();
      delete (infoMap2It->second);
    }

  accumSumWmapVec.clear();
  accumSumW2mapVec.clear();
  infoMap2.clear();

  // =================
  //  Job termination
  // =================

  G4cout << "Global hit maps (G4 internal units) written  in \""
	 << outFileName << "\" successfully." << G4endl;

}



//Method for adding each thread output file (runMaps_0.out_t*)
//------------------------------------------------------------------------------
void RunAction::AddMaps(std::vector< std::map<G4int, G4double>* >& newMaps,
			std::vector< std::map<G4int, G4double>* >& appendMaps)
{
  // iterators
  std::vector< std::map<G4int, G4double>* >::iterator vecit, vecit2;
  std::map<G4int, G4double>::iterator mapit, mapit2;
  
  vecit = newMaps.begin();
  for (vecit2 = appendMaps.begin(); vecit2 != appendMaps.end(); vecit2++)  {
    for ( mapit = (*vecit)->begin(); mapit != (*vecit)->end(); mapit++ )  {
      G4int key = mapit->first;
      mapit2 = (*vecit2)->find( key );

      if ( mapit2 != (*vecit2)->end() )
	(*(*vecit2))[key] += mapit->second;
      else
	(*vecit2)->insert( std::pair<G4int, G4double>(key, mapit->second) );
    }
    vecit++;
  }
}

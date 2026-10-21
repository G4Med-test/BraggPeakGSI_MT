//MACG 2012-07-25 : my template class
//                  It assumes that a 'Run' class derived from G4Run has been
//                  created in order to sum the scorers throughout the run.
//                  It also retrieves the positioning information of each PV
//                  copied from a LV tagged as SD, but reffered to the Mother
//                  Volume Local Frame and not to the Global Frame.
//MACG 2018-09-16: Added messenger class.
//                 RunID is NOT in file name if not using default ("runMaps").


#ifndef RunHitsGlobalPrinter_hh
#define RunHitsGlobalPrinter_hh 1

#include "globals.hh"
#include <fstream>
#include <vector>

class RunHitsGlobalPrinterMessenger;


class RunHitsGlobalPrinter
{
public:
  RunHitsGlobalPrinter();
  RunHitsGlobalPrinter(G4String fileName);
  ~RunHitsGlobalPrinter();

public:
#ifdef G4MULTITHREADED
  void DumpAll(G4int threadID);
#else
  void DumpAll();
#endif

private:
#ifdef G4MULTITHREADED
  void OpenFile(G4int threadID);
#else
  void OpenFile();
#endif
  void PrintHeader();
  void PrintMaps();
#ifdef G4MULTITHREADED
  void CloseFile(G4int threadID);
#else
  void CloseFile();
#endif

public:
  inline void SetFileName(G4String value)
  {
    theFileName = value;
    fIsDefaultFileName = false;
  }
  inline G4String GetFileName()    { return theFileName; }
  inline G4bool GetIsDefaultFileName()  { return fIsDefaultFileName; }

private:
  RunHitsGlobalPrinterMessenger* theMessenger;
  G4bool   fIsDefaultFileName;
  G4String theFileName;
  std::ofstream theOutputFile;

};

#endif

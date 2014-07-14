#ifndef __UAPDFSystPlot_H__
#define __UAPDFSystPlot_H__

#include <string>
#include <vector>
using namespace std;

class Plot_t {
  public:
  Plot_t(){;}
  virtual ~Plot_t(){;}
  Int_t nBins ;
  Float_t xMin ;
  Float_t xMax ;
  Float_t *x ;
  Int_t kLogY ;
  string Formula ;
  string NickName ;
  string XaxisTitle ;
  string YaxisTitle ;
};

#endif

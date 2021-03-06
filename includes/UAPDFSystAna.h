#ifndef __UAPDFSystAna_H__
#define __UAPDFSystAna_H__

#include <string>
#include <vector>
using namespace std;

#include "TreeFormula_t.h"
#include "UAPDFSystTH1F.h"
#include "UAPDFSystPlot.h"

class UAPDFSystAna {

  private:

    bool            bInit     ;
    bool            bConnect  ;

    // Analysis Setup
    string          NickName  ;
    TreeFormula_t   Selection ; 
    string          DataSet   ;

    // PDF Setup
    unsigned int nPDFsets;
    vector<unsigned int> nweights;
    vector<string> PDFName;
    vector <unsigned int> PDFStart ;

/*
    // Weights
    vector <double> weightedEvents ;
    vector <double> weighted2Events;
*/

    // Histograms
    vector <UAPDFSystTH1F*> SystTH1F;
	 vector <Plot_t> plots;
  public:
    
    // Contructor
    UAPDFSystAna()                                       { NickName = "NoSel"   ; Selection.NickName = "NoSel"   ; Selection.Expression = "1."       ; bInit = true ; bConnect = false ; plots = vector<Plot_t>() ; }
    UAPDFSystAna ( string sNickName , string sSelection , vector<Plot_t> plotsIn ){ NickName = sNickName ; Selection.NickName = sNickName ; Selection.Expression = sSelection ; bInit = true ; bConnect = false ; plots = plotsIn ; }

    // Destructor
    virtual ~UAPDFSystAna(){ Disconnect() ; Reset(); } 
    //virtual ~UAPDFSystAna(){;}

    // Initialisation
    void Connect( const string , const unsigned int , const vector<unsigned int> , const vector<string> , TTree* ) ;  
    void Disconnect();
    void Reset();

    // Filling and Summary
    void Fill(int,int,bool,double);
    void Print(); 
    void Plot(); 
    void Save(); 
    

};

#endif

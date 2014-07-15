#include "UAPDFSystConfig.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
using namespace std;

// ---------------------- TOOLS ------------------------------------

vector<string> UATokenize(string input , char token){
  stringstream ssinput(input);
  string Element;
  vector<string> Elements;
  while (getline(ssinput, Element , token )) {
   Elements.push_back(Element);
  }
  return Elements;
}

//  ---------------------- InitCfg() --------------------------------

void UAPDFSystConfig::InitCfg(){

  NPDFMaxLoad = 3   ;

  InDir  = "InDir/";
  OutDir = "OutDir/";

  id1.NickName   = "id1" ;
  id1.Expression = "1." ;
  
  id2.NickName   = "id2" ;
  id2.Expression = "1." ;

  x1.NickName    = "x1" ;
  x1.Expression  = "1." ;

  x2.NickName    = "x2" ;
  x2.Expression  = "1." ;

  Q.NickName     = "Q" ;
  Q.Expression   = "1." ;

  pdf1.NickName  = "pdf1" ;
  pdf1.Expression= "1." ;

  pdf2.NickName  = "pdf2" ;
  pdf2.Expression= "1." ;

  // PDF Sets
  PDFsets.clear();

  // InputData
  InputData.clear(); 

  //  variables
  Plots.clear();
	 
  // Lumi, Weights 
  BaseLumi   = 1. ;
  TargetLumi = 1. ;
  BaseWght.NickName    = "BaseWght" ;
  BaseWght.Expression  = "1." ;

  // Selections
  PreSel.NickName    = "PreSel";
  PreSel.Expression  = "1." ;
  BaseSel.NickName   = "BaseSel";
  BaseSel.Expression = "1." ;

  // PDFAna
  SystAna.clear();

}

// ---------------------- ReadCfg() --------------------------------

void UAPDFSystConfig::ReadCfg(string& ConFile){

  ifstream Cfg ;
  Cfg.open ( ConFile.c_str() );

  if(!Cfg) {
    cout << "Cannot open input file.\n";
    return ;
  }

  char str[5000];
  while( Cfg ) {

    Cfg.getline(str,5000);
    if(!Cfg) continue;
    istringstream iss(str);
    vector<string> Elements;
    do
    {
        string sub;
        iss >> sub;
        if(sub.size()>0) Elements.push_back(sub);
    } while (iss);
    if ( ! (Elements.size() > 0) ) continue;

    // Maximum # PDF sets that can be loaded at same time in LHPDF library
    // if ( Elements.at(0) == "NPDFMaxLoad" ) NPDFMaxLoad = atoi(Elements.at(1).c_str()) ;

    // In/Out Directory
    if ( Elements.at(0) == "InDir" ) InDir  = Elements.at(1);
    if ( Elements.at(0) == "OutDir") OutDir = Elements.at(1); 

    // Original PDF from Tree 
    if ( Elements.at(0) == "PDFId1"   ) id1.Expression = Elements.at(1);
    if ( Elements.at(0) == "PDFId2"   ) id2.Expression = Elements.at(1);
    if ( Elements.at(0) == "PDFx1"    ) x1.Expression  = Elements.at(1);
    if ( Elements.at(0) == "PDFx2"    ) x2.Expression  = Elements.at(1);
    if ( Elements.at(0) == "QScale"   ) Q.Expression   = Elements.at(1);
    if ( Elements.at(0) == "PDFx1PDF" ) pdf1.Expression= Elements.at(1);
    if ( Elements.at(0) == "PDFx1PDF" ) pdf2.Expression= Elements.at(1);

    // PDF Sets
    if ( Elements.at(0) == "PDFset"   ) {
      PDFset_t PDFset ;
      PDFset.NickName = Elements.at(1);
      PDFset.FileName = Elements.at(2);

      PDFsets.push_back(PDFset);
    }


    // Input Data
    if ( Elements.at(0) == "InputData") {
      InputData_t Data;
      Data.NickName = Elements.at(1) ;
      Data.FileName = Elements.at(2) ;
      Data.TreeName = Elements.at(3) ;
      Data.bFixScale         = false;
      Data.QScale.NickName   = "q-scale" ; 
      Data.QScale.Expression = "1." ; 
      Data.bFixPDF  = false ;
      Data.RefPDF   = "DUMMY";
      InputData.push_back(Data);
    } 

    // FixScale 
    if ( Elements.at(0) == "DataFixScale" ) {
      for ( vector<InputData_t>::iterator itData = InputData.begin() ; itData != InputData.end() ; ++itData ) {
        if ( itData->NickName == Elements.at(1) ) {
          itData->bFixScale = true ;
          itData->QScale.Expression = Elements.at(2) ;  
        }
      }
    }

    // DataRefPDF
    if ( Elements.at(0) == "DataRefPDF" ) {
       for ( vector<InputData_t>::iterator itData = InputData.begin() ; itData != InputData.end() ; ++itData ) {
        if ( itData->NickName == Elements.at(1) ) {
          itData->bFixPDF = true ;
          itData->RefPDF  = Elements.at(2) ; 
        }
      }
    }

    // Lumi, Weights 
    if ( Elements.at(0) == "BaseLumi"   ) BaseLumi   = atof(Elements.at(1).c_str()) ;
    if ( Elements.at(0) == "TargetLumi" ) TargetLumi = atof(Elements.at(1).c_str()) ;
    if ( Elements.at(0) == "BaseWght" )   BaseWght.Expression   = Elements.at(1) ;

    // PreSel
    if ( Elements.at(0) == "PreSel"  ) PreSel.Expression  += "&&" + Elements.at(1) ;   
    // BaseSel
    if ( Elements.at(0) == "BaseSel" ) BaseSel.Expression += "&&" + Elements.at(1) ;   
    
    // Plots
    if ( Elements.at(0) == "Plot" ) { 
       Plot_t p;  
       p.Formula    = Elements.at(1);
       p.NickName   = Elements.at(2);
       p.XaxisTitle = Elements.at(3);
       p.YaxisTitle = Elements.at(4);
       p.kLogY    = atoi(Elements.at(5).c_str());
       p.nBins      = atoi(Elements.at(6).c_str());
       string ref="-";
       if ( ! (ref.compare(Elements.at(7)) == 0) ) p.xMin       = atof(Elements.at(7).c_str());
       else                                      p.xMin       = -999;
       if ( ! (ref.compare(Elements.at(8)) == 0) ) p.xMax       = atof(Elements.at(8).c_str());
       else                                      p.xMax       = -999;
       if ( ! (ref.compare(Elements.at(9)) == 0) ) {
          char token = ',';
          vector<string> subElements = UATokenize(Elements.at(9),token);
          p.x = new Float_t[p.nBins];
          for (unsigned int iSE=0; iSE<subElements.size(); ++iSE) {
             p.x[iSE] = atof(subElements.at(iSE).c_str());
          }
       }
       else {
          p.x = new Float_t[1];
          p.x[0] = -999;
       }
       Plots.push_back(p);
    }

  }

  Cfg.close();
  Cfg.open ( ConFile.c_str() );
 

  // PDF Analysis last ! 
  while( Cfg ) {

    Cfg.getline(str,5000);
    if(!Cfg) continue;
    istringstream iss(str);
    vector<string> Elements;
    do
    {
        string sub;
        iss >> sub;
        if(sub.size()>0) Elements.push_back(sub);
    } while (iss);
    if ( ! (Elements.size() > 0) ) continue;


    // PDFAna
    if ( Elements.at(0) == "PDFAna" ) SystAna.push_back(UAPDFSystAna(Elements.at(1),Elements.at(2),Plots));

  }

}

// ---------------------- PrintCfg() --------------------------------

void UAPDFSystConfig::PrintCfg(){

  string red = "\033[1;31m";
  string plain = "\033[m";

  cout <<  "-------------------------------------------------" << endl ;  
  cout <<  "NPDFMaxLoad: " << NPDFMaxLoad << endl ;  
  cout <<  "-------------------------------------------------" << endl ;  
  cout << red <<  "--------------------------------------------------------------------------------------------------" << plain << endl ;  
  cout << red <<  "PDFId1   : " << plain << id1.Expression  << endl ;  
  cout << red <<  "PDFId2   : " << plain << id2.Expression  << endl ; 
  cout << red <<  "PDFx1    : " << plain << x1.Expression   << endl ; 
  cout << red <<  "PDFx2    : " << plain << x2.Expression   << endl ; 
  cout << red <<  "QScale   : " << plain << Q.Expression    << endl ; 
  cout << red <<  "PDFx1PDF : " << plain << pdf1.Expression << endl ; 
  cout << red <<  "PDFx1PDF : " << plain << pdf2.Expression << endl ; 

  cout << red <<  "-------------------------------------------------" << plain << endl ;  
  for ( vector<InputData_t>::iterator itData = InputData.begin() ; itData != InputData.end() ; ++itData ) {
    cout << red << "Data : " << plain << itData->NickName << endl ; 
    cout << red << "  --> FileName : " << plain << itData->FileName << endl ; 
    cout << red << "  --> TreeName : " << plain << itData->TreeName << endl ;
    cout << red << "  --> bFixScale: " << plain << itData->bFixScale << " --> " << itData->QScale.Expression << endl ;
    cout << red << "  --> bFixPDF  : " << plain << itData->bFixPDF   << " --> " << itData->RefPDF  << endl ;  
  }

  cout << red <<  "-------------------------------------------------" << plain << endl ;  
  cout << red <<  "BaseLumi  : " << plain << BaseLumi  << endl; 
  cout << red <<  "TargetLumi: " << plain << TargetLumi << endl; 
  cout << red <<  "BaseWght  : " << plain << BaseWght.Expression << endl ; 
  cout << red <<  "-------------------------------------------------" << plain << endl ;  
  cout << red <<  "Presection: " << plain << PreSel.Expression << endl ;
  cout << red <<  "Selection : " << plain << BaseSel.Expression  << endl ;
  cout << red <<  "-------------------------------------------------" << plain << endl ;  
  cout << red << "Variables : " << plain << endl ; 
  for (vector<Plot_t>::iterator iP = Plots.begin() ; iP != Plots.end() ; ++iP) {
     if (iP->xMin == -999 && iP->xMax == -999 && iP->x[0] == -999) cout << iP->NickName << " : Problem !!!" << endl;
     else if ( ! (iP->xMin == -999 || iP->xMax == -999) ) cout << "  -- " << iP->NickName << "( " << iP->Formula << " ) : " << iP->nBins << " : ( " << iP->xMin << " , " << iP->xMax << " ) " << endl;
     else {
        cout << "  -- " << iP->NickName << "( "<< iP->Formula << " ) : " << iP->nBins << " : { " ;
        for (int i=0; i<iP->nBins; ++i) cout << iP->x[i] << " , " ;
        cout << iP->x[iP->nBins] << " } " << endl;
     }
  }
  cout << red <<  "--------------------------------------------------------------------------------------------------" << plain << endl << endl << endl;


}

void UAPDFSystConfig::DummyConf(){}

//XJ   //InDir  = "/Users/xjanssen/cms/HWW2012/LatinoTrees2012/R53X_S1_V08_S2_V09_S3_V13/ReducedTrees_4L_WW_MoriondeffWPuWtriggW/";
//XJ   //InDir = "/Users/xjanssen/cms/HWW2012/LatinoTrees2012/R53X_S1_V08_S2_V09_S3_V13/MC_TightTight/4L_MoriondeffWPuWtriggW/";
//XJ   InDir  = "WWXsection/";
//XJ   OutDir = "testDir/";
//XJ 
//XJ   InputData_t Data;
//XJ   Data.NickName = "ggH125";
//XJ   Data.TreeName = "latino";
//XJ   Data.bFixScale         = true;
//XJ   Data.QScale.NickName   = "q-scale" ; 
//XJ   //Data.QScale.Expression = "MHiggs" ; 
//XJ   //Data.QScale.Expression = "sqrt(MHiggs*MHiggs+pdfscalePDF*pdfscalePDF)" ; 
//XJ   Data.QScale.Expression = "pdfscalePDF" ; 
//XJ   
//XJ   Data.bFixPDF  = true ;
//XJ //  Data.RefPDF   = "cteq66.LHgrid";
//XJ   Data.RefPDF   = "CT10nlo.LHgrid";
//XJ 
//XJ //  Data.FileName = "R53X_S1_V08_S2_V09_S3_v13_Pub2012_ggH125_WWsel.root";
//XJ //  InputData.push_back(Data);
//XJ 
//XJ //  Data.NickName = "ggH160";
//XJ //  Data.FileName = "R53X_S1_V08_S2_V09_S3_v13_Pub2012_ggH160_WWsel.root";
//XJ 
//XJ /*
//XJ   Data.NickName = "ggH125";  
//XJ   Data.FileName = "latino_1125_ggToH125toWWTo2LAndTau2Nu.root";
//XJ   InputData.push_back(Data);
//XJ 
//XJ   Data.NickName = "ggH160";  
//XJ   Data.FileName = "latino_1160_ggToH160toWWTo2LAndTau2Nu.root";
//XJ   InputData.push_back(Data);
//XJ 
//XJ   Data.NickName = "ggH350";  
//XJ   Data.FileName = "latino_1350_ggToH350toWWTo2LAndTau2Nu.root";
//XJ   InputData.push_back(Data);
//XJ 
//XJ   Data.NickName = "ggH500";  
//XJ   Data.FileName = "latino_1500_ggToH500toWWTo2LAndTau2Nu.root";
//XJ   InputData.push_back(Data);
//XJ 
//XJ   Data.NickName = "vbfH125";  
//XJ   Data.FileName = "latino_2125_vbfToH125toWWTo2LAndTau2Nu.root";
//XJ   InputData.push_back(Data);
//XJ 
//XJ   Data.NickName = "vbfH160";  
//XJ   Data.FileName = "latino_2160_vbfToH160toWWTo2LAndTau2Nu.root";
//XJ   InputData.push_back(Data);
//XJ 
//XJ   Data.NickName = "vbfH350";  
//XJ   Data.FileName = "latino_2350_vbfToH350toWWTo2LAndTau2Nu.root";
//XJ   InputData.push_back(Data);
//XJ 
//XJ   Data.NickName = "vbfH500";  
//XJ   Data.FileName = "latino_2500_vbfToH500toWWTo2LAndTau2Nu.root";
//XJ   InputData.push_back(Data);
//XJ */
//XJ 
//XJ   
//XJ   Data.NickName = "WWJets2LPowheg";  
//XJ   Data.FileName = "latino_006_WWJets2LPowheg.root" ;
//XJ   InputData.push_back(Data);
//XJ 
//XJ 
//XJ   PreSel.Expression = "ch1*ch2==-1&&trigger==1&&pt1>20&&pt2>10";
//XJ 
//XJ   BaseSel.NickName   = "BaseSel" ;
//XJ   BaseSel.Expression = "ch1*ch2==-1&&trigger==1&&pt1>20&&pt2>10&&nextra==0&&pfmet>20.&&mll>12&&(zveto==1||!sameflav)&&mpmet>20.&&(!sameflav||((njet!=0||dymva1>0.88)&&(njet!=1||dymva1>0.84)&&(njet==0||njet==1||(pfmet>45.0))))&&(njet==0||njet==1||(dphilljetjet<pi/180.*165.||!sameflav))&&bveto_mu==1&&(bveto_ip==1&&nbjettche==0)&&ptll>30."; 
//XJ 
//XJ   BaseSel.Expression = "ch1*ch2==-1&&trigger==1&&pt1>20&&pt2>20&&nextra==0&&pfmet>20.&&mll>12&&(zveto==1||!sameflav)&&mpmet>20.&&(!sameflav||((njet!=0||dymva1>0.88)&&(njet!=1||dymva1>0.84)&&(njet==0||njet==1||(pfmet>45.0))))&&(njet==0||njet==1||(dphilljetjet<pi/180.*165.||!sameflav))&&bveto_mu==1&&(bveto_ip==1&&nbjettche==0)&&ptll>30.&&(!sameflav||ptll>45)";
//XJ 
//XJ 
//XJ   BaseWght.NickName   = "BaseWght" ;
//XJ   BaseWght.Expression   = "puW*baseW*effW*triggW";
//XJ   //BaseWght.Expression = "puW*baseW*effW*triggW"; 
//XJ 
//XJ   BaseLumi       =  1000. ;
//XJ   TargetLumi     = 19468. ;
//XJ 
//XJ   id1.NickName   = "id1" ;
//XJ   id1.Expression = "pdfid1" ;
//XJ   
//XJ   id2.NickName   = "id2" ;
//XJ   id2.Expression = "pdfid2" ;
//XJ 
//XJ   x1.NickName    = "x1" ;
//XJ   x1.Expression  = "pdfx1" ;
//XJ 
//XJ   x2.NickName    = "x2" ;
//XJ   x2.Expression  = "pdfx2" ;
//XJ 
//XJ   Q.NickName     = "Q" ;
//XJ   Q.Expression   = "pdfscalePDF" ;
//XJ 
//XJ   pdf1.NickName  = "pdf1" ;
//XJ   pdf1.Expression= "pdfx1PDF" ;
//XJ 
//XJ   pdf2.NickName  = "pdf2" ;
//XJ   pdf2.Expression= "pdfx2PDF" ;
//XJ 
//XJ //  PDFsets.push_back("cteq66.LHgrid");
//XJ //  PDFsets.push_back("CT10nlo.LHgrid");
//XJ //  PDFsets.push_back("MSTW2008nlo68cl.LHgrid");
//XJ //  PDFsets.push_back("NNPDF23_nlo_as_0118.LHgrid");
//XJ 
//XJ   SystAna.push_back(UAPDFSystAna("WWsel","1."));
//XJ   SystAna.push_back(UAPDFSystAna("all_0jet","njet==0"));
//XJ   SystAna.push_back(UAPDFSystAna("all_1jet","njet==1"));
//XJ   SystAna.push_back(UAPDFSystAna("sf_0jet","njet==0&&sameflav"));
//XJ   SystAna.push_back(UAPDFSystAna("sf_1jet","njet==1&&sameflav"));
//XJ   SystAna.push_back(UAPDFSystAna("df_0jet","njet==0&&!sameflav"));
//XJ   SystAna.push_back(UAPDFSystAna("df_1jet","njet==1&&!sameflav"));
//XJ 
//XJ 
//XJ 
//XJ //  SystAna.push_back(UAPDFSystAna("all_2jet","njet==2"));
//XJ 
//XJ 
//XJ /*
//XJ   InDir  = "/Users/xjanssen/cms/HWW2012/UAPDFSyst/vbfHHbb/";
//XJ   OutDir = "testDir/";
//XJ 
//XJ   InputData_t Data;
//XJ   Data.NickName = "vbfHbb125";
//XJ   Data.TreeName = "Hbb/events";
//XJ   Data.bFixScale         = true;
//XJ   Data.QScale.NickName   = "q-scale" ; 
//XJ   //Data.QScale.Expression = "MHiggs" ; 
//XJ   Data.QScale.Expression = "Qscale" ; 
//XJ   //Data.QScale.Expression = "sqrt(125*125+Qscale*Qscale)" ; 
//XJ   Data.bFixPDF  = true ;
//XJ   Data.RefPDF   = "cteq66.LHgrid";
//XJ   Data.RefPDF   = "MSTW2008nlo90cl.LHgrid";
//XJ 
//XJ   Data.FileName = "flatTree_vbfPowheg_M125_CTEQ66_tmva.root";
//XJ   Data.FileName = "flatTree_vbfPowheg_M125_MSTW2008_tmva.root";
//XJ   InputData.push_back(Data);
//XJ 
//XJ   BaseSel.Expression = "(triggerResult[5]||triggerResult[7])&&(jetPt[0]>85&&jetPt[1]>70&&jetPt[2]>60&&jetPt[3]>40&&mqq>300&&abs(dEtaqq)>2.5 && puId[0]==1)&&(abs(dPhibb)<2.0)";
//XJ //  BaseSel.Expression = "(jetPt[0]>85&&jetPt[1]>70&&jetPt[2]>60&&jetPt[3]>40&&mqq>300&&abs(dEtaqq)>2.5 && puId[0]==1)&&(abs(dPhibb)<2.0)";
//XJ 
//XJ   BaseWght.NickName   = "BaseWght" ;
//XJ   BaseWght.Expression = "1."; 
//XJ 
//XJ   BaseLumi       = 1. ;
//XJ   TargetLumi     = 1. ;
//XJ 
//XJ   id1.NickName   = "id1" ;
//XJ   id1.Expression = "id1" ;
//XJ   
//XJ   id2.NickName   = "id2" ;
//XJ   id2.Expression = "id2" ;
//XJ 
//XJ   x1.NickName    = "x1" ;
//XJ   x1.Expression  = "x1" ;
//XJ 
//XJ   x2.NickName    = "x2" ;
//XJ   x2.Expression  = "x2" ;
//XJ 
//XJ   Q.NickName     = "Q" ;
//XJ   Q.Expression   = "Qscale" ;
//XJ 
//XJ   pdf1.NickName  = "pdf1" ;
//XJ   pdf1.Expression= "xpdf1" ;
//XJ 
//XJ   pdf2.NickName  = "pdf2" ;
//XJ   pdf2.Expression= "xpdf2" ;
//XJ 
//XJ   PDFsets.push_back("cteq66.LHgrid");
//XJ //  PDFsets.push_back("CT10nlo.LHgrid");
//XJ   PDFsets.push_back("MSTW2008nlo68cl.LHgrid");
//XJ   PDFsets.push_back("MSTW2008nlo90cl.LHgrid");
//XJ //  PDFsets.push_back("NNPDF23_nlo_as_0118.LHgrid");
//XJ 
//XJ   SystAna.push_back(UAPDFSystAna("Presel","1."));
//XJ   SystAna.push_back(UAPDFSystAna("CAT0","MLP < 0.54"));
//XJ   SystAna.push_back(UAPDFSystAna("CAT1","MLP >= 0.54 && MLP < 0.78"));
//XJ   SystAna.push_back(UAPDFSystAna("CAT2","MLP >= 0.78 && MLP < 0.90"));
//XJ   SystAna.push_back(UAPDFSystAna("CAT3","MLP >= 0.90 && MLP < 0.94"));
//XJ   SystAna.push_back(UAPDFSystAna("CAT4","MLP >= 0.94"));
//XJ */
//XJ /*
//XJ   cout << red <<  "-------------------------------------------------" << plain << endl ;  
//XJ   cout << red <<  "BaseLumi  : " << plain << BaseLumi  << endl; 
//XJ   cout << red <<  "TargetLumi: " << plain << TargetLumi << endl; 
//XJ   cout << red <<  "BaseWght  : " << plain << BaseWght.Expression << endl ; 
//XJ   cout << red <<  "-------------------------------------------------" << plain << endl ;  
//XJ   cout << red <<  "Presection: " << plain << PreSel.Expression << endl ;
//XJ   cout << red <<  "Selection : " << plain << BaseSel.Expression  << endl ;
//XJ   cout << red <<  "-------------------------------------------------" << plain << endl ;  
//XJ   cout << red << "Variables : " << plain << endl ; 
//XJ   for (vector<Plot_t>::iterator iP = Plots.begin() ; iP != Plots.end() ; ++iP) {
//XJ      if (iP->xMin == -999 && iP->xMax == -999 && iP->x[0] == -999) cout << iP->NickName << " : Problem !!!" << endl;
//XJ      else if ( ! (iP->xMin == -999 || iP->xMax == -999) ) cout << "  -- " << iP->NickName << "( " << iP->Formula << " ) : " << iP->nBins << " : ( " << iP->xMin << " , " << iP->xMax << " ) " << endl;
//XJ      else {
//XJ         cout << "  -- " << iP->NickName << "( "<< iP->Formula << " ) : " << iP->nBins << " : { " ;
//XJ         for (int i=0; i<iP->nBins; ++i) cout << iP->x[i] << " , " ;
//XJ         cout << iP->x[iP->nBins] << " } " << endl;
//XJ      }
//XJ   }
//XJ   cout << red <<  "--------------------------------------------------------------------------------------------------" << plain << endl << endl << endl;
//XJ }
//XJ */
//XJ //Sara//	void UAPDFSystConfig::DummyConf(){
//XJ //Sara//	
//XJ //Sara//	  //InDir  = "/Users/xjanssen/cms/HWW2012/LatinoTrees2012/R53X_S1_V08_S2_V09_S3_V13/ReducedTrees_4L_WW_MoriondeffWPuWtriggW/";
//XJ //Sara//	  //InDir = "/Users/xjanssen/cms/HWW2012/LatinoTrees2012/R53X_S1_V08_S2_V09_S3_V13/MC_TightTight/4L_MoriondeffWPuWtriggW/";
//XJ //Sara//	  InDir  = "WWXsection/";
//XJ //Sara//	  OutDir = "testDir/";
//XJ //Sara//	
//XJ //Sara//	  InputData_t Data;
//XJ //Sara//	  Data.NickName = "ggH125";
//XJ //Sara//	  Data.TreeName = "latino";
//XJ //Sara//	  Data.bFixScale         = true;
//XJ //Sara//	  Data.QScale.NickName   = "q-scale" ; 
//XJ //Sara//	  //Data.QScale.Expression = "MHiggs" ; 
//XJ //Sara//	  //Data.QScale.Expression = "sqrt(MHiggs*MHiggs+pdfscalePDF*pdfscalePDF)" ; 
//XJ //Sara//	  Data.QScale.Expression = "pdfscalePDF" ; 
//XJ //Sara//	  
//XJ //Sara//	  Data.bFixPDF  = true ;
//XJ //Sara//	//  Data.RefPDF   = "cteq66.LHgrid";
//XJ //Sara//	  Data.RefPDF   = "CT10nlo.LHgrid";
//XJ //Sara//	
//XJ //Sara//	//  Data.FileName = "R53X_S1_V08_S2_V09_S3_v13_Pub2012_ggH125_WWsel.root";
//XJ //Sara//	//  InputData.push_back(Data);
//XJ //Sara//	
//XJ //Sara//	//  Data.NickName = "ggH160";
//XJ //Sara//	//  Data.FileName = "R53X_S1_V08_S2_V09_S3_v13_Pub2012_ggH160_WWsel.root";
//XJ //Sara//	
//XJ //Sara//	/*
//XJ //Sara//	  Data.NickName = "ggH125";  
//XJ //Sara//	  Data.FileName = "latino_1125_ggToH125toWWTo2LAndTau2Nu.root";
//XJ //Sara//	  InputData.push_back(Data);
//XJ //Sara//	
//XJ //Sara//	  Data.NickName = "ggH160";  
//XJ //Sara//	  Data.FileName = "latino_1160_ggToH160toWWTo2LAndTau2Nu.root";
//XJ //Sara//	  InputData.push_back(Data);
//XJ //Sara//	
//XJ //Sara//	  Data.NickName = "ggH350";  
//XJ //Sara//	  Data.FileName = "latino_1350_ggToH350toWWTo2LAndTau2Nu.root";
//XJ //Sara//	  InputData.push_back(Data);
//XJ //Sara//	
//XJ //Sara//	  Data.NickName = "ggH500";  
//XJ //Sara//	  Data.FileName = "latino_1500_ggToH500toWWTo2LAndTau2Nu.root";
//XJ //Sara//	  InputData.push_back(Data);
//XJ //Sara//	
//XJ //Sara//	  Data.NickName = "vbfH125";  
//XJ //Sara//	  Data.FileName = "latino_2125_vbfToH125toWWTo2LAndTau2Nu.root";
//XJ //Sara//	  InputData.push_back(Data);
//XJ //Sara//	
//XJ //Sara//	  Data.NickName = "vbfH160";  
//XJ //Sara//	  Data.FileName = "latino_2160_vbfToH160toWWTo2LAndTau2Nu.root";
//XJ //Sara//	  InputData.push_back(Data);
//XJ //Sara//	
//XJ //Sara//	  Data.NickName = "vbfH350";  
//XJ //Sara//	  Data.FileName = "latino_2350_vbfToH350toWWTo2LAndTau2Nu.root";
//XJ //Sara//	  InputData.push_back(Data);
//XJ //Sara//	
//XJ //Sara//	  Data.NickName = "vbfH500";  
//XJ //Sara//	  Data.FileName = "latino_2500_vbfToH500toWWTo2LAndTau2Nu.root";
//XJ //Sara//	  InputData.push_back(Data);
//XJ //Sara//	*/
//XJ //Sara//	
//XJ //Sara//	  
//XJ //Sara//	  Data.NickName = "WWJets2LPowheg";  
//XJ //Sara//	  Data.FileName = "latino_006_WWJets2LPowheg.root" ;
//XJ //Sara//	  InputData.push_back(Data);
//XJ //Sara//	
//XJ //Sara//	
//XJ //Sara//	  PreSel.Expression = "ch1*ch2==-1&&trigger==1&&pt1>20&&pt2>10";
//XJ //Sara//	
//XJ //Sara//	  BaseSel.NickName   = "BaseSel" ;
//XJ //Sara//	  BaseSel.Expression = "ch1*ch2==-1&&trigger==1&&pt1>20&&pt2>10&&nextra==0&&pfmet>20.&&mll>12&&(zveto==1||!sameflav)&&mpmet>20.&&(!sameflav||((njet!=0||dymva1>0.88)&&(njet!=1||dymva1>0.84)&&(njet==0||njet==1||(pfmet>45.0))))&&(njet==0||njet==1||(dphilljetjet<pi/180.*165.||!sameflav))&&bveto_mu==1&&(bveto_ip==1&&nbjettche==0)&&ptll>30."; 
//XJ //Sara//	
//XJ //Sara//	  BaseSel.Expression = "ch1*ch2==-1&&trigger==1&&pt1>20&&pt2>20&&nextra==0&&pfmet>20.&&mll>12&&(zveto==1||!sameflav)&&mpmet>20.&&(!sameflav||((njet!=0||dymva1>0.88)&&(njet!=1||dymva1>0.84)&&(njet==0||njet==1||(pfmet>45.0))))&&(njet==0||njet==1||(dphilljetjet<pi/180.*165.||!sameflav))&&bveto_mu==1&&(bveto_ip==1&&nbjettche==0)&&ptll>30.&&(!sameflav||ptll>45)";
//XJ //Sara//	
//XJ //Sara//	
//XJ //Sara//	  BaseWght.NickName   = "BaseWght" ;
//XJ //Sara//	  BaseWght.Expression   = "puW*baseW*effW*triggW";
//XJ //Sara//	  //BaseWght.Expression = "puW*baseW*effW*triggW"; 
//XJ //Sara//	
//XJ //Sara//	  BaseLumi       =  1000. ;
//XJ //Sara//	  TargetLumi     = 19468. ;
//XJ //Sara//	
//XJ //Sara//	  id1.NickName   = "id1" ;
//XJ //Sara//	  id1.Expression = "pdfid1" ;
//XJ //Sara//	  
//XJ //Sara//	  id2.NickName   = "id2" ;
//XJ //Sara//	  id2.Expression = "pdfid2" ;
//XJ //Sara//	
//XJ //Sara//	  x1.NickName    = "x1" ;
//XJ //Sara//	  x1.Expression  = "pdfx1" ;
//XJ //Sara//	
//XJ //Sara//	  x2.NickName    = "x2" ;
//XJ //Sara//	  x2.Expression  = "pdfx2" ;
//XJ //Sara//	
//XJ //Sara//	  Q.NickName     = "Q" ;
//XJ //Sara//	  Q.Expression   = "pdfscalePDF" ;
//XJ //Sara//	
//XJ //Sara//	  pdf1.NickName  = "pdf1" ;
//XJ //Sara//	  pdf1.Expression= "pdfx1PDF" ;
//XJ //Sara//	
//XJ //Sara//	  pdf2.NickName  = "pdf2" ;
//XJ //Sara//	  pdf2.Expression= "pdfx2PDF" ;
//XJ //Sara//	
//XJ //Sara//	//  PDFsets.push_back("cteq66.LHgrid");
//XJ //Sara//	  PDFsets.push_back("CT10nlo.LHgrid");
//XJ //Sara//	  PDFsets.push_back("MSTW2008nlo68cl.LHgrid");
//XJ //Sara//	  PDFsets.push_back("NNPDF23_nlo_as_0118.LHgrid");
//XJ //Sara//	
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("WWsel","1."));
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("all_0jet","njet==0"));
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("all_1jet","njet==1"));
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("sf_0jet","njet==0&&sameflav"));
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("sf_1jet","njet==1&&sameflav"));
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("df_0jet","njet==0&&!sameflav"));
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("df_1jet","njet==1&&!sameflav"));
//XJ //Sara//	
//XJ //Sara//	
//XJ //Sara//	
//XJ //Sara//	//  SystAna.push_back(UAPDFSystAna("all_2jet","njet==2"));
//XJ //Sara//	
//XJ //Sara//	
//XJ //Sara//	/*
//XJ //Sara//	  InDir  = "/Users/xjanssen/cms/HWW2012/UAPDFSyst/vbfHHbb/";
//XJ //Sara//	  OutDir = "testDir/";
//XJ //Sara//	
//XJ //Sara//	  InputData_t Data;
//XJ //Sara//	  Data.NickName = "vbfHbb125";
//XJ //Sara//	  Data.TreeName = "Hbb/events";
//XJ //Sara//	  Data.bFixScale         = true;
//XJ //Sara//	  Data.QScale.NickName   = "q-scale" ; 
//XJ //Sara//	  //Data.QScale.Expression = "MHiggs" ; 
//XJ //Sara//	  Data.QScale.Expression = "Qscale" ; 
//XJ //Sara//	  //Data.QScale.Expression = "sqrt(125*125+Qscale*Qscale)" ; 
//XJ //Sara//	  Data.bFixPDF  = true ;
//XJ //Sara//	  Data.RefPDF   = "cteq66.LHgrid";
//XJ //Sara//	  Data.RefPDF   = "MSTW2008nlo90cl.LHgrid";
//XJ //Sara//	
//XJ //Sara//	  Data.FileName = "flatTree_vbfPowheg_M125_CTEQ66_tmva.root";
//XJ //Sara//	  Data.FileName = "flatTree_vbfPowheg_M125_MSTW2008_tmva.root";
//XJ //Sara//	  InputData.push_back(Data);
//XJ //Sara//	
//XJ //Sara//	  BaseSel.Expression = "(triggerResult[5]||triggerResult[7])&&(jetPt[0]>85&&jetPt[1]>70&&jetPt[2]>60&&jetPt[3]>40&&mqq>300&&abs(dEtaqq)>2.5 && puId[0]==1)&&(abs(dPhibb)<2.0)";
//XJ //Sara//	//  BaseSel.Expression = "(jetPt[0]>85&&jetPt[1]>70&&jetPt[2]>60&&jetPt[3]>40&&mqq>300&&abs(dEtaqq)>2.5 && puId[0]==1)&&(abs(dPhibb)<2.0)";
//XJ //Sara//	
//XJ //Sara//	  BaseWght.NickName   = "BaseWght" ;
//XJ //Sara//	  BaseWght.Expression = "1."; 
//XJ //Sara//	
//XJ //Sara//	  BaseLumi       = 1. ;
//XJ //Sara//	  TargetLumi     = 1. ;
//XJ //Sara//	
//XJ //Sara//	  id1.NickName   = "id1" ;
//XJ //Sara//	  id1.Expression = "id1" ;
//XJ //Sara//	  
//XJ //Sara//	  id2.NickName   = "id2" ;
//XJ //Sara//	  id2.Expression = "id2" ;
//XJ //Sara//	
//XJ //Sara//	  x1.NickName    = "x1" ;
//XJ //Sara//	  x1.Expression  = "x1" ;
//XJ //Sara//	
//XJ //Sara//	  x2.NickName    = "x2" ;
//XJ //Sara//	  x2.Expression  = "x2" ;
//XJ //Sara//	
//XJ //Sara//	  Q.NickName     = "Q" ;
//XJ //Sara//	  Q.Expression   = "Qscale" ;
//XJ //Sara//	
//XJ //Sara//	  pdf1.NickName  = "pdf1" ;
//XJ //Sara//	  pdf1.Expression= "xpdf1" ;
//XJ //Sara//	
//XJ //Sara//	  pdf2.NickName  = "pdf2" ;
//XJ //Sara//	  pdf2.Expression= "xpdf2" ;
//XJ //Sara//	
//XJ //Sara//	  PDFsets.push_back("cteq66.LHgrid");
//XJ //Sara//	//  PDFsets.push_back("CT10nlo.LHgrid");
//XJ //Sara//	  PDFsets.push_back("MSTW2008nlo68cl.LHgrid");
//XJ //Sara//	  PDFsets.push_back("MSTW2008nlo90cl.LHgrid");
//XJ //Sara//	//  PDFsets.push_back("NNPDF23_nlo_as_0118.LHgrid");
//XJ //Sara//	
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("Presel","1."));
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("CAT0","MLP < 0.54"));
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("CAT1","MLP >= 0.54 && MLP < 0.78"));
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("CAT2","MLP >= 0.78 && MLP < 0.90"));
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("CAT3","MLP >= 0.90 && MLP < 0.94"));
//XJ //Sara//	  SystAna.push_back(UAPDFSystAna("CAT4","MLP >= 0.94"));
//XJ //Sara//	*/
//XJ //Sara//	
//XJ //Sara//	}
//XJ //Sara//	
//XJ //Sara//	

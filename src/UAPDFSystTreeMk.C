
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iterator>
#include <math.h>
using namespace std;

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>

#include "UAPDFSystConfig.h"
#include "UAPDFSystAna.h"

void UAPDFSystTreeMk(UAPDFSystConfig& Cfg){

  TFile* File ;
  TTree* Tree ;
  TTree* Tout ;
  TFile* FTout ; 


  for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD ) {
    
    // ------------ Output Histograms ----------------

    //TH1F* hQ   = new TH1F("hQ"  ,"hQ"  ,600, 0.,600.); 
    //TH1F* hx1  = new TH1F("hx1" ,"hx1" , 40, 0.,0.8 ); 
    //TH1F* hx2  = new TH1F("hx2" ,"hx2" , 40, 0.,0.8 ); 
    //TH2F* hx12 = new TH2F("hx12","hx12", 40, 0.,0.8 , 40, 0.,0.8 );
    //TH2F* hid  = new TH2F("hid" ,"hid" ,13 ,-7,7,13,-7,7);

    // ------------ Init PDF Sets --------------------

    unsigned int nPDFsets = 0;
    unsigned int nPDFOpen = 0; 
    vector<unsigned int> nweights;
    vector<unsigned int> nweightsLocal;
    nweightsLocal.resize((Cfg.GetPDFsets())->size(),0);
    vector<string> PDFName;
    vector<Float_t >* weight_pdf1 = new vector<Float_t>;  
    vector<Float_t >* weight_pdf2 = new vector<Float_t>;  
    vector<Float_t >* weight_pdf3 = new vector<Float_t>;  
    vector<Float_t >* alphas_pdf1 = new vector<Float_t>;  
    vector<Float_t >* alphas_pdf2 = new vector<Float_t>;  
    vector<Float_t >* alphas_pdf3 = new vector<Float_t>; 

    vector<vector<Float_t>* > weight_pdf ;

    if (itD->bFixPDF) {
       ++nPDFsets;
       ++nPDFOpen; 
       PDFName.push_back(itD->RefPDF);
       LHAPDF::initPDFSet(nPDFOpen,itD->RefPDF);  
       nweights.push_back(1);  
       nweights.at(nPDFsets-1) += LHAPDF::numberPDF(nPDFOpen);
       nweightsLocal.at(nPDFOpen-1) = nweights.at(nPDFsets-1) ;
       weight_pdf1->clear();
       weight_pdf1->resize(nweights.at(0),0.);
       alphas_pdf1->clear();
       alphas_pdf1->resize(nweights.at(0),0.);

       weight_pdf.push_back( new vector<Float_t> ) ; 
       (weight_pdf.at(nPDFsets-1))->clear();
       (weight_pdf.at(nPDFsets-1))->resize(nweights.at(0),0.);

    }

    // ------------- Loop on PDF Blocks (according to NPDFMaxLoad) ------

    unsigned int iPDFBlock    = 0;
    unsigned int nPDFTotal    = (Cfg.GetPDFsets())->size();
    unsigned int NPDFMaxLoad  = Cfg.GetNPDFMaxLoad() ;
    if (itD->bFixPDF) --NPDFMaxLoad ;  
    while ( nPDFsets < nPDFTotal ) {
      //cout << nPDFsets << "-->" << nPDFsets + NPDFMaxLoad << endl ;
      ++iPDFBlock ;
      int iFirstPDF = nPDFsets ;
      int iLastPDF  = min ( nPDFsets + NPDFMaxLoad , nPDFTotal ) ;

      // ------------ Open Tree & Init New One 

      stringstream ss,ssm1;
      ss   << iPDFBlock ;
      ssm1 << iPDFBlock-1;
      if    (  iPDFBlock == 1 ) {
        cout << "Opening TTree: " << itD->NickName << " " << itD->FileName << " " << itD->TreeName << endl;
        File = new TFile((Cfg.GetInDir()+"/"+itD->FileName).c_str(),"READ");
        Tree = (TTree*) File->Get((itD->TreeName).c_str());
        Tree->SetBranchStatus("*",1);
      }    
      else {
        string TIFileName = Cfg.GetOutDir() + "/PDFSyst_Tree_" + itD->FileName + "_Pass" + ssm1.str();
        File = new TFile(TIFileName.c_str(),"READ");                 
        Tree = (TTree*) File->Get("UAPDFSyst");
        Tree->SetBranchStatus("*",1);
      } 

      string TOFileName = Cfg.GetOutDir() + "/PDFSyst_Tree_" + itD->FileName + "_Pass" + ss.str() ;
      FTout = new TFile( TOFileName.c_str(),"RECREATE" ); 
      FTout->cd();
      Tout  = Tree->CloneTree(0);
      Tout->SetName("UAPDFSyst");
      

      if ( iPDFBlock == 1 && itD->bFixPDF ) {
        //Tout->Branch(TString("UAPDFSyst_weightv_")+TString(((Cfg.GetPDFsets())->at(0)).NickName.c_str()), &(weight_pdf.at(nPDFsets-1)));
        Tout->Branch(TString("UAPDFSyst_weight_")+TString(((Cfg.GetPDFsets())->at(0)).NickName.c_str()), &weight_pdf1);
        Tout->Branch(TString("UAPDFSyst_alphas_")+TString(((Cfg.GetPDFsets())->at(0)).NickName.c_str()), &alphas_pdf1);
      }

      // ------------ Init Formulas  -------------------

      (Cfg.GetPreSel())->MakFormula(Tree);
      (Cfg.GetBaseSel())->MakFormula(Tree);
      (Cfg.GetBaseWght())->MakFormula(Tree);
      (Cfg.Getid1())->MakFormula(Tree);
      (Cfg.Getid2())->MakFormula(Tree);
      (Cfg.Getx1())->MakFormula(Tree);
      (Cfg.Getx2())->MakFormula(Tree);
      (Cfg.GetQ())->MakFormula(Tree);
      (Cfg.Getpdf1())->MakFormula(Tree);
      (Cfg.Getpdf2())->MakFormula(Tree);
      if ( itD->bFixScale ) (itD->QScale).MakFormula(Tree);

      // --------- INIT PDF SETS ---------
      for ( int iPDFSet = iFirstPDF ; iPDFSet < iLastPDF ; ++iPDFSet ) {
        //cout << nPDFsets << endl ;
        ++nPDFsets;      
        ++nPDFOpen;
        cout << "--> Loading : " << ((Cfg.GetPDFsets())->at(iPDFSet)).NickName << " at position : " << nPDFOpen << endl;
        PDFName.push_back( ((Cfg.GetPDFsets())->at(iPDFSet)).FileName ) ;
        if ( nPDFOpen > Cfg.GetNPDFMaxLoad() ) 
        { 
          cout << "[WARNING] More then " << Cfg.GetNPDFMaxLoad() << "  PDF Sets: " << ((Cfg.GetPDFsets())->at(iPDFSet)).NickName << " will be skiped !" << endl ;
        } 
        else {                          
          LHAPDF::initPDFSet(nPDFOpen, ((Cfg.GetPDFsets())->at(iPDFSet)).FileName ) ;
          nweights.push_back(1);  
          cout << LHAPDF::numberPDF(nPDFOpen) << endl; 
          nweights.at(nPDFsets-1) += LHAPDF::numberPDF(nPDFOpen);
          nweightsLocal.at(nPDFOpen-1) = nweights.at(nPDFsets-1) ;

          weight_pdf.push_back( new vector<Float_t> ) ; 
          (weight_pdf.at(nPDFsets-1))->clear();
          (weight_pdf.at(nPDFsets-1))->resize(nweights.at(nPDFsets-1),0.);
          //Tout->Branch(TString("UAPDFSyst_weightv_")+TString(((Cfg.GetPDFsets())->at(iPDFSet)).NickName.c_str()), &(weight_pdf.at(nPDFsets-1)));
 

          if      (nPDFOpen == 1) {
            weight_pdf1->clear();
            weight_pdf1->resize(nweights.at(nPDFsets-1),0.);
            Tout->Branch(TString("UAPDFSyst_weight_")+TString(((Cfg.GetPDFsets())->at(iPDFSet)).NickName.c_str()), &weight_pdf1);
            alphas_pdf1->clear();
            alphas_pdf1->resize(nweights.at(nPDFsets-1),0.);
            Tout->Branch(TString("UAPDFSyst_alphas_")+TString(((Cfg.GetPDFsets())->at(iPDFSet)).NickName.c_str()), &alphas_pdf1);
          }
          else if  (nPDFOpen == 2) {
            weight_pdf2->clear();
            weight_pdf2->resize(nweights.at(nPDFsets-1),0.);
            Tout->Branch(TString("UAPDFSyst_weight_")+TString(((Cfg.GetPDFsets())->at(iPDFSet)).NickName.c_str()), &weight_pdf2);
            alphas_pdf2->clear();
            alphas_pdf2->resize(nweights.at(nPDFsets-1),0.);
            Tout->Branch(TString("UAPDFSyst_alphas_")+TString(((Cfg.GetPDFsets())->at(iPDFSet)).NickName.c_str()), &alphas_pdf2); 
          }
          else if  (nPDFOpen == 3) {
            weight_pdf3->clear();
            weight_pdf3->resize(nweights.at(nPDFsets-1),0.);
            Tout->Branch(TString("UAPDFSyst_weight_")+TString(((Cfg.GetPDFsets())->at(iPDFSet)).NickName.c_str()), &weight_pdf3);
            alphas_pdf3->clear();
            alphas_pdf3->resize(nweights.at(nPDFsets-1),0.);
            Tout->Branch(TString("UAPDFSyst_alphas_")+TString(((Cfg.GetPDFsets())->at(iPDFSet)).NickName.c_str()), &alphas_pdf3); 
          }
        }
      }

      cout << "SIZE" << weight_pdf3->size() << endl ;


      // ------------ Event Loop -----------------------     

      Int_t nEntriesAll = Tree->GetEntries();   
      Int_t nEntries    = nEntriesAll;
      if (Cfg.GetNMax() > -1 ) nEntries = min(Cfg.GetNMax(),nEntries) ;
      cout << "[INFO] nEntriesAll= " << nEntriesAll << " nEntriesMax= " << nEntries << endl ; 

      ProgressBar Bar1(cout,nEntries);
      Bar1.SetStyle(2);
  
      for (Int_t jEntry = 0 ;  jEntry < nEntries ; ++jEntry) {
 
        Tree->GetEntry(jEntry);
        if( jEntry% 1000 == 0)
        {
           Bar1.Update(jEntry + 1);
           Bar1.Print();
        }
  
        // ------------ Eval Formulas  -------------------
        (Cfg.Getid1())->EvaFormula();
        (Cfg.Getid2())->EvaFormula();
        (Cfg.Getx1())->EvaFormula();
        (Cfg.Getx2())->EvaFormula();
        (Cfg.GetQ())->EvaFormula();
        (Cfg.Getpdf1())->EvaFormula();
        (Cfg.Getpdf2())->EvaFormula();
        if ( itD->bFixScale ) (itD->QScale).EvaFormula();
  
        int    id1  = (Cfg.Getid1())->Result() ; 
        int    id2  = (Cfg.Getid2())->Result() ; 
        double x1   = (Cfg.Getx1())->Result()  ; 
        double x2   = (Cfg.Getx2())->Result()  ; 
        double Q    = (Cfg.GetQ())->Result()   ; 
        double pdf1 = (Cfg.Getpdf1())->Result();
        double pdf2 = (Cfg.Getpdf2())->Result();
  
        // ----------- Correct Q ------------------------
  
        if ( itD->bFixScale ) Q = (itD->QScale).Result();

        // ----------- Fix PDF --------------------------
  
        if ( itD->bFixPDF ) {
          LHAPDF::usePDFMember(1,0);
          pdf1 = LHAPDF::xfx(1, x1, Q, id1)/x1;
          pdf2 = LHAPDF::xfx(1, x2, Q, id2)/x2;
        }      

        // ----------- PDF Weights ----------------------
  
        for (  unsigned int iPDF=1 ; iPDF <= nPDFOpen ; ++iPDF ) {
          for (unsigned int i=0; i<nweightsLocal.at(iPDF-1); ++i) {
        
            LHAPDF::usePDFMember(iPDF,i);
            double newpdf1 = LHAPDF::xfx(iPDF, x1, Q, id1)/x1;
            double newpdf2 = LHAPDF::xfx(iPDF, x2, Q, id2)/x2;
            double pdfwght = newpdf1/pdf1*newpdf2/pdf2 ;
            if ( iPDF == 1 ) weight_pdf1->at(i) = pdfwght ;
            if ( iPDF == 2 ) weight_pdf2->at(i) = pdfwght ;
            if ( iPDF == 3 ) weight_pdf3->at(i) = pdfwght ;
            if ( iPDF == 1 ) alphas_pdf1->at(i) = LHAPDF::alphasPDF (iPDF, Q);
            if ( iPDF == 2 ) alphas_pdf2->at(i) = LHAPDF::alphasPDF (iPDF, Q);
            if ( iPDF == 3 ) alphas_pdf3->at(i) = LHAPDF::alphasPDF (iPDF, Q);

          }
        }

        // ------------ Write Event ---------------------
        Tout->Fill();


      }

      Bar1.Update(nEntries);
      Bar1.Print();
      Bar1.PrintLine();
      cout << endl;

      // ---------- CLOSE PDF SETS --------
      for ( int iPDFSet = iFirstPDF ; iPDFSet < iLastPDF ; ++iPDFSet ) {
        cout << "--> Closing : " << ((Cfg.GetPDFsets())->at(iPDFSet)).NickName << endl;
        --nPDFOpen;
      }


      // ------------ Delete Formulas  -----------------
  
      (Cfg.GetPreSel())->DelFormula();
      (Cfg.GetBaseSel())->DelFormula();
      (Cfg.GetBaseWght())->DelFormula();
      (Cfg.Getid1())->DelFormula();
      (Cfg.Getid2())->DelFormula();
      (Cfg.Getx1())->DelFormula();
      (Cfg.Getx2())->DelFormula();
      (Cfg.GetQ())->DelFormula();
      (Cfg.Getpdf1())->DelFormula();
      (Cfg.Getpdf2())->DelFormula();
      if ( itD->bFixScale ) (itD->QScale).DelFormula();   

      // ------------ Close Tree ------------------------

      delete Tree ;
      File->Close();
      delete File;
    
      // ------------ Save Output Tree ------------------

      FTout->cd();
      Tout->AutoSave();
      delete Tout;
      FTout->Close();
      delete FTout;

 
    }

    // ----------- Clean Up different Pass --------------
   
    stringstream ss;
    ss << iPDFBlock ;
    string PAFileName = Cfg.GetOutDir() + "/PDFSyst_Tree_" + itD->FileName + "_Pass" + ss.str() ;
    string TOFileName = Cfg.GetOutDir() + "/PDFSyst_Tree_" + itD->FileName ;
    string command = "mv "+PAFileName+" "+TOFileName ;
    system(command.c_str()); 
    command = "rm " + Cfg.GetOutDir() + "/PDFSyst_Tree_" + itD->FileName + "_Pass*";
    system(command.c_str()); 


  }
}

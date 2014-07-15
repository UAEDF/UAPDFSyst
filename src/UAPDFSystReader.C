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

void UAPDFSystReader(UAPDFSystConfig& Cfg){

  TFile* File ;
  TTree* Tree ;
  TFile* FHout ; 


  // ------- Loop on InputData -----------

  for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD ) {

    // ------------ Open Tree

    string TIFileName = Cfg.GetOutDir() + "/PDFSyst_Tree_" + itD->FileName ;
    File = new TFile(TIFileName.c_str(),"READ");                 
    Tree = (TTree*) File->Get("UAPDFSyst");
    Tree->SetBranchStatus("*",1);

    // ----------- Connect PDF weights -------------------

    unsigned int nPDFsets = (Cfg.GetPDFsets())->size() ;


    vector<vector<Float_t>* > weight_pdf ;
    vector<vector<Float_t>* > alphas_pdf ;
    weight_pdf.resize(nPDFsets) ;
    alphas_pdf.resize(nPDFsets) ; 
    for ( int iPDFSet = 0 ; iPDFSet < nPDFsets ; ++iPDFSet ) {
      weight_pdf[iPDFSet] = 0 ; 
      alphas_pdf[iPDFSet] = 0 ; 
      Tree->SetBranchAddress(TString("UAPDFSyst_weight_")+TString(((Cfg.GetPDFsets())->at(iPDFSet)).NickName.c_str()),&(weight_pdf[iPDFSet]));
      Tree->SetBranchAddress(TString("UAPDFSyst_alphas_")+TString(((Cfg.GetPDFsets())->at(iPDFSet)).NickName.c_str()),&(alphas_pdf[iPDFSet]));
    }

    vector<string> PDFName;
    PDFName.resize(nPDFsets,"Undef.");
    vector<unsigned int> nweights;
    nweights.resize(nPDFsets,0);
    Tree->GetEntry(0);
    for ( int iPDFSet = 0 ; iPDFSet < nPDFsets ; ++iPDFSet ) { 
      PDFName.at(iPDFSet)  = ((Cfg.GetPDFsets())->at(iPDFSet)).NickName ; 
      nweights.at(iPDFSet) = 5 ;
      nweights.at(iPDFSet) = (weight_pdf[iPDFSet])->size() ; // 5 ; // test_pdf->size() ;
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
    if ( itD->bFixScale ) (itD->QScale).MakFormula(Tree);

    // ------------ Output Histograms ----------------

    string FOFileName = Cfg.GetOutDir() + "/PDFSyst_Hist_" + itD->FileName ;
    FHout = new TFile( FOFileName.c_str(),"RECREATE" );  

    TH1F* hQ   = new TH1F("hQ"  ,"hQ"  ,600, 0.,600.); 
    TH1F* hx1  = new TH1F("hx1" ,"hx1" , 40, 0.,0.8 ); 
    TH1F* hx2  = new TH1F("hx2" ,"hx2" , 40, 0.,0.8 ); 
    TH2F* hx12 = new TH2F("hx12","hx12", 40, 0.,0.8 , 40, 0.,0.8 );
    TH2F* hid  = new TH2F("hid" ,"hid" ,13 ,-7,7,13,-7,7);

    // ------------- Connect Analysis ----------------

    for ( vector<UAPDFSystAna>::iterator itSA = (Cfg.GetSystAna())->begin() ; itSA != (Cfg.GetSystAna())->end() ; ++itSA) itSA->Connect(itD->NickName,nPDFsets,nweights,PDFName,Tree);

    // ------------ Event Loop -----------------------     

    Int_t nEntriesAll = Tree->GetEntries();   
    Int_t nEntries    = nEntriesAll;
    if (Cfg.GetNMax() > -1 ) nEntries = min(Cfg.GetNMax(),nEntries) ;
    double WghtNMax = double(nEntriesAll)/double(nEntries);
    cout << "[INFO] nEntriesAll= " << nEntriesAll << " nEntriesMax= " << nEntries << " --> WghtNMax= " << WghtNMax << endl;
    

    ProgressBar Bar1(cout,nEntries);
    Bar1.SetStyle(2);
  
    for (Int_t jEntry = 0 ;  jEntry < nEntries ; ++jEntry) {

      Tree->GetEntry(jEntry);
/*
      for ( int iPDFSet = 0 ; iPDFSet < nPDFsets ; ++iPDFSet ) { 
        cout << weight_pdf[iPDFSet]->size() << " " <<  weight_pdf[iPDFSet]->at(0) << endl;
      }
*/

      if( jEntry% 1000 == 0)
      {
         Bar1.Update(jEntry + 1);
         Bar1.Print();
      }

      double Wght = WghtNMax;

      // ------------ Eval Formulas (Sel&Wght)  --------
      (Cfg.GetPreSel())->EvaFormula();
      bool bPreSel = ( (Cfg.GetPreSel())->Result() > 0 );
      if (!bPreSel) continue;
      (Cfg.GetBaseSel())->EvaFormula();
      bool bBaseSel = ( (Cfg.GetBaseSel())->Result() > 0 );
      (Cfg.GetBaseWght())->EvaFormula();
      Wght *= (Cfg.GetBaseWght())->Result();
      Wght *= (Cfg.GetTargetLumi())/(Cfg.GetBaseLumi()); 
      if ( Wght <= 0 ) continue ;

      // ------------ Eval Formulas  -------------------
      (Cfg.Getid1())->EvaFormula();
      (Cfg.Getid2())->EvaFormula();
      (Cfg.Getx1())->EvaFormula();
      (Cfg.Getx2())->EvaFormula();
      (Cfg.GetQ())->EvaFormula();
      if ( itD->bFixScale ) (itD->QScale).EvaFormula();
  
      int    id1  = (Cfg.Getid1())->Result() ; 
      int    id2  = (Cfg.Getid2())->Result() ; 
      double x1   = (Cfg.Getx1())->Result()  ; 
      double x2   = (Cfg.Getx2())->Result()  ; 
      double Q    = (Cfg.GetQ())->Result()   ; 
 
      // ----------- Correct Q ------------------------
 
      if ( itD->bFixScale ) Q = (itD->QScale).Result();

      // ----------- Fill Some Histos -----------------

      hQ->Fill(Q); 
      hid->Fill(id1,id2);
      hx1->Fill(x1);   
      hx2->Fill(x2);   
      hx12->Fill(x1,x2);   
 
      // ----------- Run PDF Analysis -------------


      for (  unsigned int iPDF=0 ; iPDF < nPDFsets ; ++iPDF ) {
        for (unsigned int i=0; i<nweights.at(iPDF); ++i) {

           double pdfwght = weight_pdf[iPDF]->at(i) ;
           for ( vector<UAPDFSystAna>::iterator itSA = (Cfg.GetSystAna())->begin() ; itSA != (Cfg.GetSystAna())->end() ; ++itSA) itSA->Fill(iPDF+1,i,bBaseSel,Wght*pdfwght);

        }
      }



    // End: Event Loop ----------------------------

    }

    Bar1.Update(nEntries);
    Bar1.Print();
    Bar1.PrintLine();
    cout << endl;

    // ------------ Disconnect Analysis --------------

     
    for ( vector<UAPDFSystAna>::iterator itSA = (Cfg.GetSystAna())->begin() ; itSA != (Cfg.GetSystAna())->end() ; ++itSA) itSA->Disconnect();
 

    // ------------ Delete Formulas  -----------------
  
    (Cfg.GetPreSel())->DelFormula();
    (Cfg.GetBaseSel())->DelFormula();
    (Cfg.GetBaseWght())->DelFormula();
    (Cfg.Getid1())->DelFormula();
    (Cfg.Getid2())->DelFormula();
    (Cfg.Getx1())->DelFormula();
    (Cfg.Getx2())->DelFormula();
    (Cfg.GetQ())->DelFormula();
    if ( itD->bFixScale ) (itD->QScale).DelFormula();   

    // ------------ Close Tree
          
    delete Tree ;
    File->Close();
    delete File;

    // ------------ Summary & Plot --------------------
 
    for ( vector<UAPDFSystAna>::iterator itSA = (Cfg.GetSystAna())->begin() ; itSA != (Cfg.GetSystAna())->end() ; ++itSA) itSA->Print();
    for ( vector<UAPDFSystAna>::iterator itSA = (Cfg.GetSystAna())->begin() ; itSA != (Cfg.GetSystAna())->end() ; ++itSA) itSA->Plot();

    // ------------ Save Histograms -------------------

    FHout->cd();
    for ( vector<UAPDFSystAna>::iterator itSA = (Cfg.GetSystAna())->begin() ; itSA != (Cfg.GetSystAna())->end() ; ++itSA) itSA->Save();
    hQ->Write();
    hid->Write();
    hx1->Write();
    hx2->Write();
    hx12->Write();    
    FHout->Close(); 
    delete FHout;

    // ------------ Reset Analysis --------------------

    for ( vector<UAPDFSystAna>::iterator itSA = (Cfg.GetSystAna())->begin() ; itSA != (Cfg.GetSystAna())->end() ; ++itSA) itSA->Reset();

  } // End: Loop on InputData ------------

}

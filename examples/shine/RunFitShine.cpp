//
// Created by eugene on 26/11/2019.
//

#include <TFile.h>
#include <TH2D.h>

#include <core/BaseFitterHelper.h>
#include <examples/shine/ShineDeDxParticleFitModel.h>
#include <RooWorkspace.h>
#include <RooDataHist.h>
#include <TCanvas.h>
#include <RooPlot.h>

#include <TMath.h>
#include <TPaveText.h>
#include <core/FitParameter.h>

FitParameter::ConstraintFct_t wrapToX(FitParameter::ConstraintFct_t f, int charge) {
    return [=] (double x) { return f(TMath::Exp(charge*x)/20); };
}

int main(int argc, char ** argv) {

    auto inputFile = TFile::Open(argv[1]);

    TH2D *inputHistogram = nullptr;
    inputFile->GetObject("reco_info/hTrackdEdx_allTPC", inputHistogram);
    inputHistogram->SetDirectory(0);
    inputHistogram->Print();

    BaseFitterHelper fitterHelper;
    fitterHelper.initialize();

    fitterHelper.getObservable()->setRange(0., 4.);

    {
        auto protons = new ShineDeDxParticleFitModel(2212);
        protons->fillParticleInfoFromDB();
        protons->setRange(2.2, 6.4);
        protons->setRooVarPrefix("p_");
        fitterHelper.addParticleModel(protons);

        protons->parameter("bb").fix(wrapToX(BetheBlochHelper::makeBBForPdg(2212), 1));
        protons->parameter("sigma").range(0.05, .11);


        protons->print();
    }

    {
        auto pion_pos = new ShineDeDxParticleFitModel(211);
        pion_pos->fillParticleInfoFromDB();
        pion_pos->setRange(0.4, 5.5);
        pion_pos->setRooVarPrefix("pion_pos_");
        fitterHelper.addParticleModel(pion_pos);

        pion_pos->parameter("bb").fix(wrapToX(BetheBlochHelper::makeBBForPdg(211), 1));
//        pion_pos->parameter("sigma").range(0.05, .13);
        pion_pos->parameter("sigma").fix("1.25402e-01 - 1.03784e-02*x");

        pion_pos->print();
    }

    {
        auto pion_neg = new ShineDeDxParticleFitModel(-211);
        pion_neg->fillParticleInfoFromDB();
        pion_neg->setRange(-5.5, -0.4);
        pion_neg->setRooVarPrefix("pion_neg_");
        fitterHelper.addParticleModel(pion_neg);

        pion_neg->parameter("bb").fix(wrapToX(BetheBlochHelper::makeBBForPdg(-211), -1));
//        pion_neg->parameter("sigma").range(0.02, 0.13);
        pion_neg->parameter("sigma").fix("1.25402e-01 + 1.03784e-02*x");

        pion_neg->print();
    }

//    {
//        auto kaon_neg = new ShineDeDxParticleFitModel(-321);
//        kaon_neg->fillParticleInfoFromDB();
//        kaon_neg->setRange(-5.5, -3.);
//        kaon_neg->setRooVarPrefix("kaon_neg_");
//        fitterHelper.addParticleModel(kaon_neg);
//
//        kaon_neg->parameter("bb").fix(wrapToX(BetheBlochHelper::makeBBForPdg(-321), -1));
//        kaon_neg->parameter("sigma").range(0.02, 0.13);
//
//        kaon_neg->print();
//    }

    {
        auto deuteron = new ShineDeDxParticleFitModel(1000010020);
        deuteron->fillParticleInfoFromDB();
        deuteron->setRange(3, 3.8);
        deuteron->setRooVarPrefix("deuteron_");
        fitterHelper.addParticleModel(deuteron);

        deuteron->parameter("bb").fix(wrapToX(BetheBlochHelper::makeBBForPdg(1000010020), 1));
        deuteron->parameter("sigma").range(0.05, .3);

        deuteron->print();
    }


    {
        auto positron = new ShineDeDxParticleFitModel(-11);
        positron->fillParticleInfoFromDB();
        positron->setRange(0.4, 4.);
        positron->setRooVarPrefix("positron_");
        fitterHelper.addParticleModel(positron);

        positron->parameter("bb").fix(wrapToX(BetheBlochHelper::makeBBForPdg(11), 1));
        positron->parameter("sigma").range(0.07, 0.13);

        positron->print();
    }

    {
        auto electron = new ShineDeDxParticleFitModel(11);
        electron->fillParticleInfoFromDB();
        electron->setRange(-4., 0.);
        electron->setRooVarPrefix("electron_");
        fitterHelper.addParticleModel(electron);

        electron->parameter("bb").fix(wrapToX(BetheBlochHelper::makeBBForPdg(11), -1));
        electron->parameter("sigma").range(0.07, 0.13);

        electron->print();
    }

    inputHistogram->RebinX(3);
    inputHistogram->RebinY(2);

    auto c = new TCanvas;
    c->Print("output.pdf(", "pdf");

    inputHistogram->Draw("colz");



    for (int i = 1; i < inputHistogram->GetXaxis()->GetNbins(); ++i) {
        double x = inputHistogram->GetXaxis()->GetBinCenter(i);

        if (fitterHelper.particlesModelsDefinedAt(x).empty()) continue;

        auto py = inputHistogram->ProjectionY("tmp", i, i);
        py->SetDirectory(0);
        if (py->Integral() < 1000) continue;

        fitterHelper.at(x);
        fitterHelper.applyAllParameterConstraints();

        auto frame = fitterHelper.getObservable()->frame();

        RooDataHist ds("ds", "", *fitterHelper.getObservable(), py);
        ds.plotOn(frame);

        auto model = fitterHelper.getCompositePdfAtX();
        model->fitTo(ds, RooFit::Extended());

        fitterHelper.pickAllFitResults();

        model->plotOn(frame);
        frame->Draw();

        auto text  = new TText(0.7, 0.8, Form("x = %f", x));
        text->SetNDC();
        text->Draw();

        gPad->SetLogy();

        c->Print("output.pdf", "pdf");

    }

    c->Print("output.pdf)", "pdf");

    TFile outputFile("output.root", "RECREATE");
    fitterHelper.saveAllTo(&outputFile);

    return 0;
}

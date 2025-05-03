#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC3.h"
#include "HepMC3/WriterAscii.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TParticle.h"
#include "TLorentzVector.h"

// here is the command I used to compile -- but I had a manual build for hepmc3. Your compile command could likely be different.
'''
g++ -std=c++17 -o EventGenHepAndRoot EventGenHepAndRoot.cpp \
    $(root-config --cflags --libs) \
    -I$PYTHIA8/include -L$PYTHIA8/lib -lpythia8 \
    -I$HEPMC3/include -L$HEPMC3/lib -lHepMC3
'''

using namespace Pythia8;

void GenerateEventsWithTParticles() {
    // Initialize Pythia
    Pythia pythia;
    pythia.readString("HardQCD:all = on");
    pythia.readString("Beams:idA = 2212");
    pythia.readString("Beams:idB = 2212");
    pythia.readString("Beams:eCM = 13000.");
    pythia.readString("Random:setSeed = on");
    pythia.readString("Random:seed = 4");
    pythia.init();

    // Initialize HepMC3 writer
    HepMC3::WriterAscii hepmcWriter("output1.hepmc");
    HepMC3::Pythia8ToHepMC3 toHepMC;

    // Create ROOT file and TTree
    TFile* file = new TFile("output_tparticles.root", "RECREATE");
    TTree* tree = new TTree("EventTree", "Pythia Events with TParticles");

    int eventNumber;
    // Create a TClonesArray pointer that will store TParticle objects.
    TClonesArray* particles = new TClonesArray("TParticle", 1000);

    tree->Branch("eventNumber", &eventNumber, "eventNumber/I");
    tree->Branch("particles", &particles, 256000, 0);

    // Event loop (for testing, we generate 1000 events)
    for (int iEvent = 0; iEvent < 1000; ++iEvent) {
        if (!pythia.next()) continue;

        eventNumber = iEvent;
        particles->Clear(); // Clear the container for the new event

        // Loop over particles in the event
        for (int i = 0; i < pythia.event.size(); ++i) {
            Particle& p = pythia.event[i];
            // Create a TLorentzVector for momentum and vertex position.
            TLorentzVector mom(p.px(), p.py(), p.pz(), p.e());
            TLorentzVector vtx(p.xProd(), p.yProd(), p.zProd(), p.tProd());
            // Construct a TParticle with:
            // TParticle(Int_t pdg, Int_t status, Int_t mother1, Int_t mother2,
            //           Int_t daughter1, Int_t daughter2,
            //           const TLorentzVector &momentum, const TLorentzVector &vertex)
            new((*particles)[i]) TParticle(
                p.id(), p.statusHepMC(), p.mother1(), p.mother2(),
                p.daughter1(), p.daughter2(), mom, vtx
            );
        }

        tree->Fill(); // Save the event's data into the tree

        // Optionally, write the HepMC event as well.
        HepMC3::GenEvent hepmcEvent;
        toHepMC.fill_next_event(pythia, &hepmcEvent);
        hepmcWriter.write_event(hepmcEvent);
    }

    // Finalize: close the HepMC writer and write the ROOT tree.
    hepmcWriter.close();
    tree->Write();
    file->Close();
}

int main() {
    GenerateEventsWithTParticles();
    return 0;
}
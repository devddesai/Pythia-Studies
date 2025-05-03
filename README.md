<p align="center">
  <img src="PHY 371C Presentation/images/background.png" alt="Particle Physics Banner" width="100%">
</p>

# PHY 371C Pythia Tutorial

Hi guys! This repository contains the culmination of my work in PHY 371C. I aim to present a tutorial on how to conduct Pythia studies and get through the most difficult part -- the setup. 



## Table of Contents

### EventGen  
- **Python script**: generate customizable events, saved as ROOT Tree files for downstream analysis  
- **C++ script**: produce identical ROOT Tree output (plus optional hepMC3 ASCII format)  

### Analysis Notebooks  
- Jupyter notebooks with in‑depth analysis, in‑line commentary, and accompanying Markdown snippets  
- **NOTE** In my notebooks, I often output ROOT Canvases which shows an interactive histogram. This is not compatible with GitHub, so it is not visible through web, so there are long gaps in the notebooks. 

### PHY 371C Presentation  
- LaTeX source files and compiled PDF slides providing a clear, high‑level overview of the project  

### Ravi Koka Thesis  
- Concise theoretical background by our friend Ravi Koka  
- Full thesis repository: https://github.com/ravikoka/broad  

---

## Overview of what I will be discussing here:
- Package dependencies  
- Pythia Setup  
- Analysis  

---

## Package Dependencies

I worked on a **Mac**, though **Linux-based systems** are generally preferred for this kind of workflow.  
If you're using **Windows**, that's totally fine - just consider using **WSL** (Windows Subsystem for Linux) or another Linux distribution.  
WSL is easy to install; setup instructions can be found here:  
➡️ https://learn.microsoft.com/en-us/windows/wsl/install  

---

I tested my code using both **Python 3** and **C++**, but Python is all you really need to get started and follow along.  
*(C++ is optional and only used in one alternate event generation script.)*

---

To install Python 3, there are plenty of online tutorials. For installing scientific libraries, I personally used **Miniconda**, and I *highly* recommend it:  
➡️ https://www.anaconda.com/docs/getting-started/miniconda/main

Miniconda is kind of like `pip`, but more powerful:
- It lets you create **isolated environments**
- You can install packages without breaking other projects
- It makes dependency management way easier

---

## **Dependencies: Quick Setup (Recommended)**

First, create a Conda environment and activate it:

```bash
conda create -n <your_environment_name>
conda activate <your_environment_name>
```

When you open a terminal, you might see something like this in your prompt: ``<username> (base) .....``

That means you’re currently in the **base** environment.
It’s a good idea to create a new environment for each project, especially when some libraries might conflict with each other.

After creating your environment, activate it using:

```bash
conda activate <your_environment_name>
```

Now your terminal prompt should look like: `<username> (<your_environment_name>)`


---

**Installing libraries is super straightforward.** For most packages, you can use: ``conda install <package_name>``. But for scientific and up-to-date packages, **installing from Conda-Forge** is better: `conda install -c conda-forge <package_name>`. Just make sure you are in the environment which you intend to download packages before installing anything.


The packages I used were:

- `root`: a data analysis framework by CERN for high energy physics  
- `pythia8`: the monte carlo event generator which gives us our simulated particle physics data  
- `numpy`: a math library which has some useful functions  
- `tqdm`: not necessary, but it gives a progress bar on how for loops to see how long code might take  
- `pyhepmc`: not necessary, if you wanted to play around with the hepMC output. i used root data structure

### Quick Install: 

```bash
conda install -c conda-forge root pythia8 numpy tqdm pyhepmc
```



# Pythia Event Generator

Pythia is a simulator. to start it up, you create a pythia object instance:

```python
pythia = pythia8.Pythia()
```

Before running the simulation, you can adjust all the knobs and switches that are allowed using:

```python
pythia.readString("<your setting>")
```

The settings which I used included:

- `HardQCD:all = on`: Turns on all hard scattering processes, needed for producing heavier quarks like charm  
- `PhaseSpace:pTHatMin = 10`: Minimum pT transfer after first collision, in GeV/c  
- `Beams:idA = 2212`: PDG of the first beam (proton)  
- `Beams:idB = 2212`: PDG of the second beam (proton)  
- `Beams:eCM = 13000.`: Energy of the center of mass beam, in GeV  
- `Random:setSeed = on`: Allows to set random seed  
- `Random:seed = 4`: Setting random seed to 4 to let results be reproducable

To lock the settings in place, and get ready for event generation, call:

```python
pythia.init()
```

To get more information on the event generator, navigate to `pygen.ipynb`, or for more on Pythia: https://www.pythia.org/



# Data Structure

The data structure to store the pythia events is a ROOT Tree which is branched on ROOT's TParticle Class. The overall structure is:
- ROOT File
    - EventTree (ROOT Tree)
        - `TClonesArray`: An array of `TParticle` objects, each array containing all particles in their respective events.
            - `TParticle`: A ROOT object meant to store and query particles, allowing for easy access to kinematic and production vertex information.

---

**`TParticle` Attributes** (along with many others, found at https://root.cern.ch/doc/master/classTParticle.html ):
- `.Px()` – x-component of momentum  
- `.Py()` – y-component of momentum  
- `.Pz()` – z-component of momentum  
- `.Pt()` – transverse momentum  
- `.E()` – energy  
- `.Rapidity()` – (same as `.Y()`)  
- `.Eta()` – pseudorapidity  
- `.Phi()` – azimuthal angle  
- `.GetPdgCode()` – returns the PDG ID of the particle  
- `.GetMother(0)` – index of first mother  
- `.GetMother(1)` – index of second mother  
- `.GetDaughter(0)` – index of first daughter  
- `.GetDaughter(1)` – index of last daughter  
- `.IsPrimary()` – true if the particle is a primary one  

For more reference on a ROOT Tree: https://root.cern.ch/doc/master/classTTree.html  

---

You can also use **C++ to generate events**, which may have some efficiency advantages. To install the dependencies, you can use homebrew if on Mac: 

- **Install Homebrew**: https://brew.sh/ 
- Tap homebrew-hep: 
```bash
brew tap davidchall/hep
```
- Learn about a package: `brew info <package>`
- Install a package: `brew install <package>`
- Install pythia and root using brew, and if you want, hepmc.
    - the C++ generator file should work using these three. *(I used hepMC3 ASCII format, but hepMC3 also has a root and root tree format that i couldn't get to work due to dependency issues.)*
```bash
brew install root pythia8 hepmc3
```

# Analysis

The basic format for analysis is to open the ROOT file, loop through events, and analyze particle content. You first attach a TClonesArray to match the "Particles" branch. You set the branch address so ROOT knows where to load the data for each entry. Once set up, you loop through the entries using `tree.GetEntry()` and then loop through the particles inside that event.

You can then extract any information from the `TParticle` objects—for example, checking their PDG codes, filling histograms, or computing observables like delta phi.

The `Analysis Notebooks` demonstrate all of this: setup, looping, filtering based on PDG code, and histogramming. The logic is adaptable—whether you want to look at charm hadron pairs, single particles, or correlations. Each event is isolated and processed one at a time.

---

To plot histograms, you'll usually:

* Create your histograms before the loop using `TH1F` or `TH2F`
* Fill them inside the loop
* At the end, draw them on a canvas:

```python
c = rt.TCanvas()
h_pt.Draw()
```

This gives you visual access to the kinematic distributions you're studying. ROOT also supports complex plotting, fitting, and overlays—all accessible through its Python interface.

---

Feel free to go through my analysis notebooks, which have some in line and .md commentary to help you follow. A lot of the logic and especially the plotting on histograms can be done well by AI tools such as chatGPT or Claude Sonnet -- feel free to use these to your aid to understand and expedite analysis and plotting.

I also recommend going through my slides in conjunction with the notebooks, as there is a more logical flow and explanation of what the plots are showing in the slides.

---

If there are any **errors** or you wish to **make contributions**, please [shoot me an email.](mailto:desaidev37@gmail.com), or submit a **pull request**.
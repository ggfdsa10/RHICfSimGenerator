Three different example tools to make one simple CRMC plot are included here:

1. C++ version to analyse `.hepmc3.gz` files`.  This is
   recommended. It is the most performant for larger datasets.

   If HepMC3 was found while building CRMC, then

        make -C ExampleAnalyser analysis

   will build the example.   Try for example

        ./crmc -f test.hepmc3
	./ExampleAnalyser/analysis test.hepmc3 

2. C++ version to analyse `.root` files.  Only if ROOT is present and
   output was generated in ROOT files Just type `make analysisROOT`

   If ROOT was found while building CRMC, then

        make -C ExampleAnalyser analysisROOT

   will build the example.  Try for example

        ./crmc -f test.root -o root
	./ExampleAnalyser/analysisROOT test.root 

3. Jupyter Python notebook using
   [`pyhepmc_ng`](https://github.com/scikit-hep/pyhepmc) to plot
   `.hepmc3` files While this is very flexible, it has poorer
   performance than 1.

   The notebook assumes the input file
   `./crmc_eposlhc_324385492_p_p_6500.hepmc3` exists.  If not, the
   name will need to be changed in the notebook.  To start the
   notebook, do
   
	jupyter notebook CRMC.ipynb

   
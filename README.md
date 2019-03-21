Mixture Model for Andromeda Globular Clusters
=============================================

(c) 2019 Brendon J. Brewer and Geraint F. Lewis.

This is free software
released under the terms of the MIT license.
It was used to do the model comparison and
parameter estimation calculations for our
forthcoming paper
_Kinematic evidence for two dominant accretion epochs in the assembly of the Andromeda galaxy_
by Dougal Mackey, Geraint F. Lewis, Brendon J. Brewer, and collaborators.


Instructions to Reproduce Results in the Paper
==============================================

You will require all the dependencies of [DNest4](https://github.com/eggplantbren/DNest4),
plus [corner.py](https://github.com/dfm/corner.py).
First, clone this repo recursively and compile the C++:

```
git clone --recursive https://github.com/eggplantbren/AndromedaMixture
cd AndromedaMixture
make
```

Install the DNest4 Python package (unless you already have it), like this (Mac users, see the DNest4 manual for something extra you need to do):

```
cd DNest4/python
python setup.py install
```

Now go back to the directory containing the binary, and run DNest4 with ten threads
and a random number seed of 1 (this will run model V2, the favoured model):

```
cd ../..
./main -t 10 -s 1
```

Once that's finished (it produces 100,000 saved particles), run the postprocessing script:

```
python showresults.py
```

This will produce three DNest4 plots and output the marginal likelihood to the screen
and posterior samples to `posterior_sample.txt` (one parameter per column, see the header
line for the meaning of each column).
After closing those plots, it will produce the corner plot and save it to corner.png.

Changing the rotation model
===========================

The default rotation model is V2 from the paper. Modify lines 13 and 14 of `main.cpp`
and re-compile by running `make` to use a different rotation model. For example, to
use model S1, these lines should be modified to  

```
constexpr RotationModel rotation_model = RotationModel::S;
constexpr int num_components = 1;
```

To exactly reproduce the results from the paper, don't forget to use the
correct random number seed from Table 3.

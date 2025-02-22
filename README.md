# Vector Slicer

## About
Vector Slicer is a program for the generation of print paths for 2D planar patterns in which both the shape 
and the preferred printing direction are defined. It allows both vector and director operation making it suitable
for 3D printing of anisotropic materials such as liquid crystal elastomers (LCEs) or polymers (LCPs). 

The program by default uses splay-based seeding which analyses the pattern in order to find regions where the paths 
have locally diverged the most, and seeds paths there. Furthermore, the filling is determined by generating parameters
(GPs) which are optimised using Bayesian optimisation (Bayesopt, https://github.com/rmcantin/bayesopt) in order
to minimise the user-defined penalty function. 

As an ideal slicing of a pattern where the direction is prescribed is not generally possible, one has to trade 
the amount of holes, overlaps and director agreement in order to obtain the best slicing for the application, which can be 
realised by modification of the penalty function.

Please contact Michał Zmyślony at mlz22@cam.ac.uk with any comments and suggestions.

## Citing
If used in the research work that will be published, please use the following publication for citation:

[1] Michał Zmyślony M., Klaudia Dradrach, John S. Biggins,
Slicing vector fields into tool paths for additive manufacturing of nematic elastomers,
Additive Manufacturing, Volume 97, 2025, 104604, ISSN 2214-8604, https://doi.org/10.1016/j.addma.2024.104604.

***

## Installation guide
Please ensure that system specific C++ compiler, git (https://git-scm.com/) and cmake (https://cmake.org/) are installed.

### Linux
In the desired parent directory (it will create a subdirectory _vector_slicer_) run 
```
git clone https://github.com/Zmmyslony/vector_slicer.git
cd vector_slicer
sudo chmod +x ./install_lin.sh
./install_lin.sh
```
Please export environment variables according to the instructions displayed after installation
in order for the python scripts to work as intended.

### macOS
**Warning: This software uses OpenMP and so is incompatible with AppleClang.
By default, the installation script uses LLVM.**

In the desired parent directory run
```
git clone https://github.com/Zmmyslony/vector_slicer.git
cd vector_slicer
sudo chmod +x ./install_mac.sh
./install_mac.sh
```
Please export environment variables according to the instructions displayed after installation
in order for the python scripts to work as intended.

### Windows
In the desired parent directory run in terminal
```
git clone https://github.com/Zmmyslony/vector_slicer.git
cd vector_slicer
install_win.bat
```

***

## Usage
The program can be used either as a dynamically linked library controlled from Python or directly from C++ using 
configuration files (see description below). 

For most users Python operation is recommended as it contains input file generation, slicing and reading the outputs, 
while C++ usage requires the input files to be separately generated. 

Due to the variety of gcode formats in use, the slicer does NOT generate the output in gcode, but rather in abstract
pixel-based coordinates. There is a separate package for translating it into gcode (https://github.com/Zmmyslony/VectorSlicerGCode), 
however, at the time of writing this, it directly supports Hyrel's System 30M and Prusa's MK4S, but can be configured
for other printers as well.

***

### Python usage
The Python interface can be found in _python_ subdirectory. For an example usage check **python/guide.py**.

#### Input generation
The input patterns are created by combining _Shape_ and _Director_ into _Pattern_, which then can be used to 
generate the input files. A few commonly used shapes and directors come predefined in **shapes.py** and **directors.py**.

***

### C++ usage
#### Input format
The program requires the input director pattern to be a directory containing two matrices - one boolean which needs to be named
**shape.csv** which tells which pixels ought to be filled, and another called **thetaField.csv** which is the angle 
that the director makes with the x-axis. Alternatively, **thetaField.csv** can be replaced by two matrices called **xField.csv**
and **yField.csv**  which  are respectively x and y components of the vector field dictating the preferred direction.

Optionally, analytically calculated splay vector $\bf s = \bf n (\nabla \cdot \bf n)$ can be provided as **splay.csv** 
matrix which improves the accuracy of seeding. Alternatively, if the vector field $\bf n$ is discontinuous or changes 
its direction, a better formulation is $Q = \bf n \otimes \bf n$ and $\bf s = Q \cdot (\nabla \cdot Q)$

Additionally, **config.txt** needs to be provided in order to control the filling. Options are 

Unoptimisable:
* _PrintRadius_ (default=5) – defines the relation between grid spacing and path width. Small
  values (<4) as they introduce numerical errors, likewise large values (>10) offer little improvement.
* _InitialSeedingMethod_ (optional, default=Splay) – options: _Splay_, _Perimeter_, _Dual_; Splay filling first searches 
  regions of maximum divergence and seeds lines there, after that it switches to Perimeter which finds areas on the 
  perimeter where either the splay is zero or the splay vector points outwards and then seeds them, and then dual line 
  the approach is taken in order to ensure complete filling. 
* _StepLength_ (default=10) – maximal (and usual) length of each segment of the path. Too short segments will 
  create patterns that are too dense for the printer buffer to handle.

Optimisable:
* _Repulsion_ (default=0) – strength of repulsion from preexisting paths in range _RepulsionRadius_.
* _RepulsionAngle_ (default=3.14) – the maximum angle between repulsed direction and preferred. 
* _SeedSpacing_ (default=2 * PrintRadius)
* _Seed_ (default=0) –  used only when using the function "recalculateBestConfig".
* _TerminationRadius_ (default=5) – minimal distance between two separate paths for 
  propagation to continue.
* _RepulsionRadius_ (default=0) – legacy, allows the pattern to repulse from paths further
 than the print radius away from itself.

#### Configuration
There are five text files which configure the execution of the program and they are all contained
in the <b>configuration</b> directory:

* <b>files_to_test.txt</b> – here we list using either relative or absolute paths to the directories containing the
  shape matrix and director field. Multiple directories are allowed.
* <b>disagreement.cfg</b> – sets the number of threads and seeds for each step of optimisation and the final search.
* <b>bayesian_optimisation.cfg</b> – configures the number of iterations, relearning period and other basic
  configurations.
* <b>disagreement_function.cfg</b> – defines the disagreement function used in the optimisation.
* <b>filling.cfg</b> – switching between vector and director operation, control over removal of points and short lines.

The locations of each of the configuration files together with the directories used for importing and exporting can
be modified in the **vector_slicer_config.h.in** file.

***

### Output
The program saves the patterns in the **output** directory, with the outputs of each type in their subdirectory with 
names corresponding to the names of the patterns used. 

The main output file is the **paths** file which contains the slicing information. The format is as follows:
* Layer information is contained between "# Start of pattern" and "# End of pattern". Each layer corresponds to single 
seed.
* Each line within a layer contains information about single continuous path, where the values are flattened 
pixel-based coordinates, i.e. x1, y1, x2, y2, etc.

Remaining files:
* **best_config** - config and seeds used for the generation of paths.
* **filled_matrices** - contains information about how many times was each pixel fixed for the first (best) seed.
* **logs** - Bayesopt logs used for debugging.
* **optimisation_save** - Bayesopt optimisation progress for re-optimisation or optimisation monitoring.
* **used_seeds** - list of coordinates of seeds used for propagation of paths in the best numerical seed.
***

## Funding
<img alt="EU logo" src="https://ec.europa.eu/regional_policy/images/information-sources/logo-download-center/eu_flag.jpg" width="200">
This Project has received funding from the European Union’s Horizon 2020 research and innovation program under the Marie Skłodowska-Curie grant agreement No 956150.

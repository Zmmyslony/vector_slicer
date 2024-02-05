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

[//]: # (## Requirements)

[//]: # (Boost library - https://www.boost.org/.)
****
## Installation guide
Please ensure that system specific C++ compiler, git (https://git-scm.com/), cmake (https://cmake.org/) and 
boost (https://www.boost.org/) are installed. 

### Windows
If boost is not installed we recommend using vcpkg (https://vcpkg.io/en/) which can be installed by running in the terminal
```
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
cd vcpkg
vcpkg install boost:x64-windows
vcpkg integrate install 
```
and note down the line starting with -DCMAKE_TOOLCHAIN_FILE=... .

Once boost is installed go to the desired parent directory and run in terminal
```
git clone https://github.com/Zmmyslony/vector_slicer.git
cd vector_slicer
cmake -S ./ -B ./build -DCMAKE_TOOLCHAIN_FILE=...
cmake --build ./build --config Release
```
This will build the executable in the ./build/Release directory.


### Linux / macOS
In the desired parent directory run in terminal
```
git clone https://github.com/Zmmyslony/vector_slicer.git
cd vector_slicer
cmake -S ./ -B ./build
cmake --build ./build --config Release
```


[//]: # (### macOS)

[//]: # (The guide is soon to come.)
***
## Usage
The program can be used either as a dynamically linked library controlled from Python or directly from C++ using 
configuration files (see description below). 

For most users Python operation is recommended as it contains input file generation, slicing and reading the outputs, 
while C++ usage requires the input files to be separately generated. 

Due to the variety of gcode formats in use, the slicer does NOT generate the output in gcode, but rather in abstract
pixel-based coordinates. There is a separate program for translating it into gcode (https://github.com/Zmmyslony/Vector_Slicer_GCode), 
however, at the time of writing this only directly supports Hyrel printers (specifically, System 30M), but other formats
can also be implemented on request.

### Python usage
The Python interface can be found in _python_ subdirectory. For an example usage check _python/guide.py_.

#### Input generation
The input patterns are created by combining **Shape** and **Director** into **Pattern**, which then can be used to 
generate the input files. A few commonly used shapes and directors come predefined in _shapes.py_ and _directors.py_.


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
* _InitialSeedingMethod_ (optional, default=Spla) – options: _Splay_, _Perimeter_, _Dual_; Splay filling first searches 
  regions of maximum divergence and seeds lines there, after that it switches to Perimeter which finds areas on the 
  perimeter where either the splay is zero or the splay vector points outwards and then seeds them, and then dual line 
  the approach is taken in order to ensure complete filling. 
* _StepLength_ (default=10) – maximal (and usual) length of each segment of the path. Too short segments will 
  create patterns that are too dense for the printer buffer to handle.

Optimisable:
* _Repulsion_ (default=0) – strength of repulsion from preexisting paths in range _RepulsionRadius_.
* _RepulsionAngle_ (default=3.14) – the maximum angle between repulsed direction and preferred. 
* _SeedSpacing_ (default=2 * PrintRadius)
* _Seed_ (default=0, isOptimisable=True) –  used only when using the function "recalculateBestConfig".
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

### Output
The program saves the patterns in the **output** directory, where it saves the optimised config, the filled matrix and
the paths. The resulting files will be named based on the name of the input directory.

The filled matrix shows how many times was each pixel filled which is a fast test for the quality.

The paths file contains the main output of the
program. The header contains some relevant information as to how it was sliced, and then the between "# Start of pattern"
and "# End of pattern" is the sequence of paths corresponding to a single layer. Each line corresponds to a single line, where the x and y coordinates
of subsequent points alternate (that is: x1,y1,x2,y2,x3,y3, etc.). Each of the patterns corresponds to a fully sliced
pattern which uses different seeds so that in the case of multi-layer prints there is some variability between layers
to avoid systematic errors.

The simplest way of translating the output file into .gcode is to use an associated program Vector Slicer GCode generator
(https://github.com/Zmmyslony/Vector_Slicer_GCode). It was designed to be used with the Hyrel printer (System 30M) for DIW writing 
and the gcode headers and footers are Hyrel-specific, but the body of the gcode is rather universal. 

The best configuration file is saved for future use so it can be used for recalculation.


## Funding
This Project has received funding from the European Union’s Horizon 2020 research and innovation program under the Marie Skłodowska-Curie grant agreement No 956150.

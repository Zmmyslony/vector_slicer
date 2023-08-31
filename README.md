# Vector Slicer

## About
Vector Slicer is a program for generation of print paths for 2D planar patterns in which both the shape 
and the preferred printing direction is defined. It allows both vector and director operation making it suitable
for 3D printing of anisotropic materials such as liquid crystal elastomers (LCEs) or polymers (LCPs). 

The program by default uses splay-based seeding which analyses the pattern in order to find regions where the paths 
has locally diverged the most, and seeds paths there. Furthermore, the filling is determined via generating parameters
(GPs) which are optimised using Bayesian optimisation (Bayesopt, https://github.com/rmcantin/bayesopt) in order
to minimise the penalty function. 

As an ideal slicing of a pattern where the direction is prescribed is not generally possible, one has to trade 
the amount of holes, overlaps and director agreement in order to obtain the best slicing for the application. This
can be done by changing the weights of the penalty function. 

## Requirements
Boost library - https://www.boost.org/.

## Installation guide
### Windows
In the directory where the program will be installed run a command
````asm
    git pull https://github.com/Zmmyslony/vector_slicer/
````

### Linux

## Input format
The program requires the input director pattern to be a directory containing two matrices - one boolean which needs to be named
**shape.csv** which tells which pixels ought to be filled, and another  called **thetaField.csv** which is the angle 
that director makes with x-axis. Alternatively, **thetaField.csv** can be replaced by two matrices called **xField.csv**
and **yField.csv**  which  are respectively x and y components of the vector field dictating the preferred direction.

Optionally, analytically calculated splay vector $\bf s = \bf n (\nabla \cdot \bf n)$ can be provided as **splay.csv** 
matrix which improves the accuracy of seeding. Alternatively, if the vector field $\bf n$ is discontinuous or changes 
its direction, a better formulation is $Q = \bf n \otimes \bf n$ and $\bf s = Q \cdot (\nabla \cdot Q)$

Additionally, **config.txt** needs to be provided in order to control the filling. Options are 
* _PrintRadius_ (default=5, isOptimisable=False) - defines the relation between grid spacing and path width. Small
  values (<4) as they introduce numerical errors, likewise large values (>10) offer little improvement.
* _InitialSeedingMethod_ (optional, default=Splay, isOptimisable=False) - options: _Splay_, _Perimeter_, _Dual_; Splay filling first searches 
  regions of maximum divergence and seeds lines there, after that it switches to Perimeter which finds areas on the 
  perimeter where either the splay is zero or the splay vector points outwards and then seeds them, and then dual line 
  approach is taken in order to ensure complete filling. 
* _StepLength_ (default=10, isOptimisable=False) - maximal (and usual) length of each segment of the path. Too short segements will 
  create patterns that are too dense for the printer buffer to handle.


* _Repulsion_ (default=0, isOptimisable=True) - strength of repulsion from preexisting paths in range _RepulsionRadius_.
* _RepulsionAngle_ (default=3.14, isOptimisable=True) - maximum angle between repulsed direction and preferred. 
* _SeedSpacing_ (default=2 * PrintRadius, isOptimisable=True)
* _Seed_ (default=0, isOptimisable=True, listable=True) -  used only when using the function "recalculateBestConfig".
* _TerminationRadius_ (default=5, isOptimisable=True) - minimal distance between two separate paths for 
  propagation to continue.
* _RepulsionRadius_ (default=0, isOptimisable=False) - legacy, allows the pattern to repulse from paths further
  further than print radius away from itself.

## Configuration
There are five text files which configure the execution of the program and they are all contained
in the <b>configuration</b> directory:

* <b>files_to_test.txt</b> - here we list using either relative or absolute paths to the directories containing the
  shape matrix and director field. Multiple directories are allowed.
* <b>disagreement.cfg</b> - sets number of threads and seeds for each step of optimisation and the final search.
* <b>bayesian_optimisation.cfg</b> - configures the number of iterations, relearning period and other basic
  configurations.
* <b>disagreement_function.cfg</b> - defines the disagreement function used in the optimisation.
* <b>filling.cfg</b> - switching between vector and director operation, control over removal of points and short lines.

The locations of each of the configuration files together with the directories used for importing and exporting can
be modified in the **vector_slicer_config.h.in** file.

## Output

The program saves the patterns in the **output** directory, where it saves the optimised config, the filled matrix and
the paths. The resulting files will be named based on the name of the input directory.

The filled matrix shows how many times was each pixel filled which is a fast test for the quality.

The paths file contains the main output of the
program. The header contains some relevant information as to how it was sliced, and then the between "#Start of pattern"
and "# End of pattern" is the sequence of paths. Each line corresponds to a single line, where the x and y coordinates
of subsequent points alternate (that is: x1,y1,x2,y2,x3,y3, etc.). Each of the patterns corresponds to a fully sliced
pattern which used different seeds, so that in the case of multi-layer prints there is some variability between layers
to avoid systematic errors.

The simplest way of translating the output file into .gcode is to use an associated program Vector Slicer GCode generator
(https://github.com/Zmmyslony/Vector_Slicer_GCode). It was designed to be used with the Hyrel printer for DIW writing 
and the gcode headers and footers are Hyrel specific, but the body of the gcode is rather universal. 

Best configuration file is saved for future use.


# Vector Slicer

## About

Vector Slicer is a program which is used to generate a set of coordinates forming a print
path which at each point tries to follow the preferred direction defined by the vector (director) field.
The slicer was designed to be used for 3D printing of liquid crystal elastomers, in which the
direction of printing decides the deformation of the material when heated.

The slicer generates the print patterns from a set of generating parameters, and then optimises
the generating parameters using the Bayesopt library (https://github.com/rmcantin/bayesopt)
in order to minimise the penalising function. Penalising function
can be tuned so that the overlap, number of empty spots, agreement with the desired director or
total number of lines are minimised. Each set of generating parameters is calculated for
a number of seeds in order to avoid locking on local minima.

## Input format

The program requires the input director pattern to be in form of three matrices - one boolean which needs to be named
**shape.csv** which tells which pixels ought to be filled, and two more called **xField.csv** and **yField.csv**  which
are respectively x and y components of the vector field dictating the preferred direction.

In addition to that, **config.txt** is required, with _InitialFillingMethod_, _StepLength_ and _PrintRadius_ to be
defined. _InitialFillingMethod_ can either be _Perimeter_ or _Dual_, where the perimeter approach
starts by separating the boundary of the shape into equidistant points, from which lines will be started,
while the dual approach starts the line in a random point within the pattern that has low splay. _StepLength_ is
an integer telling how long is each step in pixels, and _PrintRadius_ is a double variable deciding the radius of
where the points will be filled.

## Configuration

There are five text files which configure the execution of the program and they are all contained
in the <b>configuration</b> directory:

* <b>files_to_test.txt</b> - here we list using either relative or absolute paths to the directories containing the
  shape matrix and director field. Multiple directories are allowed.
* <b>disagreement.cfg</b> - sets number of threads and seeds for each step of optimisation and the final search.
* <b>bayesian_optimisation.cfg</b> - configures the number of iterations, relearning period and other basic
  configurations.
* <b>disagreement_function.cfg</b> - defines the disagreement function used in the optimisation.
* <b>filling.cfg</b> - controls the filling algorithm.
  The locations of each of the configuration files can be modified in the **vector_slicer_config** file.

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

Best config can be used to recalculate the optimised system once again.

## Requirements

The only requirement to run this code is to have the boost library, although OpenMP is highly recommended.
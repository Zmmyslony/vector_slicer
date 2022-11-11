# Vector Slicer

## About

Vector slicer is a program which is used to generate a set of coordinates which create a print
path which at each point tries to follow the preferred direction defined by the vector field (director).
The slicer was designed to be used for 3D printing of liquid crystal elastomers, in which the
direction of printing decides the deformation of the material when heated.

The slicer generates the print patterns from a set of generating parameters, and then optimises
the generating parameters in order to minimise the penalising function. Penalising function
can be tuned so that the overlap, number of empty spots, agreement with the desired director or
total number of lines are minimised. Each set of generating parameters is calculated for 
a number of seeds in order to avoid finding local minima. 

## Configuration

There are three text files which determine the execution of the program. 
* <b>config.txt</b> - tells the program what is the starting seed, finishing seed and how many threads should be used
* <b>filesToTest.txt</b> - here we write either relative or absolute paths to the directories containing the shape matrix and director field. Multiple directories are allowed.
* <b>optimisationSequence.txt</b> - here we define the order in which the generating parameters will be optimised, how many different steps should be used and what is the range of the parameter that we are going to be searching through.

## Requirements

The only requirement to run this code is to have the boost library.
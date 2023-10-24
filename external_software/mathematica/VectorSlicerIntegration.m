(* ::Package:: *)

(* ::Title:: *)
(*Integration with Vector Slicer*)


(* ::Author:: *)
(*Micha\[LSlash] Zmy\:015blony*)


(* ::Affiliation:: *)
(*University of Cambridge*)


(* ::Section:: *)
(*Package functions*)


BeginPackage["VectorSlicerIntegration`"]
Needs["VectorSlicerInputGenerator`"]
Needs["VectorSlicerOutputReader`"]

GenerateInputFromCartesianTheta::usage = 
	"GenerateInputFromCartesianTheta[exportDirectory, patternName, region, \[Theta]Director, lineWidthMillimetre, lineWidthPixel,fillingMethod] generates input files to be used with Vector Slicer for director defined as \[Theta][{x,y}] where \[Theta] is the angle between director and x-axis."

GenerateInputFromCartesianDirector::usage = 
	"GenerateInputFromCartesianTheta[exportDirectory, patternName, region, \[Theta]Director, lineWidthMillimetre, lineWidthPixel,fillingMethod] generates input files to be used with Vector Slicer for director defined as n[{x,y}]={nx,ny}."

GenerateInputFromRadialTheta::usage = 
	"GenerateInputFromRadialTheta[exportDirectory, patternName, region, \[Theta]Director, lineWidthMillimetre, lineWidthPixel,fillingMethod] generates input files to be used with Vector Slicer for director defined as \[Theta][r] where \[Theta] is the angle between director and x-axis."
	
PlotFillMatrix::usage = 
	"PlotFillMatrix[fillMatrixDirectory,patternName] plots the resulting fill matrix "

PlotPrintingMoves::usage = 
	"PlotPrintingMoves[lineSourceDirectory,patternName] plots printing and non-printing movements"
	
PlotSlicedPattern::usage = 
	"PlotSlicedPattern[fillMatrixDirectory,linesDirectory,patternName] plots both fill matrix and all printer moves"

ExportSlicedPattern::usage = 
	"ExportSlicedPattern[fillMatrixDirectory,linesDirectory,patternName] plots and exports the pattern"

PlotOptimisationSequence::usage = 
	"PlotOptimisationSequence[optimisationSequenceDirectory,patternName] plots how the penalty function changed through optimisation"


Begin["Private`"]

GenerateInputFromCartesianTheta[exportDirectory_, patternName_, region_, \[Theta]Director_, lineWidthMillimetre_, lineWidthPixel_,fillingMethod_]:=
	GenerateInputAndExport[GenerateFieldsCartesian,exportDirectory, patternName, region, \[Theta]Director, lineWidthMillimetre, lineWidthPixel,fillingMethod];

GenerateInputFromCartesianDirector[exportDirectory_, patternName_, region_, \[Theta]Director_, lineWidthMillimetre_, lineWidthPixel_,fillingMethod_]:=
	GenerateInputAndExport[GenerateFieldsCartesianVector,exportDirectory, patternName, region, \[Theta]Director, lineWidthMillimetre, lineWidthPixel,fillingMethod];

GenerateInputFromRadialTheta[exportDirectory_, patternName_, region_, \[Theta]Director_, lineWidthMillimetre_, lineWidthPixel_,fillingMethod_]:=
	GenerateInputAndExport[GenerateFieldsRadial,exportDirectory, patternName, region, \[Theta]Director, lineWidthMillimetre, lineWidthPixel,fillingMethod];

PlotFillMatrix[fillMatrixDirectory_,patternName_]:=Module[{results,formatedFilename},
	formatedFilename=FileNameJoin[{fillMatrixDirectory,patternName<>".csv"}];
	If[! TrueQ @ FileExistsQ  @ formatedFilename, Message[file::nofile, formatedFilename]; Abort[]];
	results=Reverse[Import[formatedFilename],2];
	DisplayFill[results]
	]

PlotPrintingMoves[lineSourceDirectory_,filename_]:=Module[{patterns,pattern},
	patterns=cleanPatterns[separatePatterns[readPatternLines[lineSourceDirectory,filename]]];
	pattern=patterns[[1]];
	Show[drawPrintingMoves[pattern],drawNonPrintingMoves[pattern],ImageSize->Medium]
	]

PlotSlicedPattern[fillMatrixDirectory_,linesDirectory_,filename_]:=Show[
	PlotFillMatrix[fillMatrixDirectory,filename],
	PlotPrintingMoves[linesDirectory,filename]
	]
	
ExportSlicedPattern[fillMatrixDirectory_,linesDirectory_,filename_]:=Export[filename<>".pdf",PlotSlicedPattern[fillMatrixDirectory,linesDirectory,filename]]

PlotOptimisationSequence[optimisationSequenceDirectory_,patternName_]:=Module[{data,minData,minPlot},
	data=ReadOptimisationSequence[optimisationSequenceDirectory,patternName];
	minData=Table[Min[data[[1;;i]]],{i,Length[data]}];
	minPlot=ListLogPlot[minData,PlotRange->Automatic,Joined->True,Frame->{{True,False},{True,False}},FrameLabel->{"optimisation iteration", "lowest disagreement"}];
	Show[minPlot,ListLogPlot[data],PlotRange->Automatic]
	]
 
End[]

EndPackage[]




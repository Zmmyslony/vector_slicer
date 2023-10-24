(* ::Package:: *)

(* ::Title:: *)
(*Functions for reading output files from Vector Slicer*)


(* ::Author:: *)
(*Micha\[LSlash] Zmy\:015blony*)


(* ::Affiliation:: *)
(*University of Cambridge*)


(* ::Section:: *)
(*Package functions*)


BeginPackage["VectorSlicerOutputReader`"]

Begin["Private`"]

$Assumptions={_\[Element]Reals};

file::nodir = "Directory `1` does not exist.";
file::nofile = "File `1` does not exist.";

DisplayFill[data_]:=Module[{maxValue},
	maxValue=Max[data,4];
	ColorNegate[Image[Transpose[data]/maxValue,ImageSize->Medium,ColorSpace->"Grayscale"]]
	]
	
colourScale=Show[ColorNegate[Image[Transpose[{{0,0,1,2,3,4},{0,0,0,0,0,0}}]/4,ImageSize->Small,ColorSpace->"Grayscale"]],
	Epilog->Style[
		{Text["Number of \ntimes filled",{1, 5.5}],
		Text["0",{1.5, 4.5}],
		Text["1",{1.5, 3.5}],
		Text["2",{1.5, 2.5}],
		Text["3",{1.5,1.5}],
		Text["4",{1.5, 0.5}]},
		Medium],
	PlotRange->All];
(*Export["colourScale.png",%,ImageResolution->300];*)

readPatternLines[linesSourceDirectory_, filename_]:=Import[FileNameJoin[{linesSourceDirectory,filename<>".csv"}]]

separatePatterns[patterns_]:=Module[{startCoordinates,endCoordinates,cleanedData},
	startCoordinates=Position[patterns,{"# Start of pattern"}][[;;,1]];
	endCoordinates=Position[patterns,{"# End of pattern"}][[;;,1]];
	cleanedData=Table[patterns[[startCoordinates[[i]]+1;;endCoordinates[[i]]-1]],{i,Length[startCoordinates]}]
]
cleanPattern[pattern_]:=Map[Transpose[{#[[;;;;2]],#[[2;;;;2]]}]&,pattern,{1}]
cleanPatterns[patterns_]:=Map[cleanPattern,patterns,{1}]

colourBlue=ColorData[97][1];
colourOrange=ColorData[97][2];
drawPrintingMoves[pattern_]:=Graphics[{colourBlue,Map[Line,pattern]}]
drawNonPrintingMoves[pattern_]:=Graphics[{colourOrange,Line[Transpose[{pattern[[;;-2,-1]],pattern[[2;;,1]]}]]}]

ReadOptimisationSequence[optimisationSequenceDirectory_,patternName_]:=Module[{mYLine,dataLine},
	mYLine=Import[FileNameJoin[{optimisationSequenceDirectory,patternName<>".txt"}],"Table"][[-2,1]];
	dataLine=StringTake[mYLine,{StringPosition[mYLine,"("][[1,1]]+1,StringPosition[mYLine,")"][[1,1]]-1}];
	Read[StringToStream[#], Number] &/@StringSplit[dataLine, ","]
	]

End[]

EndPackage[]




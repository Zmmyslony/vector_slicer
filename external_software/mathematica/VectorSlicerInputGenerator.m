(* ::Package:: *)

(* ::Title:: *)
(*Functions for generating input files for Vector Slicer*)


(* ::Author:: *)
(*Micha\[LSlash] Zmy\:015blony*)


(* ::Affiliation:: *)
(*University of Cambridge*)


(* ::Section:: *)
(*Package functions*)


BeginPackage["VectorSlicerInputGenerator`"]

Begin["Private`"]

FastCSVExport[file_String,list_List?MatrixQ]:=
	With[{str=OpenWrite[file,PageWidth->Infinity],
	len=Length[list[[1]]]},
	Scan[
		Write[str,
		Sequence@@(Flatten[Table[{FortranForm[#[[i]]],OutputForm[","]},{i,len-1}]])~Join~{FortranForm[#[[len]]]}]&,list
		];
	Close[str];
	]
	
ExportVector[path_,vector_,format_]:=Module[{reshapedArray},
	reshapedArray=ArrayReshape[vector,{Dimensions[vector][[1]],2*Dimensions[vector][[2]]}];
	FastCSVExport[path,reshapedArray];
	]
	
PatternHeader[lineWidthMillimetre_,lineWidthPixel_]:= "# Pattern generated on " <> DateString[] <> " using line width of "<>ToString[lineWidthMillimetre]<>" mm and "<>ToString[lineWidthPixel]<>" pixels."

ExportFields[exportDirectory_, patternName_, {shapeGrid_,\[Theta]Grid_,splayGrid_},lineWidthPixel_,fillingMethod_]:=Block[{directoryPath,shapeFilename,\[Theta]Filename,splayFilename,splayGradientFilename,configurationFilename},
	directoryPath=FileNameJoin[{exportDirectory,patternName}];
	If[DirectoryQ[directoryPath],Null,CreateDirectory[directoryPath]];
	shapeFilename=FileNameJoin[{directoryPath,"shape.csv"}];
	\[Theta]Filename=FileNameJoin[{directoryPath,"theta_field.csv"}];
	splayFilename=FileNameJoin[{directoryPath,"splay.csv"}];
	splayGradientFilename=FileNameJoin[{directoryPath,"splay_gradient.csv"}];
	configurationFilename=FileNameJoin[{directoryPath,"config.txt"}];
	FastCSVExport[shapeFilename,shapeGrid];
	FastCSVExport[\[Theta]Filename,\[Theta]Grid];
	ExportVector[splayFilename,splayGrid,"CSV"];
	Export[configurationFilename,
		"PrintRadius "<>ToString[N[lineWidthPixel/2]]<>
		"\nInitialSeedingMethod "<>fillingMethod];
	Dimensions[splayGrid]
	]
	
GenerateFieldsExplicit[N\[Theta]_,NSplay_,region_,pixelSize_]:=Block[{xMin,xMax,yMin,yMax,xGrid,yGrid,regionFunction,shapeGrid,\[Theta]Grid,splayGrid,splayGradientGrid,Ndiv},
	{{xMin,xMax},{yMin,yMax}}=RegionBounds[region];
	(* Arbitrary offset of 10^-10 which is supposed to remedy singularities for patterns undefined on axis *)
	xMin+=-pixelSize+10^-10; 
	yMin+=-pixelSize+10^-10;
	xMax+=pixelSize+10^-10;
	yMax+=pixelSize+10^-10;

	regionFunction=RegionMember[region];

	xGrid=Range[xMin,xMax,pixelSize];
	yGrid=Range[yMin,yMax,pixelSize];
	shapeGrid=Outer[regionFunction[{#1,#2}]&,xGrid,yGrid]/.{True->1,False->0};
	\[Theta]Grid=N[Parallelize[Outer[N\[Theta],xGrid,yGrid]]/.{Indeterminate->0,ComplexInfinity->0}];
	splayGrid=N[Parallelize[Outer[NSplay,xGrid,yGrid]]/.{Indeterminate->0,ComplexInfinity->0}];
	{shapeGrid,\[Theta]Grid,splayGrid}
	]

nCartesian[\[Theta]Cartesian_]:=Simplify@PiecewiseExpand[{Cos[\[Theta]Cartesian[{x,y}]],Sin[\[Theta]Cartesian[{x,y}]]}];
splayCartesian[\[Theta]Cartesian_]:=Simplify@PiecewiseExpand[Simplify@PiecewiseExpand[nCartesian[\[Theta]Cartesian]]Simplify@PiecewiseExpand[Div[nCartesian[\[Theta]Cartesian],{x,y}]]]/.Indeterminate->0;

GenerateFieldsCartesian[\[Theta]Cartesian_,region_,pixelSize_]:=Block[{N\[Theta],NSplay},
	N\[Theta][x_?NumberQ,y_?NumberQ]=Quiet[N[Re[\[Theta]Cartesian[{x,y}]]]];
	NSplay[x_?NumberQ,y_?NumberQ]=Quiet[splayCartesian[\[Theta]Cartesian]];
	GenerateFieldsExplicit[N\[Theta],NSplay,region,pixelSize]
	]
	
GenerateFieldsCartesianVector[nCartesian_,region_,pixelSize_]:=Block[{N\[Theta],NSplay},
	N\[Theta][x_?NumberQ,y_?NumberQ]=Quiet[N[ArcTan[nCartesian[{x,y}][[1]],nCartesian[{x,y}][[2]]]]];
	NSplay[x_?NumberQ,y_?NumberQ]=Quiet[N[nCartesian[{x,y}] Div[nCartesian[{x,y}],{x,y}]]];
	GenerateFieldsExplicit[N\[Theta],NSplay,region,pixelSize]
	]
	
nRadial[\[Theta]Radial_]:=FullSimplify@PiecewiseExpand[{Cos[\[Theta]Radial[r]],Sin[\[Theta]Radial[r]]}];
splayRadial[\[Theta]Radial_]:=FullSimplify@PiecewiseExpand[nRadial[\[Theta]Radial] Div[nRadial[\[Theta]Radial],{r,\[CurlyPhi]}, "Polar"]]/.Indeterminate->0;
radialRotation[x_,y_]=FullSimplify[RotationMatrix[ArcTan[x,y]]];

GenerateFieldsRadial[\[Theta]Field_,region_,pixelSize_]:=Block[{N\[Theta],NSplay},
N\[Theta][x_?NumberQ,y_?NumberQ]=N[Simplify@PiecewiseExpand[\[Theta]Field[Sqrt[x^2+y^2]]+ArcTan[x,y]]];
NSplay[x_?NumberQ,y_?NumberQ]=N[Simplify@PiecewiseExpand[radialRotation[x,y] . splayRadial[\[Theta]Field]]]/.{r->Sqrt[(x-10^-30)^2+(y-10^-30)^2]};
GenerateFieldsExplicit[N\[Theta],NSplay,region,pixelSize]
]

IsUndefinedFillingMethod[fillingMethod_]:=ContainsNone[{"Splay","Perimeter","Dual"},{fillingMethod}]
GenerateInputAndExport[FieldGenerator_,exportDirectory_, patternName_, region_, \[Theta]Director_, lineWidthMillimetre_, lineWidthPixel_,fillingMethod_] := 
	Module[{header,shapeGrid,\[Theta]Grid,splayGrid},
	If[IsUndefinedFillingMethod[fillingMethod],Abort[]];
	header=PatternHeader[lineWidthMillimetre,lineWidthPixel];
	{shapeGrid,\[Theta]Grid,splayGrid}=FieldGenerator[\[Theta]Director,region,lineWidthMillimetre / lineWidthPixel];
	ExportFields[exportDirectory, patternName, {shapeGrid,\[Theta]Grid,splayGrid},lineWidthPixel,fillingMethod]
	]
	
End[]

EndPackage[]




#include "EventBuilder.h"
#include "GWMEventBuilder.h"

int main(int argc, char** argv) {
	if(argc != 3) {
		std::cerr<<"Incorrect number of command line arguments!"<<std::endl;
		std::cerr<<"Need to specify type of operation (buildSlow, buildFast, etc.) and input file."<<std::endl;
		return 1;
	}

	std::string filename = argv[2];
	std::string operation = argv[1];


	/* DEFAULT Operation Types:
		buildSlow (through slowSort and then analyze)
		buildFast (start with slowSorted and then fast and analyze)
		buildAll (all steps through analyze)
		analyzeSlow (analyze from slowSorted)
		analyzeFast (analyze from fastSorted)
		archive (make an archive of raw binary data)
		convert (convert binary archive to root data)
		merge (combine root files)
		plot (generate a default histogram file from analyzed data)
	*/

	GWMEventBuilder theBuilder;
	if(!theBuilder.ReadConfigFile(filename)) {
		return 1;
	}

	if(operation == "buildAll") {
		theBuilder.SetAnalysisType(GWMEventBuilder::BUILD_ALL);
		theBuilder.BuildEvents();
	} else if(operation == "buildSlow") {
		theBuilder.SetAnalysisType(GWMEventBuilder::BUILD_SLOW);
		theBuilder.BuildEvents();
	} else if(operation == "buildFast") {
		theBuilder.SetAnalysisType(GWMEventBuilder::BUILD_FAST);
		theBuilder.BuildEvents();
	} else if(operation == "analyzeSlow") {
		theBuilder.SetAnalysisType(GWMEventBuilder::ANALYZE_SLOW);
		theBuilder.BuildEvents();
	} else if(operation == "analyzeFast") {
		theBuilder.SetAnalysisType(GWMEventBuilder::ANALYZE_FAST);
		theBuilder.BuildEvents();
	} else if(operation == "archive") {
		int runNum;
		theBuilder.SetAnalysisType(GWMEventBuilder::ARCHIVE);
		std::cout<<"Enter the run number to be archived: ";
		std::cin>>runNum;
		std::cout<<std::endl;
		theBuilder.ArchiveBinaryFiles(runNum);
	} else if(operation == "convert") {
		theBuilder.SetAnalysisType(GWMEventBuilder::CONVERT);
		theBuilder.ConvertBin2ROOT();
	} else if(operation == "merge") {
		theBuilder.SetAnalysisType(GWMEventBuilder::MERGE);
		theBuilder.MergeROOTFiles();
	} else if(operation == "plot") {
		theBuilder.SetAnalysisType(GWMEventBuilder::PLOT);
		theBuilder.PlotHistograms();
	} else {
		std::cerr<<"Unidentified type of operation! Check your first argument."<<std::endl;
		return 1;
	}

	return 0;
}
/*
This is an executable wrapper for ODKScan.
*/
#include "Processor.h"
#include "FileUtils.h"
#include "StatCollector.h"
#include "MarkupForm.h"

#include <iostream>
#include <string>

#include <fstream>
#include <sys/stat.h>

using namespace std;

int main(int argc, char *argv[]) {

	if(argc < 3) {
		cout << "Usage:" << endl;
		cout << string(argv[0]) << " templatePath inputImage outputDirectory" << endl;
		return 0;
	}

	string templatePath(argv[1]);
	string inputImage(argv[2]);
	string outputDir(argv[3]);
	//This code ensures the out dir end with a '/'
	if(outputDir[outputDir.size() - 1] != '/') outputDir.append("/");

	string expectedJsonFile = argc > 4 ? string(argv[4]) : "";

	string outputPath(outputDir);

	//Make a directory with the name of the form
	//TODO: Move?
	mkdir(outputPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir((outputPath + "segments").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	string alignedFormOutfile(outputPath  + "aligned.jpg");
	string markedupFormOutfile(outputPath + "markedup.jpg");
	string jsonOutfile(outputPath + "output.json");
	
	cout << "Processing image: " << inputImage << endl;

	Processor myProcessor("assets/");
	//TODO: Specify camera calibration somewhere else?
	//template doesn't make sense because it a property of the input image.
	#define DO_CAMERA_CALIBRATION false
	if( !myProcessor.loadFormImage(inputImage.c_str(), DO_CAMERA_CALIBRATION)) {
		cout << "\E[31m" <<  "Could not load. Arg: " << "\e[0m" << inputImage << endl;
		return 1;
	}
	
	if( !myProcessor.loadFeatureData(templatePath.c_str()) ) {
		cout << "\E[31m" <<  "Could not set load feature data. Arg: " << "\e[0m" << templatePath << endl;
		return 1;
	}
	
	if( !myProcessor.setTemplate(templatePath.c_str()) ) {
		cout << "\E[31m" <<  "Could not set template. Arg: " << "\e[0m" << templatePath << endl;
		return 1;
	}
	
	cout << "Outputting aligned image to: " << outputPath << endl;
	
	if( !myProcessor.alignForm(alignedFormOutfile.c_str()) ) {
		cout << "\E[31m" <<  "Could not align. Arg: " << "\e[0m" << alignedFormOutfile  << endl;
		return 1;
	}
	
	if( !myProcessor.processForm(outputPath.c_str()) ) {
		cout << "\E[31m" << "Could not process. Arg: " << "\e[0m" << jsonOutfile << endl;
		return 1;
	}
	if( !MarkupForm::markupForm(jsonOutfile.c_str(), alignedFormOutfile.c_str(), markedupFormOutfile.c_str()) ) {
		cout << "\E[31m" <<  "Could not markup. Arg: " << "\e[0m" << markedupFormOutfile  << endl;
		return 1;
	}
	return 0;
}
#include "ConfigFile.hpp"
#include "DoPiv.hpp"
#include "Grid.hpp"
#include "IntMap.hpp"
#include "OcvImage.hpp"
#include "PivOptions.hpp"
#include "PivClap.hpp"
#include <memory>
#include <cmath>

int main(int argc, char** argv)
{
	/* Arguments 
	 * Do all parsing up front and raise exceptions if something is wrong
	 * and end the program.
	 *
	 * No point doing any PIV if say write output to file is selected but no
	 * filename is provided */
	PivClap clArgs(argc, argv);

	/* Load a raw images and extract pixel intensity maps */
	std::unique_ptr<IntMappable> rawIm1 = std::make_unique<OCVImage>("../img/1.bmp"),
								 rawIm2 = std::make_unique<OCVImage>("../img/2.bmp");

	/* Extract pixel intensity maps */
	IntMap::Uptr i1 = std::make_unique<IntMap>(rawIm1),
	             i2 = std::make_unique<IntMap>(rawIm2);

	/* Let us try and box the images into a pair
	 * In effect, a pair of memory addresses to the two images?? */
	IntMap::Pair imPair{i1, i2};

	/* Load options from config file
	 *  Clean this up so analysisOptions takes just a filename as argument */
	PivOptions::Uptr analysisOptions = std::make_unique<PivOptions>(
				ConfigFile::parse("../config/default.cfg")
			);	

	/* Create a grid */
	PivEng::Grid::Uptr g = std::make_unique<PivEng::Grid>(*analysisOptions, *i1);
	/* We have options, images and a grid, now do some PIV */
	PivEng::DoPiv p = PivEng::DoPiv(*analysisOptions, imPair, *g);
	
	/* if -o flag is supplied with a filename, write to it */
	if (clArgs.hasParam("-o")) p.write(clArgs.getParam("-o"));
	if (clArgs.printResults()) p.print();

	return 0;
}

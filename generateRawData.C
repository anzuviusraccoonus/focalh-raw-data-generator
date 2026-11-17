// ROOT Macro

#include "src/RawDataGenerator.cpp"

void generateRawData(int nEvents = -1, int nCRULinks = -1, int nGBTLinks = -1, 
                     int nPacketsPerEvent = -1, std::string outfile = "", int fee = -1) {

  GeneratorConfig config;

  if (not (outfile == "")) { config.outfile = outfile; }
  if (nEvents > 0) { config.numEvents = nEvents; }
  if (nCRULinks > 0) { config.numCRUCards = nCRULinks; }
  if (nGBTLinks > 0) { config.numGBTLinks = nGBTLinks; }
  if (nPacketsPerEvent > 0) { config.packetsPerEvent = nPacketsPerEvent; }
  if (fee > 0) { config.detectorFEE = fee; }

  DataGenerator generator(config);
  generator.run();
}

#ifndef FOCALH_RAW_DATA_GENERATOR_H
#define FOCALH_RAW_DATA_GENERATOR_H

#include <cstdint>
#include <vector>

#include "DetectorsRaw/RawFileWriter.h"

#include "Definitions.h"

struct GBTLinkDeviceConfig {
  uint8_t id = 0;
  int16_t preFrameIdles = 33;
  uint8_t samplesPerEvent = 16;
  uint8_t packetsPerFrame = 40;
};

struct GeneratorConfig {
  size_t packetsPerEvent = 8000;
  size_t numGBTLinks = 2;
  size_t numCRUCards = 4;
  size_t numEvents = 1000;
  uint16_t detectorFEE = 0xBEEF;
  std::string outfile = "gen.raw";
};

class GBTLinkDevice {
  public:
    GBTLinkDevice(size_t id) : mConfig(GBTLinkDeviceConfig{static_cast<uint8_t>(id)}) {};

    void generateNextLine(uint8_t* p);
    void generateIdlePayload(uint8_t* p);
    void generateDAQPayload(uint8_t* p);
    void prepareNewEvent(uint32_t orbit, uint16_t bx);

  private:
    GBTLinkDeviceConfig mConfig;

    uint32_t mOrbit = 0;
    uint16_t mBx = 0;

    int32_t mLineCounter = 0;
    uint32_t mEventCounter = 0;
    uint8_t mSampleCounter = 0;
};

class CRUGenerator {
  public:
    CRUGenerator(size_t cruID, size_t numLinks, size_t eventSize);

    void prepareNewEvent(uint32_t orbit, uint16_t bx);
    void generateEvent(std::vector<uint8_t>& buffer);

    int id() const { return mID; }

  private:
    std::vector<GBTLinkDevice> mGBTLinks;
    uint16_t mID;
    uint32_t mEventSize;

    uint32_t mOrbit = 0;
    uint16_t mBx = 0;
};

class DataGenerator {
  public:
    DataGenerator(const GeneratorConfig& cfg);

    void init();
    void run();

  private:
    o2::raw::RawFileWriter mWriter{o2::header::DataOrigin{"FOC"}};
    GeneratorConfig mConfig;
    std::vector<CRUGenerator> mCRULinks;
};

#endif

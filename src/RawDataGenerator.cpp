#include "../include/RawDataGenerator.h"

void GBTLinkDevice::generateNextLine(uint8_t* p) {
  
  // Construct the "header" (first two words of a packet)
  const uint64_t header = 
    (static_cast<uint64_t>(mOrbit)      << GBTPacket::OrbitCntrOffset *8) |
    (static_cast<uint64_t>(mBx++)       << GBTPacket::BxCntrOffset    *8) |
    (static_cast<uint64_t>(mConfig.id)  << GBTPacket::LinkIDOffset    *8) |
    (static_cast<uint64_t>(PayloadWords::GBTMarker));

  std::memcpy(p, &header, sizeof(header));

  if (mSampleCounter >= mConfig.samplesPerEvent || 
      mLineCounter < 0 || 
      mLineCounter == mConfig.packetsPerFrame) {

    generateIdlePayload(p + GBTPacket::DataOffset);

  } else {

    if ((mLineCounter >= 0) && (mLineCounter < mConfig.packetsPerFrame)) {
      generateDAQPayload(p + GBTPacket::DataOffset);
    }
  }
  
  if (mLineCounter++ == mConfig.packetsPerFrame) {
    mSampleCounter++;
    mLineCounter = 0;
  }
}

void GBTLinkDevice::generateIdlePayload(uint8_t* p) {
  std::memcpy(p, &IdlePayload, sizeof(IdlePayload));
}

void GBTLinkDevice::generateDAQPayload(uint8_t* p) {
  const uint32_t word = mLineCounter == 0 ? PayloadWords::DAQH : mLineCounter; // could inject actual data instead.
  const std::array<uint32_t, 6> payload = {word, word, word, word, word, word}; // bird is the word.
  std::memcpy(p, payload.data(), sizeof(payload));
}

void GBTLinkDevice::prepareNewEvent(uint32_t orbit, uint16_t bx) {
  mOrbit = orbit;
  mBx = bx;
  mLineCounter = -mConfig.preFrameIdles;
  mSampleCounter = 0;
  mEventCounter++;
}

CRUGenerator::CRUGenerator(size_t cruID, size_t numLinks, size_t eventSize) 
  : mID(cruID), mEventSize(eventSize) {
  
  mGBTLinks.reserve(numLinks);
  for (size_t i = 0; i < numLinks; ++i) {
    mGBTLinks.emplace_back(GBTLinkDevice{i});
  }
}

void CRUGenerator::prepareNewEvent(uint32_t orbit, uint16_t bx) {
  mOrbit = orbit;
  mBx = bx;
  for (GBTLinkDevice& dev : mGBTLinks) {
    dev.prepareNewEvent(orbit, bx);
  }
}

void CRUGenerator::generateEvent(std::vector<uint8_t>& buffer) {

  const std::array<uint32_t, 8> trigger = {
    PayloadWords::TrgMarker, 
    static_cast<uint32_t>(mBx), 
    0, 
    mOrbit,
    0, 
    0, 
    0, 
    static_cast<uint32_t>(mID) // Not present in actual data; for debugging purposes
  };

  std::memcpy(buffer.data(), trigger.data(), 32);
  
  size_t i = 1; // Start at 1 because index 0 is the trigger we just copied
  while (i < mEventSize) {
    for (GBTLinkDevice& dev : mGBTLinks) {
      if (i == mEventSize) { break; }
      dev.generateNextLine(buffer.data() + i*GBTPacket::Size);
      ++i;
    }
  }
}

DataGenerator::DataGenerator(const GeneratorConfig& cfg)
  : mConfig(cfg) { 
  
  init(); 
}

void DataGenerator::init() {
  for (size_t i = 0; i < mConfig.numCRUCards; ++i) {
    mCRULinks.push_back(CRUGenerator{i, mConfig.numGBTLinks, mConfig.packetsPerEvent});
    mWriter.registerLink(mConfig.detectorFEE, i, 0, 0, mConfig.outfile);
  }
}

void DataGenerator::run() {
  std::cout << "Generating " << mConfig.numEvents << " events "
            << "for " << mConfig.numCRUCards << " CRU links "
            << "with " << mConfig.numGBTLinks << " GBT links each "
            << "from FEE " << mConfig.detectorFEE << "; "
            << mConfig.packetsPerEvent << " packets (" 
            << mConfig.packetsPerEvent * GBTPacket::Size << " bytes) "
            << "of payload per CRU link per event" << std::endl;

  std::vector<uint8_t> out(mConfig.packetsPerEvent * GBTPacket::Size);
  for (size_t n = 0; n < mConfig.numEvents; ++n) {

    // Simple choices of orbit and bunch crossing counters for now
    uint32_t ob = n;
    uint16_t bx = n % 3000;

    for (CRUGenerator& cru : mCRULinks) {
      cru.prepareNewEvent(ob, bx);
      cru.generateEvent(out);
      mWriter.addData(mConfig.detectorFEE, cru.id(), 0, 0, {bx, ob}, 
                      gsl::span<char>(reinterpret_cast<char*>(out.data()), out.size()));
    }
  }

  mWriter.close();
}

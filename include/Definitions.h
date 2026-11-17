#ifndef FOCALH_RAW_DATA_GENERATOR_DEFINITIONS_H
#define FOCALH_RAW_DATA_GENERATOR_DEFINITIONS_H

#include <array>
#include <cstdint>

// All numbers are in units of bytes
namespace GBTPacket {
  constexpr size_t Size             = 32;
  constexpr size_t MarkerOffset     = 0;
  constexpr size_t LinkIDOffset     = 1;
  constexpr size_t BxCntrOffset     = 2;
  constexpr size_t OrbitCntrOffset  = 4;
  constexpr size_t DataOffset       = 8;
}

// Commonly used words
namespace PayloadWords {
  constexpr uint32_t Idle       = 0xACCCCCCC;
  constexpr uint32_t DAQH       = 0xF0000005;
  constexpr uint32_t TrgMarker  = 0xBBBBBBBB;
  constexpr uint8_t  GBTMarker  = 0xAC;
}

// One GBT packet of Idle data
static constexpr std::array<uint32_t, 6> IdlePayload = {
  PayloadWords::Idle,
  PayloadWords::Idle,
  PayloadWords::Idle,
  PayloadWords::Idle,
  PayloadWords::Idle,
  PayloadWords::Idle
};

#endif

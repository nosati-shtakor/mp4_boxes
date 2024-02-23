#pragma once

//
// Can we replace this with intrinsics?
//

uint32_t changeEndian(const uint32_t value)
{
  static_assert(sizeof(uint32_t) == 4);

  uint32_t result;
  const uint8_t* s = reinterpret_cast<const uint8_t*>(&value);
  uint8_t* d = reinterpret_cast<uint8_t*>(&result);

  d[0] = s[3];
  d[1] = s[2];
  d[2] = s[1];
  d[3] = s[0];

  return result;
}


uint64_t changeEndian(const uint64_t value)
{
  static_assert(sizeof(uint64_t) == 8);

  uint64_t result;
  const uint8_t* s = reinterpret_cast<const uint8_t*>(&value);
  uint8_t* d = reinterpret_cast<uint8_t*>(&result);

  d[0] = s[7];
  d[1] = s[6];
  d[2] = s[5];
  d[3] = s[4];
  d[4] = s[3];
  d[5] = s[2];
  d[6] = s[1];
  d[7] = s[0];

  return result;
}

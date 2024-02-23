#pragma once

static const int32_t B64index[256] =
{
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,  0,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63,
  0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};


void b64decode(const uint8_t* in, const uint32_t len, uint8_t** out, uint32_t& out_size)
{
  if(len == 0)
    return;

  const uint8_t* p(in);
  uint32_t j = 0;
  const uint32_t pad1 = len % 4 || p[len - 1] == '=';
  const uint32_t pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
  const uint32_t last = (len - pad1) / 4 << 2;
  out_size = last / 4 * 3 + pad1 + pad2;
  *out = new(std::nothrow)uint8_t[out_size];
  uint8_t* str(*out);
  
  for(uint32_t i = 0; i < last; i += 4)
  {
    const int32_t n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
    str[j++] = static_cast<uint8_t>(n >> 16);
    str[j++] = static_cast<uint8_t>(n >> 8);
    str[j++] = static_cast<uint8_t>(n);
  }
  if(pad1)
  {
    int32_t n = B64index[p[last]] << 18 | B64index[p[last + 1]] << 12;
    str[j++] = static_cast<uint8_t>(n >> 16);
    if (pad2)
    {
      n |= B64index[p[last + 2]] << 6;
      str[j++] = static_cast<uint8_t>(n >> 8);
    }
  }
}


bool validBase64Char(const char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z' ) || c == '+' || c == '/' || c == '=';
}
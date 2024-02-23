#pragma once
#include <stdint.h>
#include <fstream>


class Mp4Box
{
public:

  struct BOX
  {
    uint64_t size;  // little endian (we will convert this to LE after retrieving from a file)
    uint32_t type;  // big endian
  };
    
  static int32_t dumpBoxes(const char* file_name, const bool dump_png);

private:

  static int32_t readBox(std::ifstream& file, const bool dump_png);
  static int32_t readSizeType(std::ifstream& file, BOX& box);
  static void printBox(const BOX& box);
  static void printContent(const BOX& box, const std::string& content);
  static void getCurrentTimeString(char* buffer, const int32_t size);
  static int32_t readContent(std::ifstream& file, const BOX& box, std::string& xml);
  static int32_t dumpPng(const std::string& xml);
};
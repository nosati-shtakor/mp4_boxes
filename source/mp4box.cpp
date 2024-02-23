#include "../mp4box.h"
#include "../boxtypes.h"
#include "../utility.h"
#include "../xmlreader.h"
#include <ctime>


int32_t Mp4Box::dumpBoxes(const char* file_name, const bool dump_png)
{
  std::ifstream mp4_file(file_name, std::ios_base::in | std::ios_base::binary);
  if(mp4_file.fail())
    return -1;

  while(readBox(mp4_file, dump_png) == 0);

  return 0;
}


int32_t Mp4Box::readSizeType(std::ifstream& file, BOX& box)
{
#pragma pack(push, 1)
  struct SIZE_TYPE
  {
    uint32_t size;
    uint32_t type;
  }st;
#pragma pack(pop)

  file.read(reinterpret_cast<char*>(&st), 8);

  if(file.gcount() != 8)
    return -1;

  st.size = changeEndian(st.size);

  if(st.size == 0)
  {
    //
    // extends to the end of the file...
    //
    // N.B.: This is not tested!
    //
    box.type = st.type;
    const std::streampos old_pos(file.tellg());
    file.seekg(0, std::ios::end);
    box.size = file.tellg() - old_pos;
    file.seekg(old_pos, std::ios::beg);
  }
  else if(st.size == 1)
  {
    // largesize?
    box.type = st.type;
    file.read(reinterpret_cast<char*>(&box.size), 8);
    if(file.gcount() != 8)
      return -1;

    box.size = changeEndian(box.size) - 8;
  }
  else
  {
    box.size = st.size;
    box.type = st.type;
  }

  return 0;
}


int32_t Mp4Box::readBox(std::ifstream& file, const bool dump_png)
{
  BOX box;
  int32_t result(readSizeType(file, box));
  if(result == 0)
  {
    printBox(box);

    switch(box.type)
    {
      case moof:
      case moov:
      case traf:
      case trak:
        readBox(file, dump_png);
        break;
      case mdat:
      {
        std::string xml;
        if(readContent(file, box, xml) == 0)
        {
          printContent(box, xml);

          if(dump_png)
            dumpPng(xml);
        }
        break;
      }
      default:
        file.seekg(box.size - 8, std::ios_base::cur); // jump over payload
        break;
    }
  }

  return result;
}


void Mp4Box::getCurrentTimeString(char* buffer, const int32_t size)
{
  timespec ts;
  if(!timespec_get(&ts, TIME_UTC))
    ts.tv_nsec = 0;

  const std::time_t now(std::time(nullptr));
  const size_t bytes_written(std::strftime(buffer, size, "%Y-%m-%d %H:%M:%S", std::localtime(&now)));
  if(bytes_written)
  {
    snprintf(buffer + bytes_written, size - bytes_written, ":%ld", ts.tv_nsec / 1000000); // N.B. Kind of a dirty hack to make it portable. A better solution would be to implement getting time with milliseconds on each platform.
  }
}


void Mp4Box::printBox(const BOX& box)
{
  char time_buffer[32];
  getCurrentTimeString(time_buffer, sizeof(time_buffer));

  printf("%s Found box of type %c%c%c%c and size %lu\n", time_buffer, (char)(box.type & 0xFF), (char)(box.type >> 8), (char)(box.type >> 16), (char)(box.type >> 24), box.size);
}


void Mp4Box::printContent(const BOX& box, const std::string& content)
{
  char time_buffer[32];
  getCurrentTimeString(time_buffer, sizeof(time_buffer));

  printf("%s Content of %c%c%c%c box is: %s\n", time_buffer, (char)(box.type & 0xFF), (char)(box.type >> 8), (char)(box.type >> 16), (char)(box.type >> 24), content.c_str());
}


int32_t Mp4Box::readContent(std::ifstream& file, const BOX& box, std::string& xml)
{
  const int64_t bytes_to_read(box.size - 8);
  xml.resize(bytes_to_read + 1);
  
  file.read(xml.data(), bytes_to_read);
  if(file.gcount() != bytes_to_read)
  {
    printf("Error reading box '%c%c%c%c'\n", (char)(box.type & 0xFF), (char)(box.type >> 8), (char)(box.type >> 16), (char)(box.type >> 24));
    return -1;
  }

  return 0;
}


int32_t Mp4Box::dumpPng(const std::string& xml)
{
  XMLReader xml_reader;
  int32_t result(xml_reader.parsePayload(xml));
  
  if(result == 0)
  {
    const std::vector<XMLReader::XML_PAYLOAD>& payload(xml_reader.getPayload());

    for(const XMLReader::XML_PAYLOAD& xml_payload : payload)
    {
      std::string file_name(xml_payload.name);
      file_name.append(".").append(xml_payload.type);
      
      std::ofstream file(file_name.c_str(), std::ios_base::out | std::ios_base::binary);
      if(file.fail())
      {
        printf("Cannot create: '%s' file\n", file_name.c_str());
      }
      else
      {
        file.write(reinterpret_cast<const char*>(xml_payload.blob), xml_payload.blob_size);
        if(file.bad())
          printf("Writing '%s' failed!\n", file_name.c_str());
      }
    }
  }

  return result;
}

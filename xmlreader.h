#pragma once
#include <string>
#include <vector>


class XMLReader
{
public:

  XMLReader();
  XMLReader(const XMLReader&) = delete;
  XMLReader& operator=(const XMLReader&) = delete;
  ~XMLReader();


  struct XML_PAYLOAD
  {
    std::string name;
    std::string type;
    uint8_t* blob;
    uint32_t blob_size;
  };

  int32_t parsePayload(const std::string& xml);
  const std::vector<XML_PAYLOAD>& getPayload()const { return m_payload; }

private:
  std::vector<XML_PAYLOAD> m_payload;
};
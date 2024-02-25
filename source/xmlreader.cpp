#include "../xmlreader.h"
#include "../3rdparty/rapidxml/rapidxml.hpp"
#include "../base64.h"
#include <algorithm>


XMLReader::XMLReader()
{
  m_payload.reserve(3);
}


XMLReader::~XMLReader()
{
  for(const XML_PAYLOAD& xml_payload : m_payload)
    delete[] xml_payload.blob;
}


int32_t XMLReader::parsePayload(const std::string& xml)
{
  rapidxml::xml_document<> xml_doc;
  int32_t result(-1);

  do 
  {
    try
    {
      xml_doc.parse<rapidxml::parse_non_destructive>(const_cast<char*>(xml.c_str()));
    }
    catch(rapidxml::parse_error&)
    {
      printf("XML parser died. RIP\n");
      break;
    }
  
    rapidxml::xml_node<>* tt(xml_doc.first_node("tt"));
    if(!tt)
      break;
  
    rapidxml::xml_node<>* head(tt->first_node("head"));
    if(!head)
      break;

    rapidxml::xml_node<>* metadata(head->first_node("metadata"));
    if(!metadata)
      break;
    
    rapidxml::xml_node<>* smpte(metadata->first_node());
    if(!smpte)
      break;

    while(smpte)
    {
      XML_PAYLOAD xml_payload;
      rapidxml::xml_attribute<>* attribute(smpte->first_attribute());
      xml_payload.blob = nullptr;
      xml_payload.blob_size = 0;

      while(attribute)
      {
        const std::string_view name(attribute->name(), attribute->name() + attribute->name_size());     // need C++20 for this
        const std::string_view value(attribute->value(), attribute->value() + attribute->value_size());
        
        if(name.compare("xml:id") == 0)
          xml_payload.name = value;
        else if(name.compare("imagetype") == 0)
        {
          xml_payload.type = value;
          std::transform(xml_payload.type.begin(), xml_payload.type.end(), xml_payload.type.begin(), [](const char c) -> char{ return static_cast<char>(std::tolower(c)); });
        }
        else if(name.compare("encoding") == 0 && value.compare("Base64") == 0) // TO DO: We should ignore case when comparing
        {
          assert(xml_payload.blob == nullptr);
          const uint8_t* b64_blob(reinterpret_cast<const uint8_t*>(smpte->value()));
          uint32_t b64_blob_size(static_cast<uint32_t>(smpte->value_size()));
          
          while(!validBase64Char(*b64_blob))
          {
            //
            // For some reason base64 string begins with a LF. Intentional task bomb??
            // This is a dirty fix.
            //
            ++b64_blob;
            --b64_blob_size;
          }
          b64decode(b64_blob, b64_blob_size, &xml_payload.blob, xml_payload.blob_size);
        }

        attribute = attribute->next_attribute();
      }

      // const char* base64_payload(smpte->value());
      // const size_t base64_size(smpte->value_size());

      m_payload.push_back(std::move(xml_payload));

      smpte = smpte->next_sibling();
    }

    result = 0;

  }while(0);

  xml_doc.clear();

  return result;
}
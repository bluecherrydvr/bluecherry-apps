#pragma once

#include <pugixml.hpp>

struct xml_string_writer: pugi::xml_writer {
  std::string result;
  virtual void write(const void* data, size_t size) {
    result += std::string(static_cast<const char*>(data), size);
  }
};

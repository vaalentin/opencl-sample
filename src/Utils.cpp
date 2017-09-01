#include "Utils.hpp"

#include <sstream>

Utils::Utils() {

}

Utils::~Utils() {

}

std::string Utils::convertToString(double num) {
  std::ostringstream convert;
  convert << num; 
  return convert.str();
}

double Utils::roundOff(double n) {
  double d = n * 100.0;
  int i = d + 0.5;
  d = (float)i / 100.0;
  return d;
}

std::string Utils::getSizeString(size_t size) {
  static const char *SIZES[] = { "B", "KB", "MB", "GB" };
  int div = 0;
  size_t rem = 0;

  while (size >= 1024 && div < (int) (sizeof SIZES / sizeof *SIZES)) {
    rem = (size % 1024);
    div++;
    size /= 1024;
  }

  double size_d = (float)size + (float)rem / 1024.0;
  std::string result = Utils::convertToString(Utils::roundOff(size_d)) + " " + SIZES[div];
  return result;
}

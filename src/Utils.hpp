#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

class Utils {
  public:
    Utils();
    ~Utils();
    static std::string convertToString(double num);
    static double roundOff(double n);
    static std::string getSizeString(size_t size);
};

#endif

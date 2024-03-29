/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
* Filename:g2time.cpp cross-platform, thread-safe replacement for C++11 non-thread-safe
*                   localtime (and similar)
* Created: 2012 by Kjell Hedstr�m
*
* PUBLIC DOMAIN and Not under copywrite protection. First published for g2log at KjellKod.cc
* ********************************************* */
#include "stdafx.h"
#include "g2time.h"

#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>


namespace g2 { namespace internal {
  // This mimics the original "std::put_time(const std::tm* tmb, const charT* fmt)"
  // This is needed since latest version (at time of writing) of gcc4.7 does not implement this library function yet.
  // return value is SIMPLIFIED to only return a std::string
  std::string put_time(const struct tm* tmb, const char* c_time_format)
  {
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    std::ostringstream oss;
    oss.fill('0');
    oss << std::put_time(const_cast<struct tm*>(tmb), c_time_format); // BOGUS hack done for VS2012: C++11 non-conformant since it SHOULD take a "const struct tm*  "
    return oss.str();
#else    // LINUX
    const size_t size = 1024;
    char buffer[size]; // OBS: kolla om inte std::put_time finns. This is way more buffer space then we need
    auto success = std::strftime(buffer, size, c_time_format, tmb); // Ta �ver denna funktion till BitBucket/code/g2log sen d� denna �r utvecklingsbranchen
    if (0 == success)
      return c_time_format; // error return result indeterminate due to buffer overflow - should throw instead?
    return buffer; // implicit conversion to std::string
#endif
  }
} // internal
} // g2



namespace g2
{
std::time_t systemtime_now()
{
  system_time_point system_now = std::chrono::system_clock::now();
  return std::chrono::system_clock::to_time_t(system_now);
}


tm localtime(const std::time_t& time)
{
  struct tm tm_snapshot;
#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
  localtime_r(&time, &tm_snapshot); // POSIX
#else
  localtime_s(&tm_snapshot, &time); // windsows
#endif
  return tm_snapshot;
}

/// returns a std::string with content of time_t as localtime formatted by input format string
/// * format string must conform to std::put_time
/// This is similar to std::put_time(std::localtime(std::time_t*), time_format.c_str());
std::string localtime_formatted(const std::time_t& time_snapshot, const std::string& time_format)
{
  std::tm t = localtime(time_snapshot); // could be const, but cannot due to VS2012 is non conformant for C++11's std::put_time (see above)
  std::stringstream buffer;
  buffer << g2::internal::put_time(&t, time_format.c_str());  // format example: //"%Y/%m/%d %H:%M:%S");
  return buffer.str();
}
} // g2
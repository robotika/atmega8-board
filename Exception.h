/*!
* \file Exception.h
* \brief Exception handling
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#ifndef EXCEPTION_H
#define EXCEPTION_H

#ifndef UNDER_CE

#ifndef linux
#pragma warning(disable:4290)
#endif

#include <exception>
#include <string>
using namespace std;

class Exception : public std::exception
{
public:
#if defined (_WIN32)
  Exception(const char * const & i_message) : std::exception(i_message){}
#else //STL
  Exception(const char * const & i_message) : std::exception(){perror(i_message);}
#endif
};

// there is still some problem with __LINE__ which is a number (later could be extended)
#define EXCEPTION(X) do { throw Exception( (string(__FILE__).append(string(": ")).append(string(X))).c_str()); } while(0)

#endif // not UNDER_CE

#endif

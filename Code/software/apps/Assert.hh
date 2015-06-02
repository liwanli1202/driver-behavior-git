#ifndef ASSERT_HH_
#define ASSERT_HH_

#include <exception>
using namespace std;

class InvalidParameterException: public exception
{
  virtual const char* what() const throw()
  {
    return "Invalid parameters";
  }
};

template <typename X, typename A>
inline void Assert(A assertion)
{
    if( !assertion ) throw X();
}

#endif

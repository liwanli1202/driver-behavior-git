/*
 * Random
 *
 *  Original version created in 2009: Somchoke Sakjirapong
 *  Started refactoring on: Nov 27, 2014
 *                  Author: Jednipat
 *
 */

#ifndef RANDOM_
#define RANDOM_

#include <boost/random.hpp>

class Random
{
 public:
  boost::mt19937 gen;
  boost::uniform_int<int> dst;
  boost::variate_generator< boost::mt19937, boost::uniform_int<int> > rand;
  Random(int N)
      : gen(static_cast<unsigned long>(std::time(0))),
        dst(0, N),
        rand(gen, dst) {}
  std::ptrdiff_t operator()(std::ptrdiff_t arg) {
    return static_cast< std::ptrdiff_t >(rand());
  }
};



#endif /* RANDOM_ */

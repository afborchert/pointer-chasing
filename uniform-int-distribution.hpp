/* 
   Copyright (c) 2016 Andreas F. Borchert
   All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
   KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#ifndef UNIFORM_INT_DISTRIBUTION_HPP
#define UNIFORM_INT_DISTRIBUTION_HPP

#include <random>

/* simple class for a pseudo-random generator producing
   uniformely distributed integers */
class UniformIntDistribution {
   public:
      UniformIntDistribution() : engine(std::random_device()()) {}
      /* return number in the range of [0..upper_limit) */
      unsigned int draw(unsigned int upper_limit) {
	 return std::uniform_int_distribution<unsigned int>
	    (0, upper_limit-1)(engine);
      }
   private:
      std::mt19937 engine;
};

#endif

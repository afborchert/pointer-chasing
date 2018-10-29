/* 
   Copyright (c) 2016, 2018 Andreas F. Borchert
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

#ifndef WALLTIME_HPP
#define WALLTIME_HPP

#include <chrono>

template<typename T>
class WallTime {
   public:
      WallTime() : t0(std::chrono::high_resolution_clock::now()) {
      }
      /* return real time in seconds using T since construction
	 of this object */
      T elapsed() const {
	 using namespace std::chrono;
	 auto time_spent = high_resolution_clock::now() - t0;
	 return duration<T, seconds::period>(time_spent).count();
      }
   private:
      std::chrono::high_resolution_clock::time_point t0;
};

#endif

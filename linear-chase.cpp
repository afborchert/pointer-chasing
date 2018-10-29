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

/* utility that measures the effect of automatic hardware-driven
   prefetches for various stride values */

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <sys/times.h>
#include <printf.hpp> /* see https://github.com/afborchert/fmt */
#include "chase-pointers.hpp"
#include "linear-chain.hpp"

#ifndef MIN_STRIDE
#define MIN_STRIDE (sizeof(void*))
#endif
#ifndef MAX_STRIDE
#define MAX_STRIDE 1200
#endif

int main() {
   fmt::printf("   stride  time in ns\n");
   for (std::size_t stride = MIN_STRIDE; stride <= MAX_STRIDE;
	 stride += sizeof(void*)) {
      size_t memsize = std::min(std::size_t{1}<<26,
	 stride * 1024 * sizeof(void*));
      void** memory = create_linear_chain(memsize, stride);
      std::size_t count = std::size_t{1}<<30;
      double t = chase_pointers(memory, count);
      delete[] memory;
      double ns = t * 1000000000 / count;
      fmt::printf(" %8u  %10.5lf\n", stride, ns); std::cout.flush();
   }
}

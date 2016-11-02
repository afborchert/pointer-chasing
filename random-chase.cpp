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

/* utility to measure cache and memory read access times */

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <printf.hpp> /* see https://github.com/afborchert/fmt */
#include "chase-pointers.hpp"
#include "uniform-int-distribution.hpp"

/* create a cyclic pointer chain that covers all words
   in a memory section of the given size in a randomized order */
void** create_random_chain(std::size_t size) {
   std::size_t len = size / sizeof(void*);
   void** memory = new void*[len];

   UniformIntDistribution uniform;

   // shuffle indices
   size_t* indices = new std::size_t[len];
   for (std::size_t i = 0; i < len; ++i) {
      indices[i] = i;
   }
   for (std::size_t i = 0; i < len-1; ++i) {
      std::size_t j = i + uniform.draw(len - i);
      if (i != j) {
	 std::swap(indices[i], indices[j]);
      }
   }
   // fill memory with pointer references
   for (std::size_t i = 1; i < len; ++i) {
      memory[indices[i-1]] = (void*) &memory[indices[i]];
   }
   memory[indices[len-1]] = (void*) &memory[indices[0]];
   delete[] indices;
   return memory;
}

unsigned int log2(std::size_t val) {
   unsigned int count = 0;
   while (val >>= 1) {
      ++count;
   }
   return count;
}

#ifndef MIN_SIZE
#define MIN_SIZE 1024
#endif
#ifndef MAX_SIZE
#define MAX_SIZE 1024 * 1024 * 32
#endif
#ifndef GRANULARITY
#define GRANULARITY 1u
#endif

int main() {
   fmt::printf("  memsize  time in ns\n");
   for (std::size_t memsize = MIN_SIZE; memsize <= MAX_SIZE;
	 memsize += (1 << (std::max(GRANULARITY, log2(memsize))-GRANULARITY))) {
      void** memory = create_random_chain(memsize);
      std::size_t count = std::max(memsize * 16, (std::size_t) 1<<30);
      double t = chase_pointers(memory, count);
      delete[] memory;
      double ns = t * 1000000000 / count;
      fmt::printf(" %8u  %10.5lf\n", memsize, ns); std::cout.flush();
   }
}

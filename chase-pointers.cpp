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

/* follow a circular pointer chain a given number of times
   and return the real time used in seconds as double */

#include <printf.hpp>
#include "chase-pointers.hpp"
#include "walltime.hpp"

/* this variable must not be declared static */
volatile void* chase_pointers_global; // to defeat optimizations

/* follow a pointer chain the given number of times and
   return the measured time */
double chase_pointers(void** memory, std::size_t count) {
   double t0 = walltime();
   // chase the pointers count times
   void** p = (void**) memory;
   while (count-- > 0) {
      p = (void**) *p;
   }
   double t1 = walltime();
   chase_pointers_global = *p;
   return t1 - t0;
}

/* print pointer chain to std::cout (for debugging) */
void debug_chain(void** memory) {
   void** p = memory;
   std::size_t count = 0;
   fmt::printf("chain at %p:\n", memory);
   do {
      fmt::printf("[0x%p] 0x%p", p, *p); ++count;
      if (*p < p) fmt::printf(" (rewinding)");
      fmt::printf("\n");
      p = (void**) *p;
   } while (p != memory);
   fmt::printf("# of pointers in chain: %zu\n", count);
}

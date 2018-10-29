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

/* this utility is an extension of the linear-chase:
   where linear-chase just has one pointer chase using a constant stride,
   this utility chases n pointers in an interleaved pattern
   for n running from 1 to 8 */

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <sys/times.h>
#include <printf.hpp> /* see https://github.com/afborchert/fmt */
#include "linear-chain.hpp"
#include "walltime.hpp"

template<typename Body, typename Object>
inline void fused_action(Body body, Object& object) {
   body(object);
}
template<typename Body, typename Object, typename... Objects>
inline void fused_action(Body body, Object& object, Objects&... objects) {
   body(object);
   fused_action(body, objects...);
}

/* this variable must not be declared static */
volatile void* fused_linear_global; // to defeat optimizations

template<typename... Pointers>
double fused_chase(std::size_t count, Pointers&... ptrs) {
   WallTime<double> walltime;
   // chase the pointers count times
   while (count-- > 0) {
      fused_action([](void**& p) { p = (void**) *p; }, ptrs...);
   }
   auto elapsed = walltime.elapsed();
   // defeat the optimization that removes the chasing
   fused_action([](void**& p) { fused_linear_global = *p; }, ptrs...);
   return elapsed;
}

#ifndef MIN_STRIDE
#define MIN_STRIDE (sizeof(void*))
#endif
#ifndef MAX_STRIDE
#define MAX_STRIDE 120
#endif

int main() {
   void **p1, **p2, **p3, **p4, **p5, **p6, **p7, **p8;
   void **m1, **m2, **m3, **m4, **m5, **m6, **m7, **m8;
   fmt::printf("                                          "
      "data access speeds in GiB/s\n");
   fmt::printf("     fuse");
   for (int i = 1; i <= 8; ++i) fmt::printf("%12d", i);
   fmt::printf("\n    stride\n");
   for (std::size_t stride = MIN_STRIDE; stride <= MAX_STRIDE;
	 stride += sizeof(void*)) {
      size_t memsize = std::min(std::size_t{1}<<26,
	 stride * 1024 * sizeof(void*));
      std::size_t count = std::size_t{1}<<30;
      fmt::printf(" %8u", stride);

      auto print_result = [=](int fuse, double t) {
	 auto volume = static_cast<double>(sizeof(void*)) * count * fuse;
	 auto speed = volume / t / (1<<30); /* in GiB/s */
	 fmt::printf("  %10.5lf", speed); std::cout.flush();
      };

      fused_action([=](void**& p) {
	 p = create_linear_chain(memsize, stride);
      }, m1, m2, m3, m4, m5, m6, m7, m8);
      p1 = m1; p2 = m2; p3 = m3; p4 = m4;
      p5 = m5; p6 = m6; p7 = m7; p8 = m8;

      print_result(1, fused_chase(count, p1));
      print_result(2, fused_chase(count, p1, p2));
      print_result(3, fused_chase(count, p1, p2, p3));
      print_result(4, fused_chase(count, p1, p2, p3, p4));
      print_result(5, fused_chase(count, p1, p2, p3, p4, p5));
      print_result(6, fused_chase(count, p1, p2, p3, p4, p5, p6));
      print_result(7, fused_chase(count, p1, p2, p3, p4, p5, p6, p7));
      print_result(8, fused_chase(count, p1, p2, p3, p4, p5, p6, p7, p8));

      fused_action([](void**& p) {
	 delete[] p;
      }, m1, m2, m3, m4, m5, m6, m7, m8);

      fmt::printf("\n"); std::cout.flush();
   }
}

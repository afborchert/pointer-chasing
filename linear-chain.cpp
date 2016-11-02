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

#include "linear-chain.hpp"

/* return the index of the most significant bit which is 1
   where the lowest significant bit has index 0,
   i.e. return minimal n where 2^(n+1) > val
*/
static unsigned int log2(unsigned int val) {
   unsigned int count = 0;
   while (val >>= 1) {
      ++count;
   }
   return count;
}

/* reverse the given number of bits within val */
static unsigned int bit_reverse(unsigned int val, unsigned int bits) {
   unsigned int result = 0;
   while (bits > 0) {
      result = (result << 1) | (val & 1);
      val >>= 1;
      --bits;
   }
   return result;
}

/* generate a bit-reversal permutation; see https://oeis.org/A030109 */
static void gen_bit_reversal_permutation(unsigned int* seq,
      unsigned int bits, unsigned int count) {
   /* generate a bit-reversal permutation for integers from 0 to (2^bits)-1 */
   unsigned int maxval = 1<<bits;
   for (unsigned int val = 0; val < maxval; ++val) {
      seq[val] = bit_reverse(val, bits);
   }
   /* cut sequence short if count is not a power of 2, i.e. count < 2^bits */
   unsigned int current = maxval;
   unsigned int index = 0;
   while (current > count) {
      while (seq[index] < count) ++index;
      --current; seq[index] = seq[current];
   }
}

/* create a cyclic pointer chain where the individual locations
   are stride bytes apart */
void** create_linear_chain(std::size_t size, std::size_t stride) {
   std::size_t len = size / sizeof(void*);
   void** memory = new void*[len];

   /* if we have multiple runs through the same buffer
      make sure that we operate with offsets where it appears
      more likely that the associated lines are not yet in
      one of the caches;
      to achieve this, we operate with bit reversal permutations,
      if runs == 8 we would get following sequence

	 0 4 2 6 1 5 3 7
   */
   unsigned int runs = stride / sizeof(void*);
   unsigned int bits = log2(runs);
   if ((1<<bits) != runs) ++bits;
   unsigned int* offset = new unsigned int[1<<bits];
   gen_bit_reversal_permutation(offset, bits, runs);

   /* generate the actual pointer chain */
   unsigned int run = 0;
   void** last = nullptr;
   for (unsigned int run = 0; run < runs; ++run) {
      char* next = (char*) memory + offset[run] * sizeof(void*);
      if (last) {
	 *last = (void*) next;
      }
      last = (void**) next;
      for(;;) {
	 char* next = (char*) last + stride;
	 if (next >= (char*) memory + size) break;
	 *last = (void*) next; last = (void**) next;
      }
   }
   *last = (void*) memory; /* close the cycle */
   return memory;
}

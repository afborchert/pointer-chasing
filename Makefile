CPPSources := $(wildcard *.cpp)
Objects := $(patsubst %.cpp,%.o,$(CPPSources))

fused-linear-chase-objects := fused-linear-chase.o \
		chase-pointers.o linear-chain.o
linear-chase-objects := linear-chase.o \
		chase-pointers.o linear-chain.o
random-chase-objects := random-chase.o \
		chase-pointers.o

CC := g++
CPPFLAGS := -std=gnu++11 -Ifmt
CXXFLAGS := $(CPPFLAGS) -g -O2
Targets := fused-linear-chase linear-chase random-chase

.PHONY:		all clean realclean depend
all:		$(Objects) $(Targets)
clean:		; rm -f $(Objects)
realclean:	clean
		rm -f $(Targets)

depend:		$(CPPSources)
		perl gcc-makedepend/gcc-makedepend.pl $(CPPFLAGS) $(CPPSources)

fused-linear-chase:	$(fused-linear-chase-objects)
		$(CXX) $(LDFLAGS) -o $@ $(fused-linear-chase-objects)
linear-chase:	$(linear-chase-objects)
		$(CXX) $(LDFLAGS) -o $@ $(linear-chase-objects)
random-chase:	$(random-chase-objects)
		$(CXX) $(LDFLAGS) -o $@ $(random-chase-objects)

# DO NOT DELETE
random-chase.o: random-chase.cpp fmt/printf.hpp chase-pointers.hpp \
 uniform-int-distribution.hpp
chase-pointers.o: chase-pointers.cpp fmt/printf.hpp chase-pointers.hpp \
 walltime.hpp
fused-linear-chase.o: fused-linear-chase.cpp fmt/printf.hpp \
 linear-chain.hpp walltime.hpp
linear-chase.o: linear-chase.cpp fmt/printf.hpp chase-pointers.hpp \
 linear-chain.hpp
linear-chain.o: linear-chain.cpp linear-chain.hpp

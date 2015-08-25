TARGET := $(shell basename `pwd`)

CXX := c++

UNAME = $(shell uname)
ifeq (${UNAME},Darwin)
    CXXFLAGS += -mmacosx-version-min=10.7 -std=c++11 -stdlib=libc++
    LDFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++
    LDLIBS += -framework OpenGL
endif


SOURCES := $(wildcard [^_]*.cc)

COMPILE.cc = ${CXX} ${CXXFLAGS} ${CPPFLAGS} -c
LINK.o = ${CXX} ${LDFLAGS}
.DEFAULT_GOAL := ${TARGET}
    
%.o: %.cc
	${COMPILE.cc} -o $@ $<

${TARGET}: ${SOURCES:.cc=.o} 
	$(LINK.o) $^  $(LDLIBS) -o $@ 

clean:
	${RM} ${TARGET} *.o Makefile.dep

.PHONY: clean

##
## PIN tools
##

##############################################################
#
# Here are some things you might want to configure
#
##############################################################

#TARGET_COMPILER?=ms
TARGET_COMPILER?=gnu

##############################################################
#
# include *.config files
#
##############################################################

ifeq ($(TARGET_COMPILER),gnu)
    include ../makefile.gnu.config
    LINK ?=${CXX}
    CXXFLAGS ?= -I$(PIN_HOME)/InstLib -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT) -MMD
endif

ifeq ($(TARGET_COMPILER),ms)
    include ../makefile.ms.config
    DBG?=
endif

##############################################################
#
# Tools sets
#
##############################################################


EXTRA_LIBS =

OBJS = CallStack.o syscall_names.o Maid.o argv_readparam.o

ifeq ($(TARGET_OS),m)
    # syscall names on Mac yet to be fixed
    TOOL_ROOTS =
else 
    ifeq ($(TARGET_OS),b)
        TOOL_ROOTS =
    else 
        ifeq ($(TARGET),ia32)
            # can't handle 64 bit syscalls
            TOOL_ROOTS = Maid
        endif
    endif
endif

##############################################################
#
# build rules
#
##############################################################

all: tools
TOOLS = $(TOOL_ROOTS:%=%$(PINTOOL_SUFFIX))

tools: $(TOOLS)
test: $(TOOLS:%=%.test)

testdir:
	+make -C tests


syscall_names.o: syscall_names.C syscall_names.H

argv_readparam.o: argv_readparam.C argv_readparam.h

## build rules

%.o : %.C
	$(CXX) -c $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

Maid$(PINTOOL_SUFFIX) : $(OBJS) $(PIN_LIBNAMES)
	${LINK} $(PIN_LDFLAGS) $(LINK_DEBUG) $(OBJS) ${LINK_OUT}$@ ${PIN_LPATHS} $(PIN_LIBS) $(EXTRA_LIBS) $(DBG)
## cleaning
clean:
	-rm -f *.o $(TOOLS) *.out *.tested *.failed *.d

testmaid: testdir maid
	pin -t ./maid --addrfile=addrfile.txt -- tests/helloworld

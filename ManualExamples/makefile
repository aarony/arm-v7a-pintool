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
    LINK?=${CXX}
    CXXFLAGS ?= -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT)
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


TOOL_ROOTS =
ifneq ($(TARGET_OS),w)
  # pinatrace and itrace currently hang windows
  TOOL_ROOTS += pinatrace  itrace
endif
TOOL_ROOTS += inscount0 inscount1 inscount2 proccount imageload staticcount detach

ifeq (${TARGET_OS},l)
TOOL_ROOTS += strace
endif

ifeq (${TARGET_OS},m)
TOOL_ROOTS += strace
endif

ifeq ($(TARGET),ia32)
TOOL_ROOTS += isampling
endif

ifeq ($(TARGET),ia32e)
TOOL_ROOTS += isampling
endif

TOOLS = $(TOOL_ROOTS:%=%$(PINTOOL_SUFFIX))

##############################################################
#
# build rules
#
##############################################################

all: tools
tools: $(TOOLS)
test: $(TOOL_ROOTS:%=%.test)


## build rules

%.o : %.C
	$(CXX) -c $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<
$(TOOLS): $(PIN_LIBNAMES)
$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	${LINK} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG)

## cleaning
clean:
	-rm -f *.o $(TOOLS) *.out *.tested *.failed *.d *.makefile.copy

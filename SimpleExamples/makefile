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
    CXXFLAGS ?= -I$(PIN_HOME)/InstLib -fomit-frame-pointer -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT) -MMD
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

TOOL_ROOTS = dcache edgcnt pinatrace trace icount inscount2_mt opcodemix malloctrace calltrace \
	     jumpmix toprtn topopcode catmix regmix ilenmix

ifeq (${TARGET_OS},l)
TOOL_ROOTS += fence
endif

ifeq (${TARGET_OS},m)
TOOL_ROOTS += fence
endif

ifeq ($(TARGET),ia32)
TOOL_ROOTS += emuload extmix xed-print
endif

SANITY_TOOLS = opcodemix$(PINTOOL_SUFFIX)
# leave out fence, see comment at top of fence.C

TEST_TOOLS_ROOTS = dcache edgcnt pinatrace trace icount opcodemix  \
		malloctrace calltrace \
	        jumpmix toprtn topopcode catmix regmix
TEST_TOOLS = $(TEST_TOOLS_ROOTS:%=%$(PINTOOL_SUFFIX))

TOOLS = $(TOOL_ROOTS:%=%$(PINTOOL_SUFFIX))

all: tools
tools: $(TOOLS)
test: $(TEST_TOOLS:%=%.test)
tests-sanity: $(SANITY_TOOLS:%=%.test)

## build rules

%.o : %.C
	${CXX} ${COPT} $(CXXFLAGS) ${PIN_CXXFLAGS} ${OUTOPT}$@ $< 

$(TOOLS): $(PIN_LIBNAMES)
$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	${LINK} ${PIN_LDFLAGS} $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} ${PIN_LIBS} $(DBG)

## cleaning
clean:
	-rm -f *.o $(TOOLS) *.out *.tested *.failed *.d *makefile.copy

-include *.d


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

TOOL_ROOTS = icount filter
CONTEXT_TOOLS = alarm_icount alarm_symbol control control_detach alarm_image_offset
SANITY_TOOLS = icount

TOOL_ROOTS += $(CONTEXT_TOOLS)

TOOLS = $(TOOL_ROOTS:%=%$(PINTOOL_SUFFIX))


##############################################################
#
# build rules
#
##############################################################

all: tools

tools: $(TOOLS)
test: $(TOOL_ROOTS:%=%.test)
tests-sanity: $(SANITY_TOOLS:%=%.test)

## build rules

%.o : %.C
	$(CXX) -c $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<
$(TOOLS): $(PIN_LIBNAMES)

$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	${LINK} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(EXTRA_LIBS) $(DBG)

## cleaning
clean:
	-rm -f *.o $(TOOLS) *.out *.tested *.failed *.d

-include *.d

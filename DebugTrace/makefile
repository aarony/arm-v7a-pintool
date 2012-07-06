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
# build rules
#
##############################################################

EXTRA_LIBS =

TOOL_ROOTS = debugtrace

all: tools
SANITY_TOOLS = 

TOOLS = $(TOOL_ROOTS:%=%$(PINTOOL_SUFFIX))

tools: $(TOOLS)
test: $(TOOLS:%=%.test)
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

%.test : % %.tested %.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -silent -instruction -memory -- $(TESTAPP) makefile $<.makefile.copy
	$(PIN_CMP) makefile $<.makefile.copy
	rm $<.makefile.copy; rm $<.failed

-include *.d

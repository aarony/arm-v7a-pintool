##
## PIN tools
##


include ../makefile.gnu.config


DBG=-g
OPT=-O2
CXXFLAGS = -I$(PIN_HOME)/InstLib -fomit-frame-pointer -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT) -MMD
 
TOOL_ROOTS = insmix

TOOLS = $(TOOL_ROOTS:%=%$(PINTOOL_SUFFIX))

all: tools
tools: $(TOOLS)

## build rules

%.o : %.C
	$(CXX) -c $(CXXFLAGS) $(PIN_CXXFLAGS) -o $@ $<

$(TOOLS): $(PIN_LIBNAMES)
$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS) $(DBG)

## cleaning
clean:
	-rm -f *.o $(TOOLS) *.out *.tested *.failed *.d

-include *.d


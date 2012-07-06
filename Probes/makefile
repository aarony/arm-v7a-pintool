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
    CXXFLAGS ?= -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT) -MMD
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


all: tools
SANITY_TOOLS = 

TOOLS =

# rc added this test, but not ready for general testing
#TOOL_ROOTS = replaceebx

ifeq ($(TARGET_OS),l)

ifeq ($(TARGET),ia32)
# malloctrace2 does not work on on systems where libdl.so is not built with symbols
TOOL_ROOTS += malloctrace atraceprobe opin1 probemalloctrace rtnreplacesignature
endif

ifeq ($(TARGET),ia32e)
TOOL_ROOTS += opin1 
endif

ifeq ($(TARGET),ipf)
# malloctrace2 does not work on on systems where libdl.so is not built with symbols
TOOL_ROOTS += malloctrace probemalloctrace
endif

TOOLS = $(TOOL_ROOTS:%=%$(PINTOOL_SUFFIX))
DLTEST=dltest
MALLOCWRAPPERS=mallocwrappers.so
endif

ifeq ($(TARGET_OS),m)
TOOL_ROOTS = malloctrace malloctrace2 probemalloctrace

TOOLS = $(TOOL_ROOTS:%=%$(PINTOOL_SUFFIX))
DLTEST=dltest-mac
MALLOCWRAPPERS=mallocwrappers-mac.so
endif


tools: $(TOOLS)
test: $(TOOLS:%=%.test)
tests-sanity: $(SANITY_TOOLS:%=%.test)

# BSD has the dynamic loader library baked in
ifeq ($(TARGET_OS),b)
  DL_LIB =
else
  DL_LIB = -ldl
endif

traceapp: traceapp.c
	$(CC) $(PIN_LPATHS) -I../Include -I. -o traceapp traceapp.c -lpinapp -g

mttraceapp: mttraceapp.c
	$(CC) $(PIN_LPATHS) -I../Include -I. -o mttraceapp mttraceapp.c -lpinapp -lpthread -g

rtnreplacesignature.test : rtnreplacesignature$(PINTOOL_SUFFIX) rtnreplacesignature.tested rtnreplacesignature.failed traceapp
	$(PIN) -t $< -- ./traceapp
	rm rtnreplacesignature.failed

atraceprobe.test : atraceprobe$(PINTOOL_SUFFIX) atraceprobe.tested atraceprobe.failed traceapp
	$(PIN) -t $< -- ./traceapp
	rm atraceprobe.failed

mtatraceprobe.test : atraceprobe$(PINTOOL_SUFFIX) mtatraceprobe.tested mtatraceprobe.failed mttraceapp
	$(PIN) -mt -t $< -- ./mttraceapp
	rm mtatraceprobe.failed

malloctrace2.test : malloctrace2$(PINTOOL_SUFFIX) malloctrace2.tested malloctrace2.failed $(DLTEST) $(MALLOCWRAPPERS)
	$(PIN) -t $< -- ./$(DLTEST)
	rm malloctrace2.failed

mallocwrappers.so: mallocwrappers.c
	$(CC) -o mallocwrappers.so -fpic -shared mallocwrappers.c -g

mallocwrappers-mac.so: mallocwrappers.c
	$(CC) -o mallocwrappers-mac.so -fPIC -dynamiclib mallocwrappers.c -g

dltest: dltest.c one.c two.c 
	$(CC) -o two.so -fpic -shared two.c -g
	$(CC) -o one.so -fpic -shared one.c -g
	$(CC) -o dltest $(APP_CXXFLAGS) dltest.c $(DL_LIB) -Wl,-rpath,`pwd` -g

dltest-mac: dltest.c one.c two.c
	$(CC) -o two.so -fPIC -dynamiclib two.c -g
	$(CC) -o one.so -fPIC -dynamiclib one.c -g
	$(CC) -o dltest-mac $(APP_CXXFLAGS) dltest.c $(DL_LIB) -g

ebxhandle: ebxhandle.c ebx.s
	$(CC) -g -o ebxhandle ebxhandle.c ebx.s

replaceebx.test : replaceebx$(PINTOOL_SUFFIX) replaceebx.tested replaceebx.failed ebxhandle
	$(PIN) -mt -t $< -- ./ebxhandle > $<.out 2>&1
	cmp $<.out $<.reference
	rm replaceebx.failed

# opin2 is not a general purpose tool, and it should never be
# added to sanity or nightly testing.
opin2.test : opin2 opin2.tested opin2.failed
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	rm opin2.failed $<.out $<.makefile.copy

opin2: opin2.C
	$(CXX) -c $(CXXFLAGS) $(PIN_CXXFLAGS) -o $@.o $@.C
	${LINK} $(PIN_LDFLAGS)  $(LINK_DEBUG) -o $@ $@.o ${PIN_LPATHS} $(PIN_LIBS) $(EXTRA_LIBS) $(DBG)

## build rules

%.o : %.C
	$(CXX) -c $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<
$(TOOLS): $(PIN_LIBNAMES)
$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	${LINK} $(PIN_LDFLAGS)  $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(EXTRA_LIBS) $(DBG)

## cleaning
clean:
	-rm -f dltest dltest-mac *.so *.o $(TOOLS) *.out *.tested *.failed *.d 
	-rm -f traceapp mttraceapp ebxhandle opin2


-include *.d

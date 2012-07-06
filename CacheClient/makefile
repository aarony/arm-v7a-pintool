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
    LPTHREAD?=-lpthread
endif

ifeq ($(TARGET_COMPILER),ms)
    include ../makefile.ms.config
    DBG?=
    LPTHREAD?=
endif


## remove cache_flusher until bug is fixed
##SPECIALRUN = cache_flusher cache_doubler bb_test 

SPECIALRUN = cache_doubler bb_test 
NORMALRUN = cache_simulator watch_fragmentation trace_insertions enter_exit link_unlink event_trace cache_stats insertDelete deleteTrace orig_address
TOOL_ROOTS = $(SPECIALRUN) $(NORMALRUN)

TOOLS = $(TOOL_ROOTS:%=%$(PINTOOL_SUFFIX))

SANITY_TOOLS = 

ifeq ($(TARGET_OS),w)
#Thread wait depends on pthreads, so it doesn't work on Windows
THREADTESTAPP =
BIGTESTAPP = ./bigBinary
else 
THREADTESTAPP = ./threadWait
BIGTESTAPP = ./bigBinary
endif

#OS Dependent
#ifeq ($(TARGET_OS),l)
#SPECIALRUN += mtFlush
#endif

#Arch Dependent
ifeq ($(TARGET),ipf)
CACHELIMIT = 262144
else 
CACHELIMIT = 131072
endif

all: tools 
tools: $(TOOLS) 
test: $(TOOL_ROOTS:%=%.test)
tests-sanity: $(SANITY_TOOLS:%=%.test)

## special testing rules

cache_flusher.test: cache_flusher$(PINTOOL_SUFFIX) cache_flusher.tested cache_flusher.failed $(BIGTESTAPP)
	$(PIN) -limit_code_cache $(CACHELIMIT) -t $< -o $<.out -- $(BIGTESTAPP)
	grep -q SWOOSH $<.out
	grep -q eof $<.out
	rm cache_flusher.failed; rm $<.out

cache_doubler.test: cache_doubler$(PINTOOL_SUFFIX) cache_doubler.tested cache_doubler.failed $(BIGTESTAPP)
	$(PIN) -limit_code_cache $(CACHELIMIT) -t $< -o $<.out -- $(BIGTESTAPP) 
	grep -q STRETCH $<.out
	grep -q eof $<.out
	rm cache_doubler.failed; rm $<.out

flushAndInsert.test: flushAndInsert$(PINTOOL_SUFFIX) flushAndInsert.tested flushAndInsert.failed $(BIGTESTAPP)
	$(PIN) -limit_code_cache $(CACHELIMIT) -t $< -o $<.out -- $(BIGTESTAPP) 
	grep -q SWOOSH $<.out
	grep -q insertion $<.out
	rm flushAndInsert.failed; rm $<.out

bb_test.test: bb_test$(PINTOOL_SUFFIX) bb_test.tested bb_test.failed $(BIGTESTAPP)
	$(PIN) -t $< -b 10 -o $<.out -- $(BIGTESTAPP)
	grep -q "Final trace count" $<.out
	grep -q -v "Final trace count: 0" $<.out
	grep -q -v "Final cache size: 0" $<.out
	rm bb_test.failed; rm $<.out

mtFlush.test: mtFlush$(PINTOOL_SUFFIX) mtFlush.tested mtFlush.failed $(THREADTESTAPP)
	$(PIN) -mt -t $< -o $<.out -- $(THREADTESTAPP)
	grep -q SWOOSH $<.out
	rm mtFlush.failed; rm $<.out

orig_address.test:orig_address$(PINTOOL_SUFFIX) orig_address.tested orig_address.failed $(BIGTESTAPP)
	$(PIN) -t $< -o $<.out -- $(BIGTESTAPP) 
	grep -q "Success" $<.out
	grep -q -v "Failure" $<.out
	grep -q "eof" $<.out
	rm orig_address.failed; rm $<.out

threadWait: threadWait.c
	$(CC) -o threadWait threadWait.c -lpthread

bigBinary: bigBinary.c
	$(CC) -o bigBinary bigBinary.c

## build rules

%.o : %.C $(BIGTESTAPP) $(THREADTESTAPP)
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<
$(TOOLS): $(PIN_LIBNAMES)
$(TOOLS): %$(PINTOOL_SUFFIX) : %.o utils.o
	${LINK} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) utils.o $(DBG)

## cleaning
clean:
	-rm -f *.o $(TOOLS) $(BIGTESTAPP) $(THREADTESTAPP) *.out *.tested *.failed *.d 

tidy:
	-rm -f *.o *.tested *.failed *.d

-include *.d

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
    OPT?=-O2 -fomit-frame-pointer
    LINK?=${CXX}
    CXXFLAGS ?= -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT) -MMD
endif

ifeq ($(TARGET_COMPILER),ms)
    include ../makefile.ms.config
    DBG?=
endif


#uninit-new
TOOL_ROOTS = detach removeinstrumentation segv probe after \
        hello icount1 icount2 icount3 icount4 icount5 icountcheck \
        sequence sequencecheck reg brtaken verify bblnull \
        cflowcheck imageLoad callargs traceusage \
        mtest malloc new statica \
        tracecount smc_check 

# leave out probe because it causes an early exit
TEST_TOOLS = after hello icount1 icount2 icount3 icount4 icount5 \
    icountcheck sequence sequencecheck reg brtaken verify bblnull \
    cflowcheck callargs malloc new removeinstrumentation



ifeq ($(TARGET),arm)
TOOL_ROOTS += memaddrcheck_arm retreg_arm
TEST_TOOLS += memaddrcheck_arm retreg_arm
endif

ifeq ($(TARGET),ia32)
TOOL_ROOTS += snprintf alah emu checkaddress operand
TEST_TOOLS += snprintf alah emu checkaddress operand
endif

ifeq ($(TARGET),ia32e)
TOOL_ROOTS += snprintf
TEST_TOOLS += snprintf
endif

ifeq ($(TARGET_OS),w)
  TEST_APPS  = cp-win
else
  TEST_APPS = 
endif

TOOLS = $(TOOL_ROOTS:%=%$(PINTOOL_SUFFIX))

all:  tools  
tools: $(TOOLS) foobar 
test-apps: $(TEST_APPS)
tests: $(TEST_TOOLS:%=%.test)

## sanity

SANITY_TOOLS =  icount2 verify after brtaken sequence 
ifneq ($(TARGET_OS),w)
   #FIXME: image api
   SANITY_TOOLS += callargs
endif
tools-sanity: $(SANITY_TOOLS)
tests-sanity: $(SANITY_TOOLS:%=%.test)
test: $(TEST_TOOLS:%=%.test)

ifeq ($(TARGET),arm)
malloc.test: malloc malloc.tested malloc.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -m 0x60000 -- /bin/cp makefile $<.makefile.copy
	$(PIN_CMP) makefile $<.makefile.copy
	rm $<.makefile.copy; rm $<.failed
endif

callargs.test: callargs${PINTOOL_SUFFIX} callargs.tested callargs.failed foobar
	$(PIN) -t $< -- ./foobar
	rm callargs.failed

foobar: foobar.c
	$(CC) $(PIN_CXXFLAGS) $(DBG) ${OUTOPT}foobar foobar.c -O0

cp-win: cp-win.cpp
	${CXX} -mno-cygwin -o $@ $<

## build rules

%.o : %.C
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<
$(TOOLS): $(PIN_LIBNAMES)


$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	${LINK} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG)

## cleaning
clean:
	-rm -f *.o $(TOOLS) *.out *.tested *.failed *.d foobar $(TEST_APPS) *.exe

-include *.d

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
endif

ifeq ($(TARGET_COMPILER),ms)
    include ../makefile.ms.config
    DBG?=
endif


## Set up tool roots

TOOL_ROOTS = args line syscall
# don't test these on windows.
ifneq ($(TARGET_OS),w)
   TOOL_ROOTS += soload mmap
endif

IA32_TOOLS = forkcallback returnreg returnflags mt malmalloc partialinline rollback sse-ref malloctracereplace

ifeq ($(TARGET),ia32)
TOOL_ROOTS += $(IA32_TOOLS) swizzle2 swizzle3 swizzle4 swizzle5 funreplace funreplace_mt appcall inlinecall 
 ifneq ($(TARGET_OS),w)
   TOOL_ROOTS += context
 endif
endif
ifeq ($(TARGET),ia32e)
TOOL_ROOTS += $(IA32_TOOLS) swizzle2 appcall
endif
ifeq ($(TARGET),ipf)
TOOL_ROOTS += mt partialinline appcall
endif

# every tool should be tested
TEST_TOOLS = $(TOOL_ROOTS)

#
# tests that have no tools
#

# Generic
TEST_TOOLS += resize

# ISA dependent
IA32_TEST_TOOLS = mtstatic kthread htab
ifeq ($(TARGET),ia32)
TEST_TOOLS += $(IA32_TEST_TOOLS)
endif
ifeq ($(TARGET),ia32e)
TEST_TOOLS += $(IA32_TEST_TOOLS) spalign
endif
ifeq ($(TARGET),ipf)
# Add 'attachnat' when kernel is fixed
TEST_TOOLS += kthread htab smc_ipf
endif

# OS dependent
# when probes is supported on windows, dynmalloctrace can be moved up.
ifeq ($(TARGET_OS),l)
#TEST_TOOLS += sigmask sigmaskstatic
TEST_TOOLS += pipe
ifeq ($(TARGET),ia32)
TOOL_ROOTS += strace_ia32 dynmalloctrace
endif
ifeq ($(TARGET),ia32e)
TOOL_ROOTS += strace_ia32 dynmalloctrace
endif
ifeq ($(TARGET),ipf)
TOOL_ROOTS += strace_ipf dynmalloctrace
endif
ifeq ($(TARGET),arm)
TOOL_ROOTS += strace_arm
endif
endif

ifeq ($(TARGET_OS),m)
#exclude rollback, mtstatic, sigmaskstatic
TOOL_ROOTS = funreplace funreplace_mt swizzle2 swizzle3 swizzle4 mt forkcallback malmalloc args line returnreg returnflags partialinline  sse-ref syscall dynmalloctrace
ifeq ($(TARGET),ia32)
TOOL_ROOTS += strace_ia32
endif
TEST_TOOLS = sigmask-mac resize kthread htab $(TOOL_ROOTS)
endif

ifeq ($(TARGET_OS),m)
HELLO=hello-mac
DLTEST=dltest-mac
else
HELLO=hello
DLTEST=dltest
endif

ONEPROG=oneprog

# if you want to build, but not test a tool, add it to TOOL_ROOTS here

TOOLS = $(TOOL_ROOTS:%=%$(PINTOOL_SUFFIX))
APPS  = $(DLTEST) $(HELLO) mmapapp pipeapp
ifeq ($(TARGET),ipf)
APPS += smcapp_ipf attachnat
endif

ifeq ($(TARGET_OS),w)
all: tools 
else
all: tools $(APPS)
endif
tools: $(TOOLS)

## sanity

SANITY_TESTS = args.test 
# sse-ref uses XMM registers only available on IA32/EM64T
ifeq ($(TARGET),ia32e)
   SANITY_TESTS += sse-ref.test
endif
ifeq ($(TARGET),ia32)
   SANITY_TESTS += sse-ref.test
endif
ifneq ($(TARGET_OS),w)
   #NOTE: image api only handles MS-compiled binaries
   SANITY_TESTS += line.test 
endif
tests-sanity: $(SANITY_TESTS)
# uncomment when mt working on ipf as2.1
#tests-sanity: htab.test mt.test
test: $(TEST_TOOLS:%=%.test)

# file rename for hello.b is to avoid the copyright notice from changing line numbers
# gdwarf-2 is the default on newer compilers
line.test : line$(PINTOOL_SUFFIX) line.tested line.failed $(HELLO)
	rm -f pin.log
	$(PIN) -separate_memory 0 -t $< -- ./$(HELLO)
	$(PIN_CMP) line.output line.reference
	# Is pin writing a log file?
	[ ! -f pin.log ]
	rm line.failed

hello: hello.b
	cp hello.b hello.c
	$(CC) -gdwarf-2 -g -O0 -o hello hello.c -static

hello-mac: hello.b
	cp hello.b hello.c
	$(CC) -g -O0 -o hello-mac hello.c

htab.test: htab htab.tested htab.failed
	$(PIN) -mt -- ./htab
	rm htab.failed

htab: htab.c
	$(CC) -o htab htab.c -lpthread

threadexit.test: threadexit threadexit.tested threadexit.failed
	$(PIN) -mt -- ./threadexit >  $<.out 2>&1
	$(PIN_CMP) $<.out threadexit.reference
	rm threadexit.failed $<.out

threadexit: threadexit.c
	$(CC) -o threadexit threadexit.c -lpthread

malmalloc.test : malmalloc$(PINTOOL_SUFFIX) thread malmalloc.tested malmalloc.failed
	$(PIN) -mt -t $< -- ./thread >  $<.out 2>&1
	$(PIN_CMP) $<.out malmalloc.reference
	rm malmalloc.failed $<.out

mt.test : mt$(PINTOOL_SUFFIX) thread mt.tested mt.failed
	$(PIN) -mt -t $< -- ./thread
	sort mt.out | grep -v 5 > mt.out.sort
	$(PIN_CMP) mt.out.sort mt.reference
	rm mt.failed mt.out mt.out.sort

forkcallback.test : forkcallback$(PINTOOL_SUFFIX) fork forkcallback.tested forkcallback.failed
	$(PIN) -mt -t $< -- ./fork
	$(PIN_CMP) forkcallback.out forkcallback.reference
	rm forkcallback.failed forkcallback.out

kthread.test: kthread kthread.tested kthread.failed
	$(PIN) -mt -xyzzy -malloc_reuse_pages -- ./kthread >  $<.out 2>&1
	$(PIN_CMP) $<.out kthread.reference
	rm kthread.failed $<.out

mtstatic.test : thread.static mtstatic.tested mtstatic.failed
	$(PIN) -mt  -- ./thread.static >  $<.out 2>&1
	$(PIN_CMP) $<.out mtstatic.reference
	rm mtstatic.failed $<.out

sigmask-mac.test: sigmask sigmask-mac.tested sigmask-mac.failed
	$(PIN) -- ./sigmask >  $<.out 2>&1
	$(PIN_CMP) $<.out sigmask-mac.reference
	rm sigmask-mac.failed $<.out

sigmask.test: sigmask sigmask.tested sigmask.failed
	$(PIN) -- ./sigmask >  $<.out 2>&1
	$(PIN_CMP) $<.out sigmask.reference
	rm sigmask.failed $<.out

sigmaskstatic.test: sigmaskstatic sigmaskstatic.tested sigmaskstatic.failed
	$(PIN) -- ./sigmaskstatic >  $<.out 2>&1
	$(PIN_CMP) $<.out sigmask.reference
	rm sigmaskstatic.failed $<.out

swizzlealloc: swizzlealloc.c
	$(CC) -o swizzlealloc $(APP_CXXFLAGS) swizzlealloc.c


swizzle2.test: swizzle2$(PINTOOL_SUFFIX) swizzlealloc swizzle2.tested swizzle2.failed
	$(PIN) -t $< -- ./swizzlealloc >  $<.out 2>&1
	$(PIN_CMP) $<.out swizzle2.reference
	rm swizzle2.failed $<.out

swizzle3.test: swizzle3$(PINTOOL_SUFFIX) swizzlealloc swizzle3.tested swizzle3.failed
	$(PIN) -t $< -- ./swizzlealloc >  $<.out 2>&1
	$(PIN_CMP) $<.out swizzle3.reference
	rm swizzle3.failed $<.out

swizzle4.test: swizzle4$(PINTOOL_SUFFIX) swizzlealloc swizzle4.tested swizzle4.failed
	$(PIN) -t $< -- ./swizzlealloc >  $<.out 2>&1
	$(PIN_CMP) $<.out swizzle4.reference
	rm swizzle4.failed $<.out

swizzle5.test: swizzle5$(PINTOOL_SUFFIX) swizzlealloc swizzle5.tested swizzle5.failed
	$(PIN) -t $< -- ./swizzlealloc >  $<.out 2>&1
	$(PIN_CMP) $<.out swizzle5.reference
	rm swizzle5.failed $<.out

kthread: kthread.c
	$(CC) -o kthread kthread.c -lpthread

thread: thread.c
	$(CC) -o thread thread.c -lpthread

thread.static: thread.c
	$(CC) -o thread.static thread.c -lpthread -static

thread_longshort: thread_longshort.c
	$(CC) -o thread_longshort thread_longshort.c -lpthread

sigmaskstatic: sigmask.c
	$(CC) -o sigmaskstatic sigmask.c -static

testfunreplace: testfunreplace.c
	$(CC) -o testfunreplace testfunreplace.c


funreplace.test: funreplace$(PINTOOL_SUFFIX) testfunreplace funreplace.tested funreplace.failed
	$(PIN) -t $< -- ./testfunreplace >  $<.out 2>&1
	$(PIN_CMP) $<.out  funreplace.reference
	rm funreplace.failed $<.out

thread_funreplace: thread_funreplace.c
	$(CC) -o thread_funreplace  thread_funreplace.c -lpthread

funreplace_mt.test: funreplace_mt$(PINTOOL_SUFFIX) thread_funreplace funreplace_mt.tested funreplace_mt.failed
	$(PIN) -mt -t $< -- ./thread_funreplace >  $<.out 2>&1
	$(PIN_CMP) $<.out funreplace_mt.reference
	rm funreplace_mt.failed $<.out

testtcreatehook: testtcreatehook.c
	$(CC) -o testtcreatehook testtcreatehook.c -lpthread


tcreatehook.test: tcreatehook$(PINTOOL_SUFFIX) testtcreatehook tcreatehook.tested tcreatehook.failed
	$(PIN)  -mt -t $< -- ./testtcreatehook >  $<.out 2>&1
	$(PIN_CMP) $<.out tcreatehook.reference
	rm tcreatehook.failed $<.out

findthreadwithappstack.test : malmalloc$(PINTOOL_SUFFIX) thread_longshort findthreadwithappstack.tested findthreadwithappstack.failed
	$(PIN) -xyzzy -switchstack 0 -mt -t $< -- ./thread_longshort >  $<.out 2>&1
	$(PIN_CMP) $<.out findthreadwithappstack.reference
	rm findthreadwithappstack.failed $<.out

dummy.so : dummy.c
	$(CC) -shared $< -o $@

stackunswitch.test: stackunswitch$(PINTOOL_SUFFIX) thread stackunswitch.tested stackunswitch.failed
	$(PIN)  -mt -t $< -- ./thread >  $<.out 2>&1
	$(PIN_CMP) $<.out stackunswitch.reference
	rm stackunswitch.failed $<.out


# setting rpath did not work with cc on mmdcs082
soload.test: soload${PINTOOL_SUFFIX} $(DLTEST) soload.tested soload.failed
	export LD_LIBRARY_PATH=`pwd`; $(PIN) -t $< -- ./$(DLTEST)
	$(PIN_CMP) soload.out soload.reference
	rm soload.failed soload.out

# Test the automatic resizing of stripes
resize.test: resize.tested resize.failed
	$(PIN) -xyzzy -max_rtn 1024 -recycle_rtn 0 -- $(TESTAPP) makefile resize.makefile.copy
	$(PIN_CMP) makefile resize.makefile.copy
	rm resize.failed resize.makefile.copy

partialinline2.test : partialinline partialinline2.tested partialinline2.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -xyzzy -inline 0 -t $< -- $(TESTAPP) makefile $<.makefile.copy
	$(PIN_CMP) makefile $<.makefile.copy
	rm partialinline2.failed $<.makefile.copy

syscall.test : syscall$(PINTOOL_SUFFIX) syscall.tested syscall.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	$(PIN_CMP) $<.out syscall.reference
	rm syscall.failed $<.makefile.copy $<.out

inlinecall.test : inlinecall$(PINTOOL_SUFFIX) inlinecall.tested inlinecall.failed $(ONEPROG)
	$(PIN) -t $< -- ./$(ONEPROG) >  $<.out 2>&1
	$(PIN_CMP) $<.out inlinecall.reference
	rm inlinecall.failed $<.out

rollback.test : rollback$(PINTOOL_SUFFIX) rollback.tested rollback.failed $(HELLO)
	$(PIN) -t $< -save 80 -resume 110 -- ./$(HELLO)
	$(PIN) -t $< -save 80 -resume 110 -usectxt -- ./$(HELLO)
	rm rollback.failed

appcall.test : appcall$(PINTOOL_SUFFIX) appcall.tested appcall.failed $(HELLO)
	$(PIN) -t $<  -- ./$(HELLO) > $<.out
	$(PIN_CMP) $<.out appcall.reference
	rm appcall.failed $<.out

strace_ipf.test : strace_ipf$(PINTOOL_SUFFIX) strace_ipf.tested strace_ipf.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep -q "Success" strace.out
	grep -q -v "Failure" strace.out
	grep -q "eof" strace.out
	rm strace_ipf.failed strace.out $<.out

strace_ia32.test : strace_ia32$(PINTOOL_SUFFIX) strace_ia32.tested strace_ia32.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep -q "Success" strace.out
	grep -q -v "Failure" strace.out
	grep -q "eof" strace.out
	rm strace_ia32.failed strace.out $<.out

strace_arm.test : strace_arm$(PINTOOL_SUFFIX) strace_arm.tested strace_arm.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep -q "Success" strace.out
	grep -q -v "Failure" strace.out
	grep -q "eof" strace.out
	rm strace_arm.failed strace.out $<.out

context.test : context$(PINTOOL_SUFFIX) context.tested context.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep -q -v "Failure" $<.out
	grep -q "Success" $<.out
	rm context.failed $<.out $<.makefile.copy

dynmalloctrace.test : dynmalloctrace$(PINTOOL_SUFFIX) dynmalloctrace.tested dynmalloctrace.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	$(PIN_CMP) makefile $<.makefile.copy
	rm dynmalloctrace.failed $<.out $<.makefile.copy

tmtest : tmtest.cc
	$(CXX) -o tmtest tmtest.cc -lpthread

transactionalmem.test : transactionalmem$(PINTOOL_SUFFIX) transactionalmem.tested transactionalmem.failed tmtest
	$(PIN) -mt -t $< -- ./tmtest
	rm transactionalmem.failed

fork: fork.c
	$(CC) -o fork fork.c

sse-ref.test : sse-ref$(PINTOOL_SUFFIX) sse sse-ref.tested sse-ref.failed
	$(PIN)  -t $< -- ./sse 5 > $<.out
	diff --ignore-space-change sse-ref.reference $<.out
	rm sse-ref.failed $<.out

# A test program that uses SSE 
sse.o: sse.C
	${CXX} $(APP_CXXFLAGS2) -g -O0 -msse2 -o $@ -c $<
sse: sse.o
	${CXX} $(APP_CXXFLAGS2)  -g -O0 -msse2 -o $@ $<

mmap.test: mmap$(PINTOOL_SUFFIX) mmapapp mmap.tested mmap.failed
	$(PIN) -t mmap$(PINTOOL_SUFFIX) -- ./mmapapp
	rm -f mmap.failed

mmapapp: mmapapp.c
	$(CC) -o $@ mmapapp.c

pipe.test: pipeapp pipe.tested pipe.failed
	$(PIN) -- ./pipeapp
	rm -f pipe.failed

pipeapp: pipeapp.c
	$(CC) -o $@ pipeapp.c

smc_ipf.test: smcapp_ipf smc_ipf.tested smc_ipf.failed
	$(PIN) -- ./smcapp_ipf > $<.out 2>&1
	$(PIN_CMP) $<.out smc_ipf.reference
	rm smc_ipf.failed $<.out

smcapp_ipf: smcapp_ipf.c smcapp_ipf_asm.s
	$(CC) -o $@ smcapp_ipf.c -x assembler-with-cpp smcapp_ipf_asm.s

attachnat.test: attachnat attachnat.tested attachnat.failed
	rm -f attachnat.pid; \
		./attachnat 2> attachnat.pid 1> attachnat.out & \
		until test -s attachnat.pid; \
			do sleep 1; \
		done; \
		pid=`head -1 attachnat.pid | sed 's/Attach to me: //'`; \
		$(PIN) -pid $$pid; \
		wait $$!
	$(PIN_CMP) attachnat.out attachnat.reference
	rm attachnat.failed attachnat.out attachnat.pid

attachnat: attachnat.c attachnat_asm.s
	$(CC) -o $@ attachnat.c -x assembler-with-cpp attachnat_asm.s

spalign.test : spalign$(PINTOOL_SUFFIX) spalign.tested spalign.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy
	rm spalign.failed

spalign$(PINTOOL_SUFFIX): spalign.o spalign_asm.o
	$(CXX) $(PIN_LDFLAGS) -o spalign spalign.o spalign_asm.o $(PIN_LIBS)


## build rules

%.o : %.C
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<
%.o : %.cc
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<
$(TOOLS): $(PIN_LIBNAMES)
%.o : %.S
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	${LINK} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG)

stackunswitch: stackunswitch.o unswitcher.o
	$(CXX) $(PIN_LDFLAGS) -o stackunswitch stackunswitch.o unswitcher.o $(PIN_LIBS)

# BSD has the dynamic loader library baked in
ifeq ($(TARGET_OS),b)
  DL_LIB =
else
  DL_LIB = -ldl
endif


dlopen: dlopen.o dummy.so
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS) $(DL_LIB} $(DBG)

dltest: dltest.c one.c two.c
	$(CC) -o two.so -fpic -shared two.c -g
	$(CC) -o one.so -fpic -shared one.c -g
	$(CC) -o dltest $(APP_CXXFLAGS) dltest.c $(DL_LIB) -Wl,-rpath,`pwd` -g

dltest-mac: dltest.c one.c two.c
	$(CC) -o two.so -fPIC -dynamiclib two.c -g
	$(CC) -o one.so -fPIC -dynamiclib one.c -g
	#$(CC) -o dltest-mac $(APP_CXXFLAGS) dltest.c $(DL_LIB) -Wl,-rpath,`pwd` -g
	$(CC) -o dltest-mac $(APP_CXXFLAGS) dltest.c $(DL_LIB) -g

oneprog: oneprog.c
	$(CC) -O0 -g -o oneprog oneprog.c

## cleaning
clean:

	-rm -f *.exe *.o $(TOOLS) $(APPS) *.out *.tested *.failed hello.c funreplace funreplace_mt testtcreatehook kthread thread thread.static thread_longshort thread_funreplace testfunreplace dummy one sigmask swizzlealloc two *.so *.output *.d tmtest fork htab mt.out.sort threadexit sse sse-ref ctxtsave.txt memsave.txt *.makefile.copy dlopen attachnat.pid spalign

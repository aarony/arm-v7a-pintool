##
## Tests to validate Pin's emulation of application signal handling
##


include ../makefile.gnu.config


#
# None of these tests run on Windows since signals are not supported there.
#

APPS_l       = simple nestmask syncasyncapp fpcheck maskcheck segv blockloop exithandler altstack \
               errcheck
APPS_ipf_l   = siginfocheck_ipf natcheck
APPS_ia32_l  = swizzleapp_ia32 xmmcheck_ia32 xmmfaultcheck_ia32 siginfocheck_ia32
APPS_ia32e_l = swizzleapp_ia32e xmmcheck_ia32e xmmfaultcheck_ia32e siginfocheck_ia32e
APPS_arm_l   = siginfocheck_arm

TOOLS_l       = syncasynctool swizzletool exittool interceptor logtool intercepttool sigflowcheck

TESTS_l       = nestmask maskcheck altstack errcheck
TESTS_ipf_l   = simpleasync simplesync fpcheck interceptsegv log intercept sigflowcheck1 sigflowcheck2 syncasync \
                siginfocheck_ipf natcheck
TESTS_ia32_l  = simpleasync simplesync fpcheck interceptsegv swizzle_ia32 xmmcheck_ia32 xmmfaultcheck_ia32 log \
                intercept sigflowcheck1 sigflowcheck2 syncasync siginfocheck_ia32
TESTS_ia32e_l = simpleasync simplesync fpcheck interceptsegv swizzle_ia32e xmmcheck_ia32e xmmfaultcheck_ia32e log \
                intercept sigflowcheck1 sigflowcheck2 syncasync siginfocheck_ia32e
TESTS_arm_l   = simpleasync simplesync interceptsegv log intercept sigflowcheck1 sigflowcheck2 syncasync \
                siginfocheck_arm

apps  = $(APPS) $(APPS_$(TARGET)) $(APPS_$(TARGET_OS)) $(APPS_$(TARGET)_$(TARGET_OS))
tools = $(TOOLS) $(TOOLS_$(TARGET)) $(TOOLS_$(TARGET_OS)) $(TOOLS_$(TARGET)_$(TARGET_OS))
tests = $(TESTS) $(TESTS_$(TARGET)) $(TESTS_$(TARGET_OS)) $(TESTS_$(TARGET)_$(TARGET_OS)) dummy


all: $(apps) $(tools)
test: $(tests:=.test)
tests-sanity: test


#
# Rules to build the applications
#
nestmask: nestmask.c
	$(CC) -o $@ $@.c

segv: segv.c
	$(CC) -o $@ $@.c

syncasyncapp: syncasyncapp.c
	$(CC) -o $@ $@.c

fpcheck: fpcheck.c
	$(CC) -O -o $@ $@.c -lm

swizzleapp_ia32: swizzleapp.c copywithxmm_ia32.s
	$(CC) -o $@ swizzleapp.c copywithxmm_ia32.s

swizzleapp_ia32e: swizzleapp.c copywithxmm_ia32e.s
	$(CC) -o $@ swizzleapp.c copywithxmm_ia32e.s

xmmcheck_ia32: xmmcheck.c copywithxmm_ia32.s
	$(CC) -o $@ xmmcheck.c copywithxmm_ia32.s

xmmcheck_ia32e: xmmcheck.c copywithxmm_ia32e.s
	$(CC) -o $@ xmmcheck.c copywithxmm_ia32e.s

xmmfaultcheck_ia32: xmmfaultcheck.c copywithxmm_ia32.s
	$(CC) -DTARGET_IA32 -o $@ xmmfaultcheck.c copywithxmm_ia32.s

xmmfaultcheck_ia32e: xmmfaultcheck.c copywithxmm_ia32e.s
	$(CC) -DTARGET_IA32E -o $@ xmmfaultcheck.c copywithxmm_ia32e.s

blockloop: blockloop.c
	$(CC) -o $@ $@.c

maskcheck: maskcheck.c
	$(CC) -o $@ $@.c

exithandler: exithandler.c
	$(CC) -o $@ $@.c

simple: simple.c
	$(CC) -o $@ $@.c

altstack: altstack.c
	$(CC) -o $@ $@.c

siginfocheck_ia32: siginfocheck.c dofault_ia32.s
	$(CC) -o $@ siginfocheck.c dofault_ia32.s

siginfocheck_ia32e: siginfocheck.c dofault_ia32e.s
	$(CC) -o $@ siginfocheck.c dofault_ia32e.s

siginfocheck_ipf: siginfocheck.c dofault_ipf.s
	$(CC) -o $@ siginfocheck.c dofault_ipf.s

siginfocheck_arm: siginfocheck.c dofault_arm.s
	$(CC) -o $@ siginfocheck.c dofault_arm.s

errcheck: errcheck.c
	$(CC) -o $@ $@.c

natcheck: natcheck.c natcheck_asm.s
	$(CC) -o $@ natcheck.c natcheck_asm.s


#
# Rules to build the tools
#
interceptor: interceptor.C
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS) $(PIN_LDFLAGS) -o $@ $@.C $(PIN_LIBS)

syncasynctool: syncasynctool.C
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS) $(PIN_LDFLAGS) -o $@ $@.C $(PIN_LIBS)

swizzletool: swizzletool.C
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS) $(PIN_LDFLAGS) -o $@ $@.C $(PIN_LIBS)

logtool: logtool.C
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS) $(PIN_LDFLAGS) -o $@ $@.C $(PIN_LIBS)

intercepttool: intercepttool.C
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS) $(PIN_LDFLAGS) -o $@ $@.C $(PIN_LIBS)

sigflowcheck: sigflowcheck.C
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS) $(PIN_LDFLAGS) -o $@ $@.C $(PIN_LIBS)

exittool: exittool.C
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS) $(PIN_LDFLAGS) -o $@ $@.C $(PIN_LIBS)


#
# Rules to run the tests.
#
nestmask.test: nestmask.tested nestmask.failed nestmask
	$(PIN) -- ./$(@:.test=)
	rm $(@:.test=.failed)

interceptsegv.test: interceptsegv.tested interceptsegv.failed interceptor segv
	$(PIN) -t interceptor -- ./segv
	rm $(@:.test=.failed)

syncasync.test: syncasync.tested syncasync.failed syncasynctool syncasyncapp
	$(PIN) -t syncasynctool -- ./syncasyncapp
	rm $(@:.test=.failed)

fpcheck.test: fpcheck.tested fpcheck.failed fpcheck
	$(PIN) -- ./$(@:.test=)
	rm $(@:.test=.failed)

swizzle_ia32.test: swizzle_ia32.tested swizzle_ia32.failed swizzleapp_ia32 swizzletool
	$(PIN) -t swizzletool -- ./swizzleapp_ia32
	rm $(@:.test=.failed)

swizzle_ia32e.test: swizzle_ia32e.tested swizzle_ia32e.failed swizzleapp_ia32e swizzletool
	$(PIN) -t swizzletool -- ./swizzleapp_ia32e
	rm $(@:.test=.failed)

xmmcheck_ia32.test: xmmcheck_ia32.tested xmmcheck_ia32.failed xmmcheck_ia32
	$(PIN) -- ./$(@:.test=)
	rm $(@:.test=.failed)

xmmcheck_ia32e.test: xmmcheck_ia32e.tested xmmcheck_ia32e.failed xmmcheck_ia32e
	$(PIN) -- ./$(@:.test=)
	rm $(@:.test=.failed)

xmmfaultcheck_ia32.test: xmmfaultcheck_ia32.tested xmmfaultcheck_ia32.failed xmmfaultcheck_ia32
	$(PIN) -- ./$(@:.test=)
	rm $(@:.test=.failed)

xmmfaultcheck_ia32e.test: xmmfaultcheck_ia32e.tested xmmfaultcheck_ia32e.failed xmmfaultcheck_ia32e
	$(PIN) -- ./$(@:.test=)
	rm $(@:.test=.failed)

log.test: log.tested log.failed logtool nestmask
	$(PIN) -t logtool -- ./nestmask 2> log.out
	$(PIN_CMP) log.out log.reference
	rm $(@:.test=.failed)

intercept.test: intercept.tested intercept.failed intercepttool blockloop
	rm -f intercept.out; touch intercept.out; \
        $(PIN) -pause_tool 1 -t intercepttool -s 1 -c 2 -p -- ./blockloop 2> intercept.tool.out | \
            tee intercept.out & \
        until grep 'Signals are blocked' intercept.out > /dev/null; \
            do sleep 1; \
        done; \
        pid=`head -1 intercept.out | sed 's/.*to pid//'`; \
        kill -HUP $$pid; sleep 1; kill -HUP $$pid; \
        wait $$!
	$(PIN_CMP) intercept.tool.out intercept.tool.reference
	rm $(@:.test=.failed)

maskcheck.test: maskcheck.tested maskcheck.failed maskcheck
	./$(@:.test=) > $(@:.test=.out)
	$(PIN) -- ./$(@:.test=) > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

sigflowcheck1.test: sigflowcheck1.tested sigflowcheck1.failed sigflowcheck nestmask
	$(PIN) -t sigflowcheck -- ./nestmask > sigflowcheck1.out
	$(PIN_CMP) sigflowcheck1.out sigflowcheck1.reference
	rm $(@:.test=.failed)

sigflowcheck2.test: sigflowcheck2.tested sigflowcheck2.failed sigflowcheck exithandler
	$(PIN) -t sigflowcheck -- ./exithandler > sigflowcheck2.out
	$(PIN_CMP) sigflowcheck2.out sigflowcheck2.reference
	rm $(@:.test=.failed)

simpleasync.test: simpleasync.tested simpleasync.failed simple exittool
	$(PIN) -t exittool -- ./simple > simpleasync.out
	$(PIN_CMP) simpleasync.out simpleasync.reference
	rm $(@:.test=.failed)

simplesync.test: simplesync.tested simplesync.failed segv exittool
	$(PIN) -t exittool -- ./segv > simplesync.out
	$(PIN_CMP) simplesync.out simplesync.reference
	rm $(@:.test=.failed)

altstack.test: altstack.tested altstack.failed altstack
	$(PIN) -- ./altstack
	rm $(@:.test=.failed)

siginfocheck_ia32.test: siginfocheck_ia32.tested siginfocheck_ia32.failed siginfocheck_ia32
	./$(@:.test=) > $(@:.test=.out)
	$(PIN) -- ./$(@:.test=) > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

siginfocheck_ia32e.test: siginfocheck_ia32e.tested siginfocheck_ia32e.failed siginfocheck_ia32e
	./$(@:.test=) > $(@:.test=.out)
	$(PIN) -- ./$(@:.test=) > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

siginfocheck_ipf.test: siginfocheck_ipf.tested siginfocheck_ipf.failed siginfocheck_ipf
	./$(@:.test=) > $(@:.test=.out)
	$(PIN) -- ./$(@:.test=) > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

siginfocheck_arm.test: siginfocheck_arm.tested siginfocheck_arm.failed siginfocheck_arm
	./$(@:.test=) > $(@:.test=.out)
	$(PIN) -- ./$(@:.test=) > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

errcheck.test: errcheck.tested errcheck.failed errcheck
	$(PIN) -- ./errcheck
	rm $(@:.test=.failed)

natcheck.test: natcheck.tested natcheck.failed natcheck
	$(PIN) -- ./natcheck
	rm $(@:.test=.failed)

dummy.test:


clean:
	rm -f $(apps) $(tools) $(apps:=.o) $(tools:=.o) $(tests:=.tested) $(tests:=.failed) $(tests:=.out) \
        $(tests:=.tool.out) $(tests:=.pin.out)

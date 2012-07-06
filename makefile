
#
# NB: this top level makefile is recursive
#     All directories in decends into MUST be
#     be exported as part of the pinkit
#

all: 
	-$(MAKE) -C Tests test-apps
	-$(MAKE) -C ToolUnitTests
	-$(MAKE) -C SignalTests
	-$(MAKE) -C Tests
	-$(MAKE) -C SimpleExamples
	-$(MAKE) -C ManualExamples
	-$(MAKE) -C Memory
	-$(MAKE) -C InstLibExamples
	-$(MAKE) -C DebugTrace
	-$(MAKE) -C PinPoints
	-$(MAKE) -C CacheClient
	-$(MAKE) -C CodeCacheFootprint
	-$(MAKE) -C Maid
	-$(MAKE) -C Probes

tests-sanity:
	-$(MAKE) -C Tests test-apps
	-$(MAKE) -C Tests tests-sanity
	-$(MAKE) -C ToolUnitTests tests-sanity
	-$(MAKE) -C SignalTests tests-sanity
	-$(MAKE) -C Probes tests-sanity

test: 
	-$(MAKE) -C Tests test-apps
	-$(MAKE) -C ToolUnitTests test
	-$(MAKE) -C SignalTests test
	-$(MAKE) -C Tests test
	-$(MAKE) -C SimpleExamples test
	-$(MAKE) -C ManualExamples test
	-$(MAKE) -C Memory test
	-$(MAKE) -C InstLibExamples test
	-$(MAKE) -C DebugTrace test
	-$(MAKE) -C PinPoints test
	-$(MAKE) -C CacheClient test
	-$(MAKE) -C CodeCacheFootprint test
	-$(MAKE) -C Maid test
	-$(MAKE) -C Probes test
	./testsummary

clean:
	-$(MAKE) -C ToolUnitTests clean
	-$(MAKE) -C SignalTests clean
	-$(MAKE) -C Tests clean
	-$(MAKE) -C SimpleExamples clean
	-$(MAKE) -C ManualExamples clean
	-$(MAKE) -C Memory clean
	-$(MAKE) -C InstLibExamples clean
	-$(MAKE) -C DebugTrace clean
	-$(MAKE) -C PinPoints clean
	-$(MAKE) -C CacheClient clean
	-$(MAKE) -C CodeCacheFootprint clean
	-$(MAKE) -C Maid clean
	-$(MAKE) -C Probes clean




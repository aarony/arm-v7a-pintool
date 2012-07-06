include ../makefile.gnu.config

ifeq (${TARGET},ia32)
  LINK_ADDRESS =  -Wl,--section-start,.interp=0x1048420
endif

ifeq (${TARGET},ia32e)
  LINK_ADDRESS =  -Wl,--section-start,.interp=0x30401024
endif

ifeq (${TARGET},ipf)
  LINK_ADDRESS =  -Wl,--section-start,.interp=0x0000080000002000,--section-start,.preinit_array=0x00000a0000000000
endif

pin.static:
	$(CXX) -static -Wl,--defsym -Wl,_pinstatic=0 -Wl,-wrap,mmap,-wrap,__mmap,-wrap,brk,-wrap,__brk,-wrap,__sbrk,-wrap,sbrk,-wrap,__default_morecore $(LINK_ADDRESS) -o pin.static $(PIN_LPATHS) -lpinvm -lxed


MAKEFILE_LINUX=Makefile.linux
MAKEFILE_PRU=Makefile.pru

.PHONY: all linux pru linux-clean pru-clean linux-run-tests clean

all: linux pru
clean: linux-clean pru-clean

linux:
	$(MAKE) -f $(MAKEFILE_LINUX)

pru:
	$(MAKE) -f $(MAKEFILE_PRU)


linux-clean: 
	$(MAKE) -f $(MAKEFILE_LINUX) clean

pru-clean:
	$(MAKE) -f $(MAKEFILE_PRU) clean

linux-run-tests: linux
	$(MAKE) -f $(MAKEFILE_LINUX) run-tests

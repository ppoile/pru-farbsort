LINKER_COMMAND_FILE=./AM335x_PRU.cmd
LIBS=--library=$(PRU_LIB)/lib/rpmsg_lib.lib
INCLUDE=--include_path=$(PRU_LIB)/include/ --include_path=$(PRU_LIB)/include/am335x/
STACK_SIZE=0x100
HEAP_SIZE=0x100

#Common compiler and linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
CFLAGS=-v3 -O2 --display_error_number --endian=little --hardware_mac=on -ppd -ppa
#Linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
LFLAGS=--reread_libs --warn_sections --stack_size=$(STACK_SIZE) --heap_size=$(HEAP_SIZE)

TARGET=firmware
SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %,%,$(SOURCES:.c=.o))

all: $(TARGET)

# Invokes the linker (-z flag) to make the firmware
$(TARGET): $(OBJECTS) $(LINKER_COMMAND_FILE)
	$(PRU_CGT)/bin/clpru $(CFLAGS) -z -i$(PRU_CGT)/lib -i$(PRU_CGT)/include $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LINKER_COMMAND_FILE) --library=libc.a $(LIBS)

# Invokes the compiler on all c files in the directory to create the object files
%.o: %.c
	$(PRU_CGT)/bin/clpru --include_path=$(PRU_CGT)/include $(INCLUDE) $(CFLAGS) -fe $@ $<

.PHONY: all clean

clean:
	rm -rf $(TARGET) *.o *.pp

# Includes the dependencies that the compiler creates (-ppd and -ppa flags)
-include $(OBJECTS:%.o=%.pp)


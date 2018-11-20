 # dUQx firmware , main application
 #
 # This file is part of dUQx.
 #
 # Copyright (c) 2018 Alexander López Parrado
 #
 # dUQx is free software: you can redistribute it and/or modify
 # it under the terms of the GNU Lesser General Public License as published by
 # the Free Software Foundation, either version 3 of the License, or
 # (at your option) any later version.
 #
 # dUQx is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU Lesser General Public License for more details.
 #
 # You should have received a copy of the GNU Lesser General Public License
 # along with dUQx.  If not, see <https://www.gnu.org/licenses/>.
 


DEVICE  = atmega328p
F_CPU   = 16000000	# in Hz
FUSE_L  = 0xdf	# see below for fuse values for particular devices
FUSE_H  = 0xde
AVRDUDE = avrdude -F -V -c arduino -p $(DEVICE) -P /dev/ttyACM0 -b 115200 # edit this line for your programmer

CFLAGS  = -I. -DDEBUG_LEVEL=0
OBJECTS =  duqx.o serial.o adc.o

COMPILE = avr-gcc -Wall -O3 -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE)




# symbolic targets:
help:
	@echo "This is the duqx Makefile, it has no default rule. Use one of the following:"
	@echo "make hex ....... to build duqx.hex"	
	@echo "make flash ..... to flash the firmware on Arduino UNO"
	@echo "make clean ..... to delete objects and hex file"

hex: duqx.hex



# rule for uploading firmware:
flash: duqx.hex
	$(AVRDUDE) -U flash:w:duqx.hex:i

# rule for deleting dependent files (those which can be built by Make):
clean:
	rm -f duqx.hex duqx.lst duqx.obj duqx.cof duqx.list duqx.map duqx.eep.hex duqx.elf *.o  duqx.s 

# Generic rule for compiling C files:
.c.o:
	$(COMPILE) -c $< -o $@

# Generic rule for assembling Assembler source files:
.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

# Generic rule for compiling C to assembler, used for debugging only.
.c.s:
	$(COMPILE) -S $< -o $@

# file targets:



duqx.elf: $(OBJECTS)
	$(COMPILE) -o duqx.elf $(OBJECTS)

duqx.hex: duqx.elf
	rm -f duqx.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex duqx.elf duqx.hex
	avr-size -C --mcu=atmega328p duqx.elf

# debugging targets:

disasm:	duqx.elf
	avr-objdump -d duqx.elf



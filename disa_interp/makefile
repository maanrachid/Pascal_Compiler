all: interpret disassemble

interpret: machineI.o interp.o iolib.o
	gcc -o interpret machineI.o interp.o iolib.o -lm

disassemble: machineD.o disasm.o
	gcc -o disassemble machineD.o disasm.o

machineI.o: machine.c opcodes.h machine.h
	gcc -o machineI.o -c machine.c

machineD.o: machine.c opcodes.h machine.h
	gcc -o machineD.o -c -DDISASM  machine.c

disasm.o: disasm.c opcodes.h machine.h
	gcc -c disasm.c

interp.o: interp.c opcodes.h machine.h iolib.h
	gcc -c interp.c

iolib.o: iolib.c machine.h iolib.h
	gcc -c iolib.c
clean:
	rm -f *.o disassemble interpret

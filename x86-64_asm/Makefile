CFLAGS   = -g -O -Wall
CXXFLAGS = -g -O -Wall
OBJS = Reg.o Address.o Assembler.o Disassembler.o 

test.exe: test.o $(OBJS)
	g++  -o test.exe test.o $(OBJS) -lopcodes -lbfd -liberty

clean:
	-rm -f *.o

.PHONY: clean

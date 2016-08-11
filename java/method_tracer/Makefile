INCLUDES = -I. -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux

ALL: libmethod_tracer.so

libmethod_tracer.so: method_tracer.cpp
	gcc -Wall -fPIC -g -fno-rtti -lstdc++ -lpthread -shared $(INCLUDES) $< -o $@

clean:
	rm -f *.so *.o

.PHONY: all clean

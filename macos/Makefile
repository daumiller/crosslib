COMPILER  = clang
LINKER    = libtool
WARNINGS  = -Wall -Wno-incompatible-pointer-types-discards-qualifiers
C_FLAGS   = -I../include -std=c11
M_FLAGS   = -I../include -fobjc-arc
L_FLAGS   = -static

OBJECTS =      \
	keyboard.o \
	thread.o   \
	path.o     \
	process.o  \
	string.o   \
	watch.o    \
	window.o

TARGET = crosslib.a
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINKER) $(L_FLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(COMPILER) $(C_FLAGS) $(WARNINGS) -c $^ -o $@

%.o: %.m
	$(COMPILER) $(M_FLAGS) $(WARNINGS) -c $^ -o $@

clean:
	rm -f $(OBJECTS)

veryclean: clean
	rm -f $(TARGET)

remake: veryclean $(TARGET)

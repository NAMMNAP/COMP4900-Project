# Name of compiler (clang works well on macOS, but gcc is also fine).
CC = clang

# Compilation flags:
# -Wall, -Wextra for warnings, -O2 for some optimization, etc.
CFLAGS = -Wall -Wextra -O2

# The final program name:
TARGET = mission_script_example

# Libraries to link (pthread in this case).
LIBS = -lpthread

all: $(TARGET)

$(TARGET): mission_script_example.o environment.o
	$(CC) $(CFLAGS) -o $(TARGET) mission_script_example.o environment.o $(LIBS)

# Compile environment.c into an object file:
environment.o: environment.c environment.h
	$(CC) $(CFLAGS) -c environment.c -o environment.o

# Compile mission_script_example.c into an object file:
mission_script_example.o: mission_script_example.c environment.h
	$(CC) $(CFLAGS) -c mission_script_example.c -o mission_script_example.o

# Clean up built files:
clean:
	rm -f *.o $(TARGET)

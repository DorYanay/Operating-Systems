# Assigning the compiler as gcc (GNU Compiler Collection) for this makefile
CC = gcc

# The default target; it will execute when 'make' is run without specifying any target.
# It depends on the 'stnc' target, making sure 'stnc' is built.
all: stnc

# The 'stnc' target: linking the object files (sidefunctions.o and stnc.o) to produce the executable 'stnc'.
stnc: sidefunctions.o stnc.o
	gcc -o stnc stnc.o sidefunctions.o # Command to link the object files into an executable

# The 'test' target: building a test executable from the test.o and sidefunctions.o object files.
test: test.o sidefunctions.o
	gcc -o test test.o sidefunctions.o # Command to link the object files into a test executable

# The 'clean' target: a utility target to clean up the build environment by removing generated files.
clean:
	rm -f stnc test *.o *.txt # Command to remove the executable and object files, keeping the workspace clean.

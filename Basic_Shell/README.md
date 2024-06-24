# OperatingSystems-2

This project created by Dor Yanay and Nevo Gadassi that compiles five tools:

cmp: This tool compares two files and displays the differences between them. 
To use it, simply run the command ./cmp file1 file2.
 where file1 and file2 are the names of the files you want to compare.

copy: This tool copies the contents of one file to another.
 To use it, run the command ./copy source_file destination_file.
 where source_file is the file you want to copy and destination_file is the name of the copy you want to create.

encode and decode: These tools encode and decode a message using two shared libraries: libcodecA.so and libcodecB.so.
 To use them, you need to have these two shared libraries in the same directory as the encode and decode executables. 
To encode a message, run the command ./encode message, where message is the message you want to encode. 
To decode a message, run the command ./decode encoded_message, where encoded_message is the message you want to decode.

stshell: This tool is a simple shell that can execute basic commands. 
To use it, simply run the command ./stshell and you will be presented with a prompt where you can enter commands.
the stshell supports unlimited amount of pipes(till the limit of the system).


It is worth mentioning that this makefile assumes that all necessary files (source code files, shared libraries, and executables) are located in the same directory as the makefile itself. 
The makefile it uses the gcc compiler and the Wall and Werror flags.

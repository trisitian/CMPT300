Assignment 1 README

This assignment was a collaboration between Jinhan Liu (301430717) and Tristian Labanowich (301422226).

Q4:

For question 1, approximately 2-3 hours were spent on reviewing basic operations in C and limitations, and an extra 1-2 hours to program and optimize the solution.
(Jinhan) I learned that in C, the manual allocation of memory is especially important for string/char array operations, and C is more limited than C++ in terms of string manipulation.
            Pointers are still quite confusing, but the compiler outputs help a lot with setting the correct type for variables.
            It was also a good practice on making Makefiles.
(Tristian) I learned the basics of the dirent.h library and the composition of file structure in the proc folder.
            Additionaly there were different string manipulation tactics that I discovered when applying formatting to this questions, lastly I finally good a good grasp on memmove. 

Question 2: approximately 2 hours were spent working on the logic and formatting of the question.
(Tristian) I leaned how much information is stored in proc! as well as how to calculate the memory utilization of a program.
            Additionally I learned some additional parsing techniques for data.
            This question seemed very reasonable for what it was. 


For question 3, approximately 3 hours were spent on tackling the actual question.
(Jinhan) I used a tacky way to get around the limitations brought about by C. For the OS name, I looked up some documentations regarding the version file in Linux, and they seem to follow a convention.
            I tell the program to try and find the line "PRETTY_NAME=", then told it to start printing from index 13 of the captured string, thus printing the OS name until a " symbol is hit.
            It's a good review on loops and conditionals.

Assignment 1 references:
https://www.programiz.com/c-programming/c-dynamic-memory-allocation
https://www.cplusplus.com/reference/cstring/
https://www.tutorialspoint.com/c_standard_library/c_function_isdigit.htm
https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm
https://www.tutorialspoint.com/c_standard_library/c_function_strstr.htm
https://www.tutorialspoint.com/cprogramming/c_error_handling.htm
https://www.tutorialspoint.com/c_standard_library/c_function_printf.htm
https://en.cppreference.com/w/c/string/byte/strcat
https://stackoverflow.com/questions/1345670/stack-smashing-detected
https://en.wikibooks.org/wiki/C_Programming/POSIX_Reference/dirent.h
https://u.osu.edu/cstutorials/2018/09/28/how-to-debug-c-program-using-gdb-in-6-simple-steps/
https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html#creating
https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
https://pubs.opengroup.org/onlinepubs/7908799/xsh/dirent.h.html
https://www.tutorialspoint.com/c_standard_library/c_function_memmove.htm
https://www.tutorialspoint.com/c_standard_library/c_function_atoi.htm
https://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm
https://www.tutorialspoint.com/c_standard_library/c_function_strrchr.htm
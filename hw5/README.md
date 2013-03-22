EECS338: Homework #5
---------------------------

Author
========
Patrick Landis, <pal25@case.edu>

Files
=======
+ **main.c**: Contains the main program
+ **oxygen.c**: Contains the oxygen process code
+ **hydrogen.c**: Contains the hydrogen process code
+ **Makefile**: Contains the build code for the program
+ **semaphore.h**: Contains the files associated with semaphore.c
+ ** semaphore.c**: Contains wrapper functions for the sys-v functionality

How to Build
=======
To build the project simply use `make all`.
To build the project with debugging information use `make debug`

How to Run
=======
To run simply use `./main num_of_bonds` where num_of_bonds is the number of bonds
you'd like to create.
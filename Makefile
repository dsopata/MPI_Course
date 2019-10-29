# MPI compiler for C:
# CCOMP = /usr/bin/mpic++
# LOADER = /usr/bin/mpic++

# MPI compiler for C++:
CCOMP = /usr/bin/mpic++
LOADER = /usr/bin/mpic++

MPIRUN = /usr/bin/mpiexec
NP = 1

SOURCE = main.cpp MyMPI.cpp mpiusage.cpp Simulation.cpp
OBJECT_FILE = main.o MyMPI.o mpiusage.o Simulation.o

OPT = -O1

all: clean main run

main:
	$(CCOMP) MyMPI.cpp mpiusage.cpp main.cpp Simulation.cpp -o a.out

run:
	$(MPIRUN) -np $(NP) ./a.out

clean:
	rm -f *.o

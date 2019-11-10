# N-boady Simulation on a Ring
A parallel program for perfoming two-dimensional N-body calculation.  Implemented in C using MPI.    

## Setup
1. Install [MPICH](https://www.mpich.org/)
2. Run `make`
3. (Optional) Run `npm install` for visualization

## Run 
1. Run `mpiexec -n x ./nbody y data.txt`, where `x` is the number of bodies and `y` is the number of rounds
2. (Optional) Run `npm init` and open `localhost:3000` to see the simulation

## Input
data.txt contains the initial positions.  Each line contains `x-postion y-postion mass` and corresponds to one body.  The number of lines in the file has to be greather than the argument `x`.

## Details
The program consists of three phases of communication: 
1. Initialization (distributing the data).  Rank 1 process reads the input file, keeps the first one and forwards the rests.  Each processor also keeps the first one it received and forwards the rest to the next process on the Ring.
2. Simulation (some number of simulation rounds).  Each process updates and forward its position. 
3. Termination (gathering of the data).  Rank 1 process collects and outputs the data to console.

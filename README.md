tmpi - Message Passing Interface (MPI) via Tcl
====

Usage:
`package require tmpi 0.1

You have to have a valid [MPI-Installation](http://www.mpich.org/) on your machine. 
`mpiexec -n ?nb of processors/processes? tclsh85.exe script.tcl

This actually means you have many tclsh85.exe processes running, each on a different processor, and now it's the developers job to get benefit out of that. You have to use tmpi to synchronize both processes and transfer data from one process to another.

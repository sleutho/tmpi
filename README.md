tmpi - Message Passing Interface (MPI) via Tcl
====

Some old discussion can be found [here](http://www2.tcl.tk/20234).
You have to have a valid [MPI-Installation](http://www.mpich.org/) on your machine: 
```
mpiexec -n ?nb of processors/processes? tclsh85.exe script.tcl
```
This actually means you have many tclsh processes running, each on a different processor, and now it's the developers job to get benefit out of that. You have to use tmpi to synchronize both processes and transfer data from one process to another.

##Start
```Tcl
package require tmpi 0.1
```

##Commands
```Tcl
tmpi::init ?options?
```
Initialize the MPI execution environment. Returns 1 or fails.
possible options are:
* *-mpiqueue* print out the state of the message queues when tmpi::finalize is called. All processors print; the output may be hard to decipher. This is intended as a debugging aid.
* *-mpiversion* print out the version of the implementation (not of MPI), including the arguments that were used with configure.
* *-mpinice nn* Increments the nice value by nn (lowering the priority of the program by nn). nn must be positive (except for root). Not all systems support this argument; those that do not will ignore it.

NOTE: The MPI standard does not say what a program can do before an tmpi::init or after an tmpi::finalize. In the MPICH implementation, you should do as little as possible. In particular, avoid anything that changes the external state of the program, such as opening files, reading standard input or writing to standard output. According to that, the usage within Tcl is questionable, or even risky, because Tcl does a lot before it evaluate its actual Tcl-Input.


```Tcl
tmpi::size
```
Determines the size of the group associated with the communictor MPI_COMM_WORLD. Returns the size.


```Tcl
tmpi::rank
```
Determines the rank of the calling process in the communicator MPI_COMM_WORLD. Returns the rank.

```Tcl
tmpi::barrier
```
Blocks until all process have reached this routine. Returns 1 or fails.

```Tcl
tmpi::send -type [integer|double] ?-list? val dest
```
Performs a basic send of val from one process to dest process among MPI_COMM_WORLD. Send-tags are not supported. Returns 1 or fails.

```Tcl
tmpi::recv -type [integer|double] ?-length l? source
```
Counterpart of send to recieve val from source process. If -length is supplied, a list is assumed. Returns the recieved value, possibly a list of values.

```Tcl
tmpi::finalize
```
Terminates MPI execution environment. All processes must call this routine before exiting. The number of processes running after this routine is called is undefined; it is best not to perform much more than a return rc after calling. Returns 1 or fails.

##Hello World
```Tcl
package require tmpi 0.1
 
proc hello_world {} {

    tmpi::init

    set size [tmpi::size]
    set rank [tmpi::rank]

    puts "Hello world from process $rank of $size."

    tmpi::finalize
}
```
Execute:
```
mpiexec -n 2 tclsh85.exe hello_world.tcl
```

Output: 
```
>Hello world from process 0 of 2.
>Hello world from process 1 of 2.
```

## Example: [Approximation of pi](http://www.llnl.gov/computing/tutorials/parallel_comp/#ExamplesPI) without tmpi
```Tcl
proc approx_pi_sp {n} {
     
     set circle_count 0
     
     for {set i 0} {$i < $n} {incr i} {
         set x [expr rand()]
         set y [expr rand()]
         if {[expr {sqrt(pow($x,2)+pow($y,2)) < 1.0}]} {
             incr circle_count
         }
     }
     
     puts [expr {4.0 * $circle_count / $n}]
 }
 
 puts [time {approx_pi_sp 5000000} 1]
```

Execute:
```
tclsh85 approx_pi_sp.tcl
```

Output: 
```
>24.07 sek.
```

## [Approximation of pi](http://www.llnl.gov/computing/tutorials/parallel_comp/#ExamplesPI) with tmpi

```Tcl
proc approx_pi_mp {n} {
     
     
     tmpi::init
     
     set size [tmpi::size]
     set rank [tmpi::rank]
     
     set nb [expr {$n / $size}]
     
     set circle_count 0
     
     for {set i 0} {$i < $nb} {incr i} {
         set x [expr rand()]
         set y [expr rand()]
         if {[expr {sqrt(pow($x,2)+pow($y,2)) < 1.0}]} {
             incr circle_count
         }
     }
     
     # main process collects the vec
     if {$rank == 0} {
         for {set i 1} {$i < $size} {incr i} {
             set circle_count [expr {$circle_count + [tmpi::recv -type integer $i]}]
         }
         puts [expr {4.0 * $circle_count / ($nb * $size)}]
     } else  {
         for {set i 1} {$i < $size} {incr i} {
             tmpi::send -type integer $circle_count 0
         }
     }
     
     tmpi::finalize
 }
    
 puts [time {approx_pi_mp 5000000} 1]
```
Execute:
```
mpiexec -n 2 tclsh85 approx_pi_mp.tcl
```

Output: 
```
 >12.12 sek.
 >12.14 sek.
```

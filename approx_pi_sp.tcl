lappend auto_path .

package require tmpi 0.1

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

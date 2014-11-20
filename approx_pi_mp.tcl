lappend auto_path .

package require tmpi 0.1

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

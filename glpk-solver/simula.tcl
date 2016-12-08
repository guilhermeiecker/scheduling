#! /usr/bin/tclsh
# vim: autoindent shiftwidth=2 softtabstop=2 tabstop=2 :

set VAL_NODES {80 90 100}
set INC 100

foreach NODE $VAL_NODES {
	set AREA 10000
	while {$AREA > 0} {
		set RUN 1
		while {$RUN < 101} { 
			puts stderr "./main $NODE $AREA $RUN"
			exec ./main $NODE $AREA $RUN >> ../results/n$NODE.txt
			incr RUN
		}
		set AREA [expr {$AREA - $INC}]
	}
}

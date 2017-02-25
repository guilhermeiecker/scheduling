#! /usr/bin/tclsh
# vim: autoindent shiftwidth=2 softtabstop=2 tabstop=2 :

set VAL_NODES {20 30 40 50 60 70}
set INC 1000

foreach NODE $VAL_NODES {
	set AREA 10000
	while {$AREA > 0} {
		set RUN 1
		while {$RUN < 101} { 
			puts stderr "./enumerator $NODE $AREA $RUN"
			#exec qsub -e /dev/null -o results/n$NODE.txt -q high.q -l hostname=node10 -V -b y -cwd -shell n ./scheduling $NODE $AREA $RUN
			exec ./enumerator $NODE $AREA $RUN >> results/n$NODE.txt
			incr RUN
		}
		set AREA [expr {$AREA - $INC}]
	}
}

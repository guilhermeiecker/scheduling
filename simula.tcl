#! /usr/bin/tclsh
# vim: autoindent shiftwidth=2 softtabstop=2 tabstop=2 :

set VAL_NODES {100}
set INC 1000

foreach NODE $VAL_NODES {
        set AREA 4000
        while {$AREA > 0} {
                set RUN 32
                while {$RUN < 44} {
                        puts stderr "./main $NODE $AREA $RUN"
                        #exec qsub -e /dev/null -o results/n$NODE.txt -q high.q -l hostname=node10 -V -b y -cwd -shell n ./scheduling $NODE $AREA $RUN
                        exec ./main $NODE $AREA $RUN >> log/$NODE.txt
                        incr RUN
                }
                set RUN 45
                while {$RUN < 64} {
                        puts stderr "./main $NODE $AREA $RUN"
                        #exec qsub -e /dev/null -o results/n$NODE.txt -q high.q -l hostname=node10 -V -b y -cwd -shell n ./scheduling $NODE $AREA $RUN
                        exec ./main $NODE $AREA $RUN >> log/$NODE.txt
                        incr RUN
                }
                set RUN 65
                while {$RUN < 85} {
                        puts stderr "./main $NODE $AREA $RUN"
                        #exec qsub -e /dev/null -o results/n$NODE.txt -q high.q -l hostname=node10 -V -b y -cwd -shell n ./scheduling $NODE $AREA $RUN
                        exec ./main $NODE $AREA $RUN >> log/$NODE.txt
                        incr RUN
                }
                set RUN 86
                while {$RUN < 101} {
                        puts stderr "./main $NODE $AREA $RUN"
                        #exec qsub -e /dev/null -o results/n$NODE.txt -q high.q -l hostname=node10 -V -b y -cwd -shell n ./scheduling $NODE $AREA $RUN
                        exec ./main $NODE $AREA $RUN >> log/$NODE.txt
                        incr RUN
                }

                set AREA [expr {$AREA - $INC}]
        }
}

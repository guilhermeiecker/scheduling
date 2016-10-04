#! /usr/bin/tclsh
# vim: autoindent shiftwidth=4 softtabstop=4 tabstop=8 :

set RUNS 100

set NRNODES {100}

set AREASIDE {3000}

set ALPHA {4}

set BETA {25}

set TPOWER {1}

set DELTA_MI {0.1}

set BINARIO "scheduling"

set INTERVALO 3000

set CORES 6

# Cria o diretorio de saida
proc createDIR { dir } {

    global a

    if {[file exists $dir]} {
	set a 1
	set bkp_old "$dir-$a"
	while {[file exists $bkp_old]} {
	    incr a
	    set bkp_old $dir-$a
	}
	exec mv $dir $bkp_old
    }
    exec mkdir $dir
}

switch [lindex $argv 0] {

    "LOCAL" {
	foreach nrnodes $NRNODES {
	    foreach areaside $AREASIDE {
		foreach alpha $ALPHA {
		    foreach beta $BETA {
			foreach tpower $TPOWER {
			    foreach deltami $DELTA_MI {
				set dir output-nrNodes$nrnodes-areaside$areaside-alpha$alpha-beta$beta-tpower$tpower-deltami$deltami
				createDIR $dir
				for {set i 1} {$i <= $RUNS} {incr i} {
				    if {![file exists $dir/log.$i]} {
					puts stderr "./scheduling $nrnodes $areaside $alpha $beta $tpower $deltami $dir/log.$i $dir/result.$i $i"
					exec $BINARIO $nrnodes $areaside $alpha $beta $tpower $deltami $dir/logLP.$i $dir/result.$i $i >& $dir/log.$i &
					set running 10000 
					while {$running > $CORES} {
					    after $INTERVALO
					    set running [exec ps -xa | grep $BINARIO | grep -v ps | grep -v grep | wc -l  ]
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }

    "SUBMIT" {
        foreach nrnodes $NRNODES {
            foreach areaside $AREASIDE {
                foreach alpha $ALPHA {
                    foreach beta $BETA {
                        foreach tpower $TPOWER {
                            foreach deltami $DELTA_MI {
                                set dir output-nrNodes$nrnodes-areaside$areaside-alpha$alpha-beta$beta-tpower$tpower-deltami$deltami
                                createDIR $dir
                                for {set i 1} {$i <= $RUNS} {incr i} {
                                    if {![file exists $dir/log.$i]} {
                                        exec qsub -o /dev/null -e /dev/null -V -cwd -b y -shell n ./scheduling $nrnodes $areaside $alpha $beta $tpower $deltami $dir/log.$i $dir/result.$i $i
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
	    
    "PARSE" {
	foreach nrnodes $NRNODES {
	    foreach areaside $AREASIDE {
		foreach alpha $ALPHA {
		    foreach beta $BETA {
			foreach tpower $TPOWER {
			    foreach deltami $DELTA_MI {
				set dir output-nrNodes$nrnodes-areaside$areaside-alpha$alpha-beta$beta-tpower$tpower-deltami$deltami
				createDIR $dir
				for {set i 1} {$i <= $RUNS} {incr i} {
				    if {![file exists $dir/log.$i]} {
					puts stderr "./scheduling $nrnodes $areaside $alpha $beta $tpower $deltami $dir/log.$i $dir/result.$i $i"
					exec $BINARIO $nrnodes $areaside $alpha $beta $tpower $deltami $dir/log.$i $dir/result.$i $i >& /dev/null
    set running 10000 
    while {$running >= $maximo} {
       after $INTERVALO
       set running [exec ps -xa | grep $BINARIO | grep -v ps | grep -v grep | wc -l  ]
					if {[file exists $dir/result.$i]} {
					    exec cat $dir/result.$i >> $dir/results
					}
				    }
				}
				exec ./ci.m $dir/results $dir/final 3 95
			    }
			}
		    }
		}
	    }
	}
    }

	foreach proativo $PROATIVO {
	    foreach SUs $NRDESUS {
		foreach tmin $TMIN {
		    foreach tmax $TMAX {
			foreach tswitch $TSWITCH {
			    foreach pfa $PFA {
				foreach banda $BANDA {
				    foreach snr $SNR {
					foreach interfmax $INTERFMAX {
					    set dir output-SU$SUs-TMIN$tmin-TMAX$tmax-TSW$tswitch-PFA$pfa-BANDA$banda-SNR$snr-INTERFMAX$interfmax-EST$ESTIMATIVA-PRO$proativo
					    set lixo [concat [glob -nocomplain $dir/resultados.*] [glob -nocomplain $dir/final*]]
					    foreach nome $lixo {
						exec /bin/rm $nome
					    }
					    foreach intervvarr $INTERVVARR {
						foreach canais $NRDECANAIS {
						    foreach pmd $PMD {
							foreach ordem $ORDEMSENSOREAMENTO {
							    for {set i 1} {$i <= $RODADAS} {incr i} {
								exec cat $dir/resultado.$intervvarr.$canais.$pmd.$ordem.$i >> $dir/resultados.$intervvarr.$canais.$pmd.$ordem
							    }
							    exec ./ci.m $dir/resultados.$intervvarr.$canais.$pmd.$ordem $dir/final.$pmd.$canais.$ordem 10 95
							}
						    }
						}
					    }
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
    
    "PARSE-FINAL" {
	set lixo [concat [glob -nocomplain resultadosfinais-*/pmd.*] [glob -nocomplain resultadosfinais-*/tswitch.*] [glob -nocomplain resultadosfinais-*/intervvarr.*] [glob -nocomplain output*/final2.*]]
	foreach nome $lixo {
	    exec /bin/rm $nome
	}
	foreach proativo $PROATIVO {
	    foreach SUs $NRDESUS {
		foreach tmin $TMIN {
		    foreach tmax $TMAX {
			foreach pfa $PFA {
			    foreach banda $BANDA {
				foreach snr $SNR {
				    foreach interfmax $INTERFMAX {
					foreach canais $NRDECANAIS {
					    set resultdir "resultadosfinais-${canais}canais-${proativo}proativo"
					    createDIR $resultdir
					    foreach tswitch $TSWITCH {
						foreach pmd $PMD {
						    foreach intervvarr $INTERVVARR {
							foreach ordem $ORDEMSENSOREAMENTO {
							    set dir output-SU$SUs-TMIN$tmin-TMAX$tmax-TSW$tswitch-PFA$pfa-BANDA$banda-SNR$snr-INTERFMAX$interfmax-EST$ESTIMATIVA-PRO$proativo
								exec awk "\{ if (\$2 == $intervvarr) print \}; END \{\}" $dir/final.$pmd.$canais.$ordem >> $dir/final2.$intervvarr.$pmd.$canais.$ordem
							}
						    }
						}
					    }
					    foreach tswitch $TSWITCH {
						foreach pmd $PMD {
						    foreach intervvarr $INTERVVARR {
							foreach ordem $ORDEMSENSOREAMENTO {
							    set dir output-SU$SUs-TMIN$tmin-TMAX$tmax-TSW$tswitch-PFA$pfa-BANDA$banda-SNR$snr-INTERFMAX$interfmax-EST$ESTIMATIVA-PRO$proativo
								exec cat $dir/final2.$intervvarr.$pmd.$canais.$ordem >> $resultdir/intervvarr.$tmin.$tswitch.$pmd.$ordem
							}
						    }
						}
					    }
					    foreach intervvarr $INTERVVARR {
						foreach tswitch $TSWITCH {
						    foreach pmd $PMD {
							foreach ordem $ORDEMSENSOREAMENTO {
							    set dir output-SU$SUs-TMIN$tmin-TMAX$tmax-TSW$tswitch-PFA$pfa-BANDA$banda-SNR$snr-INTERFMAX$interfmax-EST$ESTIMATIVA-PRO$proativo
								exec cat $dir/final2.$intervvarr.$pmd.$canais.$ordem >> $resultdir/pmd.$tmin.$tswitch.$intervvarr.$ordem
							}
						    }
						}
					    }
					    foreach intervvarr $INTERVVARR {
						foreach pmd $PMD {
						    foreach tswitch $TSWITCH {
							foreach ordem $ORDEMSENSOREAMENTO {
							    set dir output-SU$SUs-TMIN$tmin-TMAX$tmax-TSW$tswitch-PFA$pfa-BANDA$banda-SNR$snr-INTERFMAX$interfmax-EST$ESTIMATIVA-PRO$proativo
								exec cat $dir/final2.$intervvarr.$pmd.$canais.$ordem >> $resultdir/tswitch.$tmin.$pmd.$intervvarr.$ordem
							}
						    }
						}
					    }
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
    
    default {
	puts "Tipo de simulacao invalido! (LOCAL/SUBMIT/PARSE)"
    }
        
}

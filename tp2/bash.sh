for ((i=1 ; 100000000 - $i ; i=i*10))
    do /usr/bin/time -f "%e" ./ex1-omp $i 3 >rep-q3
done

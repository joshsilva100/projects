#!/bin/bash

# runner.sh
# Prof. Jon Calhoun
# jonccal
# ECE 2230 Spring 2024
# MP3

# Script to quickly run many sorting expirements


# set these lists with the parameter space's values
sizes='250000 500000 750000 1000000 1250000'

# descending,  ascending, random
orders='1 2 3'

# sorting algorithm IDs
sorts='5'



for s in $sorts
do
    echo "Method: $s"

    for o in $orders
    do
        echo "Data Order: $o"
        
        for n in $sizes
        do
            echo $n $o $s
            ./geninput $n $o $s 2 | ./lab3 1 >> testoutput_5.txt
            #./geninput $n $o $s 1 | ./lab3 1 >> testlog.txt 
            #./geninput $n $o $s 1 | ./lab3 1 >> testlog.txt 
            #./geninput $n $o $s 1 | ./lab3 1 >> testlog.txt 
            #./geninput $n $o $s 1 | ./lab3 1 >> testlog.txt
        done
    done
done

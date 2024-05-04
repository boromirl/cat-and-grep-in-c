#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

test(){
    test_num=$1
    program=$2
    flags=$3
    file=$4
    echo "Test $test_num: $file : testing flags $flags:"
    cat $flags $file > cat_output.txt
    ./$program $flags $file > my_output.txt

    diff cat_output.txt my_output.txt

    if [ $? -eq 0 ]; then
        echo "${GREEN}SUCCESS${NC}"
    else
        echo "${RED}FAIL${NC}"
    fi
    rm cat_output.txt my_output.txt
}

echo "  -- Tests start --"

if [ $# -lt 2 ]; then
    echo "Usage: $0 <program> <file.txt>"
    exit 1
fi

program=$1

for ((i = 2; i <= $#; i++))
do
    text_file="${!i}"
    test 1 $program "-b" $text_file
    test 2 $program "-e" $text_file
    test 3 $program "-n" $text_file
    test 4 $program "-s" $text_file
    test 5 $program "-t" $text_file
    test 6 $program "-v" $text_file
    test 7 $program "" $text_file
    test 8 $program "-k" $text_file

    echo "${GREEN}--------------------------------------${NC}"
done




echo "  -- Tests end --"

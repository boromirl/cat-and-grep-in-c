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


text_files="${@:2}"
test 1 $program "-b" "${text_files[@]}"
test 2 $program "-e" "${text_files[@]}"
test 3 $program "-n" "${text_files[@]}"
test 4 $program "-s" "${text_files[@]}"
test 5 $program "-t" "${text_files[@]}"
test 6 $program "-v" "${text_files[@]}"
test 7 $program "" "${text_files[@]}"
test 8 $program "-k" "${text_files[@]}"
test 9 $program "-b" "doesnt_exist.txt"

echo "${GREEN}--------------------------------------${NC}"




echo "  -- Tests end --"
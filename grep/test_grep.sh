#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

test(){
    test_num=$1
    program=$2
    parameters=$3
    file=$4
    echo "Test $test_num: $file : testing flags $parameters:"
    grep $parameters $file > grep_output.txt
    ./$program $parameters $file > my_output.txt

    diff grep_output.txt my_output.txt

    if [ $? -eq 0 ]; then
        echo "${GREEN}SUCCESS${NC}"
    else
        echo "${RED}FAIL${NC}"
    fi

    leaks --atExit -- ./$program $parameters $file 1>my_output.txt 2> leaks_output.txt

    grep -q "leaks for" leaks_output.txt

    if [ $? -eq 0 ]; then
        echo "${RED}Memory leaks found${NC}"
    else
        echo "${GREEN}No leaks${NC}"
    fi


    rm grep_output.txt my_output.txt leaks_output.txt
}

echo "  -- Tests start --"

if [ $# -lt 2 ]; then
    echo "Usage: $0 <program> <file.txt>"
    exit 1
fi

program=$1

text_files="${@:2}"
test 1 $program "-e Lorem -e 2" "${text_files[@]}"
test 2 $program "-e Lorem sit" "${text_files[@]}"
test 3 $program "-i lor" "${text_files[@]}"
test 4 $program "-i -e SIT -e lorem" "${text_files[@]}"
test 5 $program "-v sit" "${text_files[@]}"
test 6 $program "-v -i SIT" "${text_files[@]}"
test 7 $program "-c sit" "${text_files[@]}"
test 8 $program "-c -v sit" "${text_files[@]}"
test 9 $program "-c -i SIT" "${text_files[@]}"
test 10 $program "-l sit" "${text_files[@]}"
test 11 $program "-l -v sit" "${text_files[@]}"
test 12 $program "-l -i SIT" "${text_files[@]}"
test 13 $program "-n sit" "${text_files[@]}"
test 14 $program "-n -v sit" "${text_files[@]}"
test 15 $program "-h sit" "${text_files[@]}"
test 16 $program "-h -c sit" "${text_files[@]}"
test 17 $program "-h -l sit" "${text_files[@]}"
test 18 $program "-s sit" "${text_files[@]}"
test 19 $program "-f patterns.txt" "${text_files[@]}"
test 20 $program "-o -e sit -e Lorem" "${text_files[@]}"
test 21 $program "Lorem" "${text_files[@]}"
test 22 $program "-ivnhso sit" "${text_files[@]}"
test 23 $program "-ivnhs sit" "${text_files[@]}"
test 24 $program "-ivnhs -f patterns.txt" "${text_files[@]}"
test 25 $program "-e sit -f patterns.txt" "${text_files[@]}"
test 26 $program "-e sit -f patterns.txt -f patterns2.txt" "${text_files[@]}"
test 27 $program "-f patterns.txt -i -o" "${text_files[@]}"

echo "${GREEN}--------------------------------------${NC}"




echo "  -- Tests end --"
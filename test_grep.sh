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
    rm grep_output.txt my_output.txt
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
    test 1 $program "-e Lorem -e 2" $text_file
    test 2 $program "-e Lorem sit" $text_file
    test 3 $program "-i lor" $text_file
    test 4 $program "-i -e SIT -e lorem" $text_file
    test 5 $program "-v sit" $text_file
    test 6 $program "-v -i SIT" $text_file
    test 7 $program "-c sit" $text_file
    test 8 $program "-c -v sit" $text_file
    test 9 $program "-c -i SIT" $text_file
    test 10 $program "-l sit" $text_file
    test 11 $program "-l -v sit" $text_file
    test 12 $program "-l -i SIT" $text_file
    test 13 $program "-n sit" $text_file
    test 14 $program "-n -v sit" $text_file
    test 15 $program "-h sit" $text_file
    test 16 $program "-h -c sit" $text_file
    test 17 $program "-h -l sit" $text_file
    test 18 $program "-s sit" $text_file
    test 19 $program "-f patterns.txt" $text_file
    test 20 $program "-o -e sit -e Lorem" $text_file
    test 21 $program "Lorem" $text_file
    test 22 $program "-ivnhso sit" $text_file
    test 23 $program "-ivnhs sit" $text_file
    test 24 $program "-ivnhs -f patterns.txt" $text_file

    echo "${GREEN}--------------------------------------${NC}"
done




echo "  -- Tests end --"

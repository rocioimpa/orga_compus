#!/bin/bash

GREEN="\e[32m"
RED="\e[31m"
WHITE="\e[0m"
YELLOW="\e[93m"

echo "TEST CASES FOR TP1 - ORGANIZACION DE COMPUTADORAS (66.20)"
echo ""

echo -e "$WHITE---------------------------------TEST CASE FOR EMPTY FILE--------------------------------"

echo -e "Summary: Receive an empty file as an input and handle it correctly by displaying a proper error message."
EMPTYFILENAME="empty_file.txt"
touch $EMPTYFILENAME

OUTPUT=$( ./tp1 -i $EMPTYFILENAME -o output.txt )
ERRORMSG="Unable to open input file $EMPTYFILENAME"

if [[ ${OUTPUT} == $ERRORMSG ]]; then
  echo -e "Status:\0$GREEN passed$WHITE"
else
  echo -e "Status:$RED failed"
  echo -e "$WHITE\0Expected message:" $ERRORMSG
fi

echo -e "$WHITE----------------------------------------TEST CASES---------------------------------------"

for file in ./test-files/test-cases/test-case-*.txt
do
    ./tp1 -i "$file" -o output.txt;

    TEST_NUMBER=$( echo "$file" | sed -n 's|.*/test-case-\([0-9]*\).txt|\1|p' );
    TEST_SUMMARY="./test-files/test-summary/test-summary-$TEST_NUMBER.txt"
    TEST_RESULT="./test-files/test-results/test-result-$TEST_NUMBER.txt"
    FILE_NAME="test-case-$TEST_NUMBER"

    echo -e "$WHITE----------------------------------TEST CASE $TEST_NUMBER---------------------------------"
    echo ""

    echo -e "Test case summary: "
    cat $TEST_SUMMARY
    echo ""

    if ! diff output.txt $TEST_RESULT >/dev/null ; then
      echo -e "Status:$RED failed"
      echo -e "$WHITE\0File $FILE_NAME's content does not match sample file's content"
      echo -e "Expected:"
      cat $TEST_RESULT
      echo ""
      echo -e "Got:"
      cat output.txt
      echo ""
    else
      echo -e "Status:\0$GREEN passed$WHITE"
    fi
    echo "-------------------------------------------------------------------------------"
done

echo -e "$WHITE----------------------------------TEST CASE A---------------------------------"

echo -e "Summary: Introducing hex characters"

echo -ne "\xff" > ff.bin
echo "Somos" > somos.bin
echo "Ojo" > ojo.bin
cat somos.bin ff.bin ojo.bin > input.txt

./tp1 -i input.txt -o output.txt;

if ! diff output.txt test-results/test-result-05.txt >/dev/null ; then
  echo -e "Status:$RED failed"
else
  echo -e "Status:\0$GREEN passed$WHITE"
fi

echo -e "$WHITE----------------------------------TEST CASE B---------------------------------"

echo -e "Summary: Introducing hex number '\x00'"

echo -ne "\x00" > 00.bin
cat somos.bin 00.bin ojo.bin > input.txt

./tp1 -i input.txt -o output.txt;

if  ! diff output.txt test-results/test-result-05.txt >/dev/null ; then
  echo -e "Status:$RED failed$WHITE"
else
  echo -e "Status:\0$GREEN passed$WHITE"
fi

echo "-------------------------------------------------------------------------------"

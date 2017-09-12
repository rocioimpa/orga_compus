#!/bin/bash
for file in ./test-files/test-cases/test-case-*.txt
do
    ./tp0 -i "$file" -o output.txt;
    CASE_NUMBER=$( echo "$file" | sed -n 's|.*/test-case-\([0-9]*\).txt|\1|p' );
    TEST_RESULT="./test-files/test-results/test-result-$CASE_NUMBER.txt"
    if ! diff output.txt $TEST_RESULT >/dev/null ; then
      echo "The result expected in file $file are wrong. Test failed."
      echo "Expected:"
      cat $TEST_RESULT
      echo "Result:"
      cat output.txt
      exit 1
    fi
done

echo "Test passed successfuly."

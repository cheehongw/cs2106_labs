#!/bin/bash

# Check if we have enough arguments
if [[ $# -ne 1 ]]; then
    echo "Usage: ./grade.sh <filename>"
    exit -1
fi
# Delete temporary files

rm -rf ./ref/*.out

# Compile the reference program

program=$1
gcc ./ref/*.c -o $program

# Generate reference output files

maxscore=0

for i in ./ref/*.in; do
    ./$program < $i > $i.out
    maxscore=$((maxscore + 1))
done

# Now mark submissions

#
# Note: See Lab02Qn.pdf for format of output file. Marks will be deducted for missing elements.
#

echo "Test date and time: $(date +"%A, %d %B %Y, %T")" > results.out
echo >> results.out
# Iterate over every submission directory

studentCount=0

for i in ./subs/*; do
    # Compile C code
    studentID=$(basename $i)
    gcc $i/*.c -o $i/$program 2> /dev/null
    if [[ $? -ne 0 ]]; then
        echo "Directory $studentID has a compile error." >> results.out
    fi
    
    score=0

    for j in ./ref/*.in; do
        filename=$(basename $j)
        outFile=$i/$filename.out
        refFile=$j.out
        $i/$program < $j > $outFile 2> /dev/null

        diff $outFile $refFile &> /dev/null
        if [[ $? -eq 0 ]]; then
            score=$((score + 1))
        fi
    done 

    echo "Directory $studentID score $score / $maxscore" >> results.out
    studentCount=$((studentCount + 1))
    # Generate output from C code using *.in files in ref
    # Compare with reference output files  and award 1 mark if they are identical
# print score for student
# print total files marked.
done

echo -e "\nProcessed $studentCount files." >> results.out

exit 0
    

#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Lab 2: I/O Lab                          Fall 2020                               System Programming
#
# script to test I/O Lab before submit
# this script is NOT OFFICIAL test script, which is written by a student
# there might be some errors
# tested in vm of the class only
#
# !WARNING! This script **DOES NOT GUARANTEE 100% PASS** on real test
#
# Author: Lee Inyong <dyd1928@naver.com>
#

RED='\033[1;31m'
LIGHTGREEN='\033[1;32m'
NC='\033[0m'
PPWD=$PWD
TARGET=$PWD/.

echo "Compiling..."
cd $TARGET
make --silent
if [ $? != 0 ]; then
  echo Compile Fail
  exit 1
fi

. testcases.sh

mkdir -p $OUTPUT_DIR
mkdir -p $EXPECTED_DIR

function run() {
  i=$1
  j=$2

  LABEL=${TESTCASES[$i]}
  PARAMS=${TESTCASES[$i + 1]}
  MODIFIER=${OPTIONS[$j]}
  OPTION=${OPTIONS[$j + 1]}

  printf "%3d) " "$((($i / 2) * (${#OPTIONS[@]} / 2) + ($j / 2) + 1))"
  echo -n "Testing ${LABEL}${MODIFIER}..."
  O_NAME="${TEMP_OUTPUT}${LABEL}${MODIFIER}"
  E_NAME="${TEMP_EXPECTED}${LABEL}${MODIFIER}"
  $MINE $OPTION $PARAMS > "$O_NAME"
  $REF $OPTION $PARAMS > "$E_NAME"
  OUTPUT=`cat $O_NAME 2>/dev/null`
  EXPECTED=`cat $E_NAME 2>/dev/null`
  if [ "$OUTPUT" = "$EXPECTED" ]; then
    echo -e "${LIGHTGREEN}PASS${NC}"
    return 1
  else
    echo -e "${RED}FAIL${NC}"
    return 0
  fi
}

TOTAL=0
PASS=0
for (( i = 0 ; i < ${#TESTCASES[@]} ; i += 2 )); do
  for (( j = 0 ; j < ${#OPTIONS[@]} ; j += 2 )); do
    run $i $j
    PASS=$(($PASS + $?))
    TOTAL=$(($TOTAL + 1))
  done
done
echo "RESULT: ${PASS}/${TOTAL}"

exit 0

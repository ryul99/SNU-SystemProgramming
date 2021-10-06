#!/bin/bash
TARGET=.
. testcases.sh

if [ $# != 1 ]; then
  echo Enter Testcase Number
  exit 1
fi

NUMBER=$(($1 - 1))
OPTION_LEN=${#OPTIONS[@]}
LABEL=${TESTCASES[$((($NUMBER / ($OPTION_LEN / 2)) * 2))]}
MODIFIER=${OPTIONS[$((($NUMBER % ($OPTION_LEN / 2)) * 2))]}

if [ "$LABEL" = "" ]; then
  echo "No Label"
  exit 1
fi

echo "vimdiff ${LABEL}${MODIFIER}"
vimdiff ${TEMP_OUTPUT}${LABEL}${MODIFIER} ${TEMP_EXPECTED}${LABEL}${MODIFIER}


MINE=${TARGET}/dirtree
REF=${TARGET}/reference/dirtree
TESTDIR=${TARGET}/testcases
OUTPUT_DIR=${TARGET}/output
EXPECTED_DIR=${TARGET}/answer
TEMP_OUTPUT=${OUTPUT_DIR}/OUTPUT_
TEMP_EXPECTED=${EXPECTED_DIR}/EXPECTED_

OPTIONS=(
  "" ""
  "-FANCY" "-t"
  "-SUMMARY" "-s"
  "-VERBOSE" "-v"
  "-FANCY-SUMMARY" "-t -s"
  "-SUMMARY-VERBOSE" "-s -v"
  "-VERBOSE-FANCY" "-v -t"
  "-ALL" "-t -s -v"
  # "MODIFIER" "OPTION"
)

TESTCASES=(
  "NOT_EXIST" "${TESTDIR}/not-exist"
  "NOT_DIR" "${TESTDIR}/not-dir"
  "NOT_PERMITTED" "${TESTDIR}/not-permitted"
  "EMPTY" "${TESTDIR}/empty"
  "VARIOUS_FILES" "${TESTDIR}/varfiles"
  # "NESTED_DIRECTORY" "${TESTDIR}/nested"
  # "NOT_PERMITTED_IN_NESTED" "${TESTDIR}/not-permitted-in-nested"
  "VVV" "${TESTDIR}/varfiles ${TESTDIR}/varfiles ${TESTDIR}/varfiles"

  "test1" "${TARGET}/tools/test1"
  "test2" "${TARGET}/tools/test2"
  "test3" "${TARGET}/tools/test3"
  "test1-and-test2" "${TARGET}/tools/test1 ${TARGET}/tools/test2"

  "ETC-VAR-LOG" "/etc /var /log"
  "DEV" "/dev"

  # "LABEL" "PARAM1 PARAM2 ..."
)

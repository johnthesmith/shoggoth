#!/bin/bash

# This script reads data from commit.json files

echo -e "folder\ttick\tresult\ttestCount\ttestSuccessCount\ttestSuccessCoefficient\tconfigTestSuccessCoefficient" && \
find . -type f -path "./ver/*/mon/commit.json" -exec sh -c '
    for file; do 
        folder=$(dirname "$file")
        jq -r --arg folder "$folder" '\''[
            $folder,
            .tick,
            .result,
            (.reason.testCount | tonumber),
            (.reason.testSuccessCount | tonumber),
            (.reason.testSuccessCoeffitient | tonumber),
            (.reason.configTestSuccessCoeffitient | tonumber),
            (.reason.test_error.errorsBeforeChange )
        ] | @tsv'\'' "$file"
    done
' sh {} + | column -t


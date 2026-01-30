#!/bin/bash

DB_FILE="db.sql"

sqlite3 "$DB_FILE" <<EOF

SELECT
    nc.netVersion,
    REPLACE(REPLACE(ns.mutation, CHAR(10), ''), CHAR(13), '') AS mutation,
    nc.result,
    nc.currentSurvivalErrorAvg,
    nc.parentSurvivalErrorAvg
FROM
    net_start ns
JOIN
    net_commit nc ON ns.netVersion = nc.netVersion;

;
EOF




./stop.sh
rm db.sql
rm -rf ./mon
rm -rf ./log
rm *.log
find ./ver -mindepth 1 -maxdepth 1 ! -name '000000.000000.000000' -exec rm -rf {} +

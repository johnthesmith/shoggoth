#cat ./mon/dump/AFTER_FRONT_retina_VALUES.txt ; \
#echo ---- ; \


watch -n 0.1 \
" \
cat ./mon/dump/AFTER_FRONT_cortex_1_VALUES.txt ; \
echo ; \
cat ./mon/dump/AFTER_FRONT_cortex_2_VALUES.txt ; \
echo ; \
cat ./mon/dump/AFTER_FRONT_result_VALUES.txt ; \
echo ; \
cat ./mon/dump/AFTER_FRONT_sample_VALUES.txt ; \
echo ; \
cat ./mon/dump/AFTER_FRONT_error_VALUES.txt ; \
echo ; \
cat ./mon/dump/AFTER_FRONT_command_VALUES.txt ; \
echo ; \
cat ./mon/dump/AFTER_FRONT_bias_VALUES.txt; \
echo ; \
cat ./mon/dump/AFTER_FRONT_test_VALUES.txt; \
echo ; \
cat ./mon/dump/AFTER_FRONT_test_error_VALUES.txt; \
echo ; \
cat ./mon/dump/AFTER_FRONT_test_command_VALUES.txt \
"

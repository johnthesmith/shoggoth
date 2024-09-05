watch -d -n 0.1 \
" \
cat ./mon/dump/AFTER_BACK_retina_ERRORS.txt && \
echo ---- && \
cat ./mon/dump/AFTER_BACK_cortex_1_ERRORS.txt && \
echo ---- && \
cat ./mon/dump/AFTER_BACK_cortex_2_ERRORS.txt && \
echo ---- && \
cat ./mon/dump/AFTER_BACK_result_ERRORS.txt && \
echo ---- && \
cat ./mon/dump/AFTER_BACK_sample_ERRORS.txt && \
echo ---- && \
cat ./mon/dump/AFTER_BACK_error_ERRORS.txt && \
echo ---- && \
cat ./mon/dump/AFTER_BACK_command_ERRORS.txt && \
echo ---- && \
cat ./mon/dump/AFTER_BACK_bias_ERRORS.txt \
"

delimiter="---------------------------------------------"

watch -d -n 0.1 \
" \
cat ./mon/dump/AFTER_LEARNING_WEIGHTS_cortex_1[0,0,0]-PARENT.txt && \
echo $delimiter && \
cat ./mon/dump/AFTER_LEARNING_WEIGHTS_cortex_1[1,0,0]-PARENT.txt && \
echo $delimiter && \
cat ./mon/dump/AFTER_LEARNING_WEIGHTS_cortex_2[0,0,0]-PARENT.txt && \
echo $delimiter && \
cat ./mon/dump/AFTER_LEARNING_WEIGHTS_cortex_2[1,0,0]-PARENT.txt && \
echo $delimiter && \
cat ./mon/dump/AFTER_LEARNING_WEIGHTS_result[0,0,0]-PARENT.txt && \
echo $delimiter && \
cat ./mon/dump/AFTER_LEARNING_WEIGHTS_result[1,0,0]-PARENT.txt \
"

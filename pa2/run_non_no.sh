PREFETCHER=no

./build_champsim.sh $PREFETCHER && \
./bin/champsim-offset_prefetcher -warmup_instructions 25000000 -simulation_instructions 25000000 -traces ./traces/trace1.champsimtrace.xz > trace1_non_${PREFETCHER}_output.txt && \
./bin/champsim-offset_prefetcher -warmup_instructions 25000000 -simulation_instructions 25000000 -traces ./traces/trace2.champsimtrace.xz > trace2_non_${PREFETCHER}_output.txt && \
./bin/champsim-offset_prefetcher -warmup_instructions 25000000 -simulation_instructions 25000000 -traces ./traces/trace3.champsimtrace.xz > trace3_non_${PREFETCHER}_output.txt && \
./bin/champsim-offset_prefetcher -warmup_instructions 25000000 -simulation_instructions 25000000 -traces ./traces/trace4.champsimtrace.xz > trace4_non_${PREFETCHER}_output.txt
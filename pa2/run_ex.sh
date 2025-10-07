PREFETCHER=offset_prefetcher

./build_champsim_exclusive.sh $PREFETCHER && \
./bin/champsim-exclusive-offset_prefetcher -warmup_instructions 25000000 -simulation_instructions 25000000 -traces ./traces/trace1.champsimtrace.xz > trace1_ex_${PREFETCHER}_output.txt && \
./bin/champsim-exclusive-offset_prefetcher -warmup_instructions 25000000 -simulation_instructions 25000000 -traces ./traces/trace2.champsimtrace.xz > trace2_ex_${PREFETCHER}_output.txt && \
./bin/champsim-exclusive-offset_prefetcher -warmup_instructions 25000000 -simulation_instructions 25000000 -traces ./traces/trace3.champsimtrace.xz > trace3_ex_${PREFETCHER}_output.txt && \
./bin/champsim-exclusive-offset_prefetcher -warmup_instructions 25000000 -simulation_instructions 25000000 -traces ./traces/trace4.champsimtrace.xz > trace4_ex_${PREFETCHER}_output.txt
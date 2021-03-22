import json
import os
import sys

from collections import defaultdict

if len(sys.argv) != 2:
    print('Usage: {} directory'.format(sys.argv[0]))
    sys.exit(0)

parent_dir = sys.argv[1]

if not os.path.isdir(parent_dir):
    print('Directory {} not found!'.format(parent_dir))
    sys.exit(0)

os.chdir(parent_dir)

filters = [
        'sim_insts',
        'sim_ops',
        'sim_ticks',
        'system.cpu.numCycles',
        'system.cpu.num_load_insts',
        'system.cpu.num_store_insts',
        'system.cpu.num_mem_refs',
        'system.cpu.num_idle_cycles',
        'system.cpu.op_class::MemRead',
        'system.cpu.op_class::MemWrite',
        'system.cpu.dcache.ReadReq_hits::total',
        'system.cpu.dcache.ReadReq_misses::total',
        'system.cpu.dcache.ReadReq_miss_rate::total',
        'system.cpu.dcache.ReadReq_accesses::total',
        'system.cpu.dcache.ReadReq_avg_miss_latency::total',
        'system.cpu.dcache.WriteReq_hits::total',
        'system.cpu.dcache.WriteReq_misses::total',
        'system.cpu.dcache.WriteReq_miss_rate::total',
        'system.cpu.dcache.WriteReq_accesses::total',
        'system.cpu.dcache.WriteReq_avg_miss_latency::total',
        'system.mem_ctrl.readReqs',
        'system.mem_ctrl.writeReqs',
        'system.mem_ctrl.nvm.bytes_read::.cpu.data',
        'system.mem_ctrl.nvm.bytes_written::total',
        'system.mem_ctrl.nvm.num_reads::.cpu.data',
        'system.mem_ctrl.nvm.num_writes::total',
        'system.nvm_ctrl.readReqs',
        'system.nvm_ctrl.writeReqs',
        'system.nvm_ctrl.nvm.bytes_read::.cpu.data',
        'system.nvm_ctrl.nvm.bytes_written::total',
        'system.nvm_ctrl.nvm.num_reads::.cpu.data',
        'system.nvm_ctrl.nvm.num_writes::total',
]

def parse_stats(stats):
    data = {}

    for entry in stats:
        if '#' not in entry:
            continue

        entry = entry.split()

        comment_starts = entry.index('#')
        entry_type = entry[0]
        entry_data = ' '.join(entry[1:comment_starts])

        data[entry_type] = entry_data

    return data

def data_diffs(data):
    if len(set(data)) == 1:
        return '{} (no diff)'.format(data[0])

    return '\t'.join(data)

benchmarks = [x for x in os.listdir() if os.path.isdir(x)]

benchmarks_data = {}

for benchmark in benchmarks:
    memory_configs = [x for x in os.listdir(benchmark) if os.path.isdir('{}/{}'.format(benchmark, x))]

    benchmarks_data[benchmark] = {}

    for memory_config in memory_configs:
        with open('{}/{}/stats.txt'.format(benchmark, memory_config)) as fp:
            stats = fp.readlines()
            benchmarks_data[benchmark][memory_config] = parse_stats(stats)

filtered_data = {}

for benchmark, benchmark_data in benchmarks_data.items():
    filtered_data[benchmark] = defaultdict(lambda : defaultdict(list))

    for memory_config, memory_data in benchmark_data.items():
        for entry_type, entry_val in memory_data.items():
            if entry_type in filters:
                filtered_data[benchmark][entry_type][memory_config] = entry_val

with open('results.json', 'w') as fp:
    json.dump(filtered_data, fp)

pretty_data = ''
pretty_data_suppress = ''

for benchmark, benchmark_data in filtered_data.items():
    for entry_type, entry_vals in benchmark_data.items():
        if len(pretty_data) == 0:
            memory_types = '\t'.join([x for x in entry_vals.keys()])
            pretty_data = 'BENCHMARK\tMETRIC\t{}\n'.format(memory_types)
            pretty_data_suppress = 'BENCHMARK\tMETRIC\t{}\n'.format(memory_types)

        entry_vals_parsed = data_diffs([x for x in entry_vals.values()])
        pretty_data += '{}\t{}\t{}\n'.format(benchmark, entry_type, entry_vals_parsed)

        if 'no diff' not in entry_vals_parsed:
            pretty_data_suppress += '{}\t{}\t{}\n'.format(benchmark, entry_type, entry_vals_parsed)

with open('results.txt', 'w') as fp:
    fp.write(pretty_data)

with open('results_diff.txt', 'w') as fp:
    fp.write(pretty_data_suppress)

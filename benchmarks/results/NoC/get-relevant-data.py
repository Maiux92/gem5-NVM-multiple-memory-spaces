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

        'system.cpu0.numCycles',
        'system.cpu0.num_busy_cycles',
        'system.cpu0.num_idle_cycles',
        'system.cpu0.num_load_insts',
        'system.cpu0.num_store_insts',
        'system.cpu0.num_mem_refs',
        'system.cpu0.op_class::MemRead',
        'system.cpu0.op_class::MemWrite',

        'system.cpu1.numCycles',
        'system.cpu1.num_busy_cycles',
        'system.cpu1.num_idle_cycles',
        'system.cpu1.num_load_insts',
        'system.cpu1.num_store_insts',
        'system.cpu1.num_mem_refs',
        'system.cpu1.op_class::MemRead',
        'system.cpu1.op_class::MemWrite',

        'system.cpu2.numCycles',
        'system.cpu2.num_busy_cycles',
        'system.cpu2.num_idle_cycles',
        'system.cpu2.num_load_insts',
        'system.cpu2.num_store_insts',
        'system.cpu2.num_mem_refs',
        'system.cpu2.op_class::MemRead',
        'system.cpu2.op_class::MemWrite',

        'system.cpu3.numCycles',
        'system.cpu3.num_busy_cycles',
        'system.cpu3.num_idle_cycles',
        'system.cpu3.num_load_insts',
        'system.cpu3.num_store_insts',
        'system.cpu3.num_mem_refs',
        'system.cpu3.op_class::MemRead',
        'system.cpu3.op_class::MemWrite',

        'system.noc_network.network.packets_injected::total',
        'system.noc_network.network.packets_received::total',

        'system.noc_network.network.routers0.buffer_reads',
        'system.noc_network.network.routers0.buffer_writes',

        'system.noc_network.network.routers1.buffer_reads',
        'system.noc_network.network.routers1.buffer_writes',

        'system.noc_network.network.routers2.buffer_reads',
        'system.noc_network.network.routers2.buffer_writes',

        'system.noc_network.network.routers3.buffer_reads',
        'system.noc_network.network.routers3.buffer_writes',

        'system.noc_network.network.routers4.buffer_reads',
        'system.noc_network.network.routers4.buffer_writes',

        'system.noc_network.network.routers5.buffer_reads',
        'system.noc_network.network.routers5.buffer_writes',

        'system.noc_network.socket_controllers1.mem_ctrl.readReqs',
        'system.noc_network.socket_controllers1.mem_ctrl.writeReqs',
        'system.noc_network.socket_controllers1.mem_ctrl.nvm.bytes_read::total',
        'system.noc_network.socket_controllers1.mem_ctrl.nvm.bytes_written::total',
        'system.noc_network.socket_controllers1.mem_ctrl.nvm.num_reads::total',
        'system.noc_network.socket_controllers1.mem_ctrl.nvm.num_writes::total',
        'system.noc_network.socket_controllers1.mem_ctrl.nvm.bytesRead',
        'system.noc_network.socket_controllers1.mem_ctrl.nvm.bytesWritten',

        'system.noc_network.socket_controllers3.mem_ctrl.readReqs',
        'system.noc_network.socket_controllers3.mem_ctrl.writeReqs',
        'system.noc_network.socket_controllers3.mem_ctrl.nvm.bytes_read::total',
        'system.noc_network.socket_controllers3.mem_ctrl.nvm.bytes_written::total',
        'system.noc_network.socket_controllers3.mem_ctrl.nvm.num_reads::total',
        'system.noc_network.socket_controllers3.mem_ctrl.nvm.num_writes::total',
        'system.noc_network.socket_controllers3.mem_ctrl.nvm.bytesRead',
        'system.noc_network.socket_controllers3.mem_ctrl.nvm.bytesWritten',

        'system.noc_network.socket_controllers0.cacheMemory.demand_access',
        'system.noc_network.socket_controllers0.cacheMemory.demand_hist',
        'system.noc_network.socket_controllers0.cacheMemory.demand_misses',

        'system.noc_network.socket_controllers1.cacheMemory.demand_access',
        'system.noc_network.socket_controllers1.cacheMemory.demand_hist',
        'system.noc_network.socket_controllers1.cacheMemory.demand_misses',

        'system.noc_network.socket_controllers2.cacheMemory.demand_access',
        'system.noc_network.socket_controllers2.cacheMemory.demand_hist',
        'system.noc_network.socket_controllers2.cacheMemory.demand_misses',

        'system.noc_network.socket_controllers3.cacheMemory.demand_access',
        'system.noc_network.socket_controllers3.cacheMemory.demand_hist',
        'system.noc_network.socket_controllers3.cacheMemory.demand_misses',

        'system.noc_network.socket_controllers4.cacheMemory.demand_access',
        'system.noc_network.socket_controllers4.cacheMemory.demand_hist',
        'system.noc_network.socket_controllers4.cacheMemory.demand_misses',
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

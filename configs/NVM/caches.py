import m5
from m5.objects import Cache

class L1Cache(Cache):
    assoc = 2
    tag_latency = 2
    data_latency = 2
    response_latency = 2
    mshrs = 4
    tgts_per_mshr = 20

    def __init__(self):
        super(L1Cache, self).__init__()

    def connectBus(self, bus):
        self.mem_side = bus.cpu_side_ports

    def connectCPU(self, cpu):
        raise NotImplementedError


class L1ICache(L1Cache):
    #size = '16kB'
    size = '128B'

    def __init__(self):
        super(L1ICache, self).__init__()

    def connectCPU(self, cpu):
        self.cpu_side = cpu.icache_port


class L1DCache(L1Cache):
    #size = '64kB'
    size = '128B'

    def __init__(self):
        super(L1DCache, self).__init__()

    def connectCPU(self, cpu):
        self.cpu_side = cpu.dcache_port


class L2Cache(Cache):
    #size = '256kB'
    size = '512B'

    assoc = 8
    tag_latency = 20
    data_latency = 20
    response_latency = 20
    mshrs = 20
    tgts_per_mshr = 12

    def __init__(self):
        super(L2Cache, self).__init__()

    def connectCPUSideBus(self, bus):
        self.cpu_side = bus.mem_side_ports

    def connectMemSideBus(self, bus):
        self.mem_side = bus.cpu_side_ports

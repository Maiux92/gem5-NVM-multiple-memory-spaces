
import m5
from m5.defines import buildEnv
from m5.objects import *
from m5.util import addToPath, fatal
import math


class NoCSocket(object):
    """docstring for NoCSocket."""




    def __init__(self):
        super(NoCSocket, self).__init__()
        self.controller = 0


class DirectorySocket(NoCSocket):
    """docstring for DirectorySocket."""

    _version = 0
    @classmethod
    def versionCount(cls):
        cls._version += 1 # Use count for this particular type
        return cls._version - 1


    def __init__(self, ruby_system, mem_ctrl):
        """ranges are the memory ranges assigned to this controller.
        """
        super(DirectorySocket, self).__init__()

        self.version = self.versionCount()


        controller = Directory_Controller()
        self.controller = controller
        controller.mem_ctrl = mem_ctrl

        controller.version = self.version
        controller.addr_ranges = mem_ctrl.nvm.range
        controller.ruby_system = ruby_system
        controller.directory = RubyDirectoryMemory()

        # Connect this directory to the memory side.
        controller.memory = mem_ctrl.port
        self.connectQueues(ruby_system)


    def connectQueues(self, ruby_system):

        if buildEnv['PROTOCOL'] == 'MI_example':

            # Connect the directory controllers and the network
            self.controller.requestToDir = MessageBuffer(ordered = True)
            self.controller.requestToDir.in_port = ruby_system.network.out_port

            self.controller.responseFromDir = MessageBuffer()
            self.controller.responseFromDir.out_port = ruby_system.network.in_port
            self.controller.forwardFromDir = MessageBuffer()
            self.controller.forwardFromDir.out_port = ruby_system.network.in_port
            self.controller.requestToMemory = MessageBuffer()
            self.controller.responseFromMemory = MessageBuffer()

            self.controller.dmaRequestToDir = MessageBuffer(ordered = True)
            self.controller.dmaRequestToDir.out_port = ruby_system.network.in_port
            self.controller.dmaResponseFromDir = MessageBuffer(ordered = True)
            self.controller.dmaResponseFromDir.in_port = ruby_system.network.out_port


        if buildEnv['PROTOCOL'] == 'MOESI_CMP_directory':
            # Connect the directory controllers and the network
            self.controller.requestToDir = MessageBuffer()
            self.controller.requestToDir.in_port = ruby_system.network.out_port
            self.controller.responseToDir = MessageBuffer()
            self.controller.responseToDir.in_port = ruby_system.network.out_port
            self.controller.responseFromDir = MessageBuffer()
            self.controller.responseFromDir.out_port = ruby_system.network.in_port
            self.controller.forwardFromDir = MessageBuffer()
            self.controller.forwardFromDir.out_port = ruby_system.network.in_port
            self.controller.requestToMemory = MessageBuffer()
            self.controller.responseFromMemory = MessageBuffer()



class CPUSocket(NoCSocket):
    """docstring for CPUSocket."""

    _version = 0
    @classmethod
    def versionCount(cls):
        cls._version += 1 # Use count for this particular type
        return cls._version - 1


    def __init__(self, system, ruby_system):
        super(CPUSocket, self).__init__()

        self.version = self.versionCount()

        cpu = TimingSimpleCPU()
        self.cpu = cpu
        cpu.clk_domain = system.cpu_clk_domain


        self.cache = RubyCache(size = '16kB',
                        assoc = 8,
                        start_index_bit = self.getBlockSizeBits(system))

        self.controller = L1Cache_Controller(version = self.version,
                                    send_evictions = self.sendEvicts(cpu),
                                    ruby_system = ruby_system,
                                    clk_domain = cpu.clk_domain,
                                    cacheMemory = self.cache)


        self.connectQueues(ruby_system)



        sequencer = RubySequencer(version = self.version,
                                # I/D cache is combined and grab from ctrl
                                icache = self.cache,
                                dcache = self.cache,
                                clk_domain = cpu.clk_domain)

        #ruby_system.sequencers.append(sequencer)
        self.sequencer = sequencer
        self.controller.sequencer = sequencer


        cpu.icache_port = sequencer.in_ports
        cpu.dcache_port = sequencer.in_ports
        isa = buildEnv['TARGET_ISA']

        cpu.createInterruptController()

        if isa == 'x86':
            cpu.interrupts[0].pio = sequencer.interrupt_out_port
            cpu.interrupts[0].int_requestor = sequencer.in_ports
            cpu.interrupts[0].int_responder = sequencer.interrupt_out_port
        if isa == 'x86' or isa == 'arm':
            cpu.itb.walker.port = sequencer.in_ports
            cpu.dtb.walker.port = sequencer.in_ports




    def getBlockSizeBits(self, system):
        bits = int(math.log(system.cache_line_size, 2))
        if 2**bits != system.cache_line_size.value:
            panic("Cache line size not a power of 2!")
        return bits


    def sendEvicts(self, cpu):
        """True if the CPU model or ISA requires sending evictions from caches
            to the CPU. Two scenarios warrant forwarding evictions to the CPU:
            1. The O3 model must keep the LSQ coherent with the caches
            2. The x86 mwait instruction is built on top of coherence
            3. The local exclusive monitor in ARM systems
        """
        if type(cpu) is DerivO3CPU or \
            buildEnv['TARGET_ISA'] in ('x86', 'arm'):
            return True
        return False


    def connectQueues(self, ruby_system):


        if buildEnv['PROTOCOL'] == 'MI_example':

            self.controller.mandatoryQueue = MessageBuffer()

            # Connect the L1 controllers and the network
            self.controller.requestFromCache = MessageBuffer(ordered = True)
            self.controller.requestFromCache.out_port = ruby_system.network.in_port
            self.controller.responseFromCache = MessageBuffer(ordered = True)
            self.controller.responseFromCache.out_port = ruby_system.network.in_port
            self.controller.forwardToCache = MessageBuffer(ordered = True)
            self.controller.forwardToCache.in_port = ruby_system.network.out_port
            self.controller.responseToCache = MessageBuffer(ordered = True)
            self.controller.responseToCache.in_port = ruby_system.network.out_port

        if buildEnv['PROTOCOL'] == 'MOESI_CMP_directory':
            self.controller.mandatoryQueue = MessageBuffer()
            self.controller.requestFromL1Cache = MessageBuffer()
            self.controller.requestFromL1Cache.out_port = ruby_system.network.in_port
            self.controller.responseFromL1Cache = MessageBuffer()
            self.controller.responseFromL1Cache.out_port = ruby_system.network.in_port
            self.controller.requestToL1Cache = MessageBuffer()
            self.controller.requestToL1Cache.in_port = ruby_system.network.out_port
            self.controller.responseToL1Cache = MessageBuffer()
            self.controller.responseToL1Cache.in_port = ruby_system.network.out_port
            self.controller.triggerQueue = MessageBuffer(ordered = True)

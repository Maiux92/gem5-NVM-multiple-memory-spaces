import m5
from m5.objects import *

from caches import *
from NVMTypes import *

# Supported: SRAM, FRAM, MRAM, RRAM, STTRAM
MEM_TYPE = 'STTRAM'

NVM_START_ADDR = 0x30000000

system = System()

system.clk_domain = SrcClockDomain()
#system.clk_domain.clock = '1GHz'
system.clk_domain.clock = '16MHz'
system.clk_domain.voltage_domain = VoltageDomain()

system.mem_mode = 'timing'
system.mem_ranges = [
        AddrRange('512MB'),
        AddrRange(NVM_START_ADDR, size='512MB')
]

system.cpu = TimingSimpleCPU()

system.cpu.icache = L1ICache()
system.cpu.dcache = L1DCache()

system.cpu.icache.connectCPU(system.cpu)
system.cpu.dcache.connectCPU(system.cpu)

system.l2bus = L2XBar()
system.cpu.icache.connectBus(system.l2bus)
system.cpu.dcache.connectBus(system.l2bus)

system.l2cache = L2Cache()
system.l2cache.connectCPUSideBus(system.l2bus)

system.membus = SystemXBar()
system.l2cache.connectMemSideBus(system.membus)

#system.cpu.icache_port = system.membus.cpu_side_ports
#system.cpu.dcache_port = system.membus.cpu_side_ports

system.cpu.createInterruptController()

if m5.defines.buildEnv['TARGET_ISA'] == "x86":
    system.cpu.interrupts[0].pio = system.membus.mem_side_ports
    system.cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
    system.cpu.interrupts[0].int_responder = system.membus.mem_side_ports

system.mem_ctrl = MemCtrl()
# target MCU uses SRAM...
#system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.nvm = SRAM()
system.mem_ctrl.nvm.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

system.nvm_ctrl = MemCtrl()
#system.nvm_ctrl.dram = DDR3_1600_8x8()
#system.nvm_ctrl.dram.range = system.mem_ranges[1]
if MEM_TYPE == 'SRAM':
    system.nvm_ctrl.nvm = SRAM()

elif MEM_TYPE == 'FRAM':
    system.nvm_ctrl.nvm = FRAM()

elif MEM_TYPE == 'MRAM':
    system.nvm_ctrl.nvm = MRAM()

elif MEM_TYPE == 'RRAM':
    system.nvm_ctrl.nvm = RRAM()

elif MEM_TYPE == 'STTRAM':
    system.nvm_ctrl.nvm = STTRAM()

else:
    print("Unsupported memory type!")
    system.nvm_ctrl.nvm = NVM_2400_1x64()

system.nvm_ctrl.nvm.range = system.mem_ranges[1]
system.nvm_ctrl.port = system.membus.mem_side_ports

system.system_port = system.membus.cpu_side_ports

isa = str(m5.defines.buildEnv['TARGET_ISA']).lower()

binary = 'tests/soc-project/a.out'

#system.workload = SEWorkload()

process = Process()
process.cmd = [binary]
system.cpu.workload = process
system.cpu.createThreads()

root = Root(full_system = False, system = system)
m5.instantiate()
#process.map(NVM_START_ADDR, NVM_START_ADDR, 0x20000000)

print("Beginning simulation!")
exit_event = m5.simulate()
print('Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause()))

#!/usr/bin/python3


import optparse
import m5
from m5.defines import buildEnv
from m5.objects import *
from m5.util import addToPath, fatal

addToPath('../../gem5-20.1/configs/')

#addToPath('../../')
import json
import os

# from network import Network
from common import Options
from noc_network import NoCSystem
from noc_config import noc_config

#-------------------------------------------#
# Add Script arguments                      #
#-------------------------------------------#
parser = optparse.OptionParser()
# Options.addCommonOptions(parser)
# Options.addSEOptions(parser)
#Ruby.define_options(parser)
#Network.define_options(parser)


parser.add_option("--noc-config", type="string", default="noc_config.json",
                    help="""select noc configuration files (default: noc_config.json).""")
parser.add_option("--router-latency", action="store", type="int",
                  default=1,
                  help="""number of pipeline stages in the garnet router.
                        Has to be >= 1.
                        Can be over-ridden on a per router basis
                        in the topology file.""")
parser.add_option("--link-latency", action="store", type="int", default=1,
                  help="""latency of each link the simple/garnet networks.
                        Has to be >= 1.
                        Can be over-ridden on a per link basis
                        in the topology file.""")
parser.add_option("--link-width-bits", action="store", type="int",
                  default=128,
                  help="width in bits for all links inside garnet.")
parser.add_option("--vcs-per-vnet", action="store", type="int", default=4,
                  help="""number of virtual channels per virtual network
                        inside garnet network.""")
parser.add_option("--garnet-deadlock-threshold", action="store",
                  type="int", default=50000,
                  help="network-level deadlock threshold.")
parser.add_option("--nvm_type", action="store",
                  type="string", default='STTRAM',
                  help="Supported: SRAM, FRAM, MRAM, RRAM, STTRAM")
parser.add_option("--input-start-address", action="store",
                type="int", default=0x60000000,
                help="Input memory start address")
parser.add_option("--input-addr-size", action="store",
                type="string", default='512MB',
                help="Input memory address range")
#parser.add_option("--output-memtype", action="store",
#                  type="string", default='STTRAM',
#                  help="Supported: SRAM, FRAM, MRAM, RRAM, STTRAM")
parser.add_option("--output-start-address", action="store",
                type="int", default=0x90000000,
                help="Output memory start address")
parser.add_option("--output-addr-size", action="store",
                type="string", default='512MB',
                help="Output memory address range")


(options, args) = parser.parse_args()


file_path = os.path.dirname(os.path.abspath(__file__)) + \
            "/" + \
            options.noc_config

print("NVM: {}".format(options.nvm_type))

"""
with open(file_path, "r") as f:
    #print(f.read())
    options.map = json.loads(f.read())
    options.mesh_rows = len(options.map)
"""
options.map = noc_config
options.mesh_rows = len(options.map)
#-------------------------------------------#


#-------------------------------------------#
# Create system                             #
#-------------------------------------------#
system = System(mem_mode = 'timing')
#-------------------------------------------#


#-------------------------------------------#
# SET system voltages                       #
#-------------------------------------------#

# Create a top-level voltage domain
system.voltage_domain = VoltageDomain()

# Create a source clock for the system and set the clock period
system.clk_domain = SrcClockDomain(clock =  '1GHz',
                                   voltage_domain = system.voltage_domain)
# Create a CPU voltage domain
system.cpu_voltage_domain = VoltageDomain()

# Create a separate clock domain for the CPUs
system.cpu_clk_domain = SrcClockDomain(clock = '1GHz',
                                       voltage_domain = system.cpu_voltage_domain)
#-------------------------------------------#



#-------------------------------------------#
# Create NoCSystem                          #
#-------------------------------------------#
system.noc_network = NoCSystem()
system.noc_network.setup(system, options)
#-------------------------------------------#


#-------------------------------------------#
# Init CPUs process                         #
#-------------------------------------------#

# Multi-thread single file support
process = Process()
process.cmd = ['../benchmarks/a.out']

for (i, cpu) in enumerate(system.cpu):
    # Re-enable for single-process multiple cpu + multiple parallel execution of same program
    #process = Process(pid = (100 + i))
    ##process.cmd = ['../benchmarks/{}.out'.format(i)]
    #process.cmd = ['../benchmarks/a.out']
    cpu.workload = process
    cpu.createThreads()

#-------------------------------------------#



root = Root(full_system = False, system = system)
#Simulation.run(options, root, system, FutureClass)


#-------------------------------------------#
# Create system instance and launch         #
# simulation                                #
#-------------------------------------------#
m5.instantiate()

print("Beginning simulation!")
exit_event = m5.simulate()
#-------------------------------------------#


print('Exiting @ tick {} because {}'
    .format(m5.curTick(), exit_event.getCause()))

import math
import m5
from m5.objects import *
from m5.defines import buildEnv
from m5.util import addToPath, fatal
# from common import FileSystemConfig

addToPath('../configs/')
from NVMTypes import *

from socket import CPUSocket, DirectorySocket


class NoCSystem(RubySystem):

    def __init__(self):
        if buildEnv['PROTOCOL'] not in ['MI_example', 'MOESI_CMP_directory']:
            fatal("This system needs MI_example or MOESI_CMP_directory as PROTOCOL!")

        super(NoCSystem, self).__init__()


    def setup(self, system, options):

        self.network = NoCNetwork(self)

        map = options.map

        cpus = []
        cpu_sockets = []
        socket_controllers = []
        sequencers = []
        input_socket = 0
        output_socket = 0

        i_ext_node = 0

        # For each point in noc_config.json file create
        # the corrisponding controller socket.
        for row in range(len(map)):
            for col in range(len(map[row])):

                t_socket = map[row][col]

                if(t_socket == 'c'):

                    # CPU conrtoller socket
                    s_cpu = CPUSocket(system, self)
                    cpu_sockets.append(s_cpu)
                    cpus.append(s_cpu.cpu)
                    sequencers.append(s_cpu.sequencer)
                    socket_controllers.append(s_cpu.controller)


                if t_socket == 'i' or t_socket == 'o':

                    # Memory socket
                    # mem_ctrl = DDR3_1600_8x8()
                    # mem_ctrl.range = AddrRange("512MB")

                    mem_ctrl = MemCtrl()

                    if t_socket == 'i':
                        mem_ctrl.nvm = eval("%s()" % options.input_memtype)
                        mem_ctrl.nvm.range = AddrRange(options.input_start_address, size=options.input_addr_size)
                    else:
                        mem_ctrl.nvm = eval("%s()" % options.output_memtype)
                        mem_ctrl.nvm.range = AddrRange(options.output_start_address, size=options.output_addr_size)


                    s_dir = DirectorySocket(self, mem_ctrl)

                    if(t_socket == 'i'):
                        input_socket = s_dir
                    else:
                        output_socket = s_dir

                    socket_controllers.append(s_dir.controller)

                i_ext_node += 1


        self.socket_controllers = socket_controllers
        self.num_of_sequencers = len(sequencers)

        self.network.init_network(socket_controllers, options)

        # MI_example needs 5 virtual network
        if buildEnv['PROTOCOL'] == 'MI_example':
            self.number_of_virtual_networks = 5
            self.network.number_of_virtual_networks = 5

        elif buildEnv['PROTOCOL'] == 'MOESI_CMP_directory':
            self.number_of_virtual_networks = 3
            self.network.number_of_virtual_networks = 3


        self.sys_port_proxy = RubyPortProxy(ruby_system = self)
        system.system_port = self.sys_port_proxy.slave

        system.cpu = cpus


class NoCNetwork(GarnetNetwork):

    def __init__(self, ruby_system):
        super(NoCNetwork, self).__init__()
        self.netifs = []
        self.routers = []
        self.ext_links = []
        self.int_links = []
        self.ruby_system = ruby_system


    def init_network(self, controllers, options):
        self.num_rows = len(options.map)
        self.vcs_per_vnet = options.vcs_per_vnet
        self.ni_flit_size = options.link_width_bits / 8
        self.routing_algorithm = 1 # MeshXY
        self.garnet_deadlock_threshold = options.garnet_deadlock_threshold

        self.makeTopology(controllers, options)

        netifs = [GarnetNetworkInterface(id=i) for (i,n) in enumerate(self.ext_links)]
        self.netifs = netifs



    def makeTopology(self, nodes, options):

        num_routers = len(nodes)
        num_rows = len(options.map)

        # default values for link latency and router latency.
        # Can be over-ridden on a per link/router basis
        link_latency = options.link_latency # used by simple and garnet
        router_latency = options.router_latency # only used by garnet


        # There must be an evenly divisible number of cntrls to routers
        # Also, obviously the number or rows must be <= the number of routers
        cntrls_per_router = 1
        assert(num_rows > 0 and num_rows <= num_routers)
        num_columns = int(num_routers / num_rows)
        assert(num_columns * num_rows == num_routers)

        # Create the routers in the mesh
        routers = [GarnetRouter(router_id=i, latency = router_latency) \
            for i in range(num_routers)]
        self.routers = routers

        # link counter to set unique link ids
        link_count = 0


        # Connect each node to the appropriate router
        ext_links = []
        for (i, n) in enumerate(nodes):
            ext_links.append(GarnetExtLink(link_id=link_count, ext_node=n,
                                    int_node=routers[i],
                                    latency = link_latency))
            link_count += 1


        self.ext_links = ext_links

        # Create the mesh links.
        int_links = []

        # East output to West input links (weight = 1)
        for row in range(num_rows):
            for col in range(num_columns):
                if (col + 1 < num_columns):
                    east_out = col + (row * num_columns)
                    west_in = (col + 1) + (row * num_columns)
                    int_links.append(GarnetIntLink(link_id=link_count,
                                             src_node=routers[east_out],
                                             dst_node=routers[west_in],
                                             src_outport="East",
                                             dst_inport="West",
                                             latency = link_latency,
                                             weight=1))
                    link_count += 1

        # West output to East input links (weight = 1)
        for row in range(num_rows):
            for col in range(num_columns):
                if (col + 1 < num_columns):
                    east_in = col + (row * num_columns)
                    west_out = (col + 1) + (row * num_columns)
                    int_links.append(GarnetIntLink(link_id=link_count,
                                             src_node=routers[west_out],
                                             dst_node=routers[east_in],
                                             src_outport="West",
                                             dst_inport="East",
                                             latency = link_latency,
                                             weight=1))
                    link_count += 1

        # North output to South input links (weight = 2)
        for col in range(num_columns):
            for row in range(num_rows):
                if (row + 1 < num_rows):
                    north_out = col + (row * num_columns)
                    south_in = col + ((row + 1) * num_columns)
                    int_links.append(GarnetIntLink(link_id=link_count,
                                             src_node=routers[north_out],
                                             dst_node=routers[south_in],
                                             src_outport="North",
                                             dst_inport="South",
                                             latency = link_latency,
                                             weight=2))
                    link_count += 1

        # South output to North input links (weight = 2)
        for col in range(num_columns):
            for row in range(num_rows):
                if (row + 1 < num_rows):
                    north_in = col + (row * num_columns)
                    south_out = col + ((row + 1) * num_columns)
                    int_links.append(GarnetIntLink(link_id=link_count,
                                             src_node=routers[south_out],
                                             dst_node=routers[north_in],
                                             src_outport="South",
                                             dst_inport="North",
                                             latency = link_latency,
                                             weight=2))
                    link_count += 1


        self.int_links = int_links


    # # Register nodes with filesystem
    # def registerTopology(self, options):
    #     for i in range(options.num_cpus):
    #         FileSystemConfig.register_node([i],
    #                 MemorySize(options.mem_size) / options.num_cpus, i)

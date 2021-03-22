from m5.objects import NVM_2400_1x64, DDR3_1600_8x8

class FakeNVM(DDR3_1600_8x8):
    tCK = '125ns'
    tRCD = '60ns'
    tRP = '60ns'

class FRAM(NVM_2400_1x64):
    # 8 MHz
    #tCK = '250ns'

    tREAD = '384ns'
    tWRITE = '320ns'
    #tSEND = '125ns'
    #tBURST = '3.332ns'

    # Default all bus turnaround and rank bus delay to 2 cycles
    # With DDR data bus, clock = 1200 MHz = 1.666 ns
    #tWTR = '250ns';
    #tRTW = '250ns';
    #tCS = '250ns'

class RRAM(NVM_2400_1x64):
    tREAD = '40ns' 
    tWRITE = '60ns' 

class MRAM(NVM_2400_1x64):
    tREAD = '5ns' 
    tWRITE = '4ns' 

class STTRAM(NVM_2400_1x64):
    tREAD = '1.95ns'
    tWRITE = '10.5ns'

class SRAM(NVM_2400_1x64):
    tREAD = '1.23ns'
    tWRITE = '1.21ns'

# gem5 extension for supporting separate accesses to multiple (non-volatile) memory spaces

This is the final project for the course "[Advances in System-on-Chip Design](https://www11.ceda.polimi.it/schedaincarico/schedaincarico/controller/scheda_pubblica/SchedaPublic.do?&evn_default=evento&c_classe=721652&polij_device_category=DESKTOP&__pj0=0&__pj1=282bb4b139c5a8068d7956b3653d7bcb)" taught by of Prof. [Pilato](http://pilato.faculty.polimi.it) to PhD students at Politecnico di Milano (Italy).
The final project presentation is available [here](Presentation.pptx).

The project was done in collaboration with [Simone Salgaro]().

# Project
This project enables support for accesses to separate memory spaces into [gem5](https://www.gem5.org).

## Motivation
Emerging mixed-volatile platforms, such as the [MSP430-FRxxxx](https://www.ti.com/lit/ds/symlink/msp430fr5969.pdf) from Texas Instruments, feature an internal Non Volatile Memory (NVM) that is directly addressable from the MCU.
Such platforms allows programs to directly access either volatile or non-volatile memory, managing the two memories as two separate memory spaces.
At the time of the project, the NVM support of gem5 does not allow such behavior, as the entire memory is considered as a single memory space.

This project aims at enabling direct accesses to separate memory spaces, so that we can simulate the behavior of emerging mixed-volatility platforms.

## Guide and project walkthrough
The `gem5-20.1` folder contains the patched version of gem5 that supports separate memory spaces.
We provide a guide that shows how to simulate a NVM into gem5, and how to patch and use gem5 20.1 for accessing separate memory spaces [here](HOWTO-NVM.md).
Finally, we show a practical example with a multi-core system that uses a Network-on-Chip [here](HOWTO-NoC.md).

A docker image for compiling gem5 is available in the `docker` folder.

# Links
The benchmarks were taken from the [MiBench2 benchmarking suite](https://github.com/impedimentToProgress/MiBench2). 

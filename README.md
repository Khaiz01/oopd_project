Cellular Network Simulator

OOPD Project – Monsoon 2025

Overview

This project implements a Cellular Network Simulator in C++, modeling: -
User Devices - Cell Towers - Cellular Cores - 2G, 3G, 4G, and 5G network
technologies

The simulator applies OOP principles including inheritance, abstraction,
polymorphism, encapsulation, templates, and exception handling.

Features

-   Object-oriented architecture
-   Protocol classes for 2G, 3G, 4G, 5G
-   Capacity calculations
-   Frequency/channel allocation
-   Message overhead computation
-   Antenna/multi-core handling for 4G/5G
-   Exception handling
-   Template utilities

Technology Specifications

2G: - TDMA - 16 users per 200 kHz - 1 MHz → 5 channels → 80 users -
Messages/user: 5 (data), 15 (voice)

3G: - CDMA - 32 users per 200 kHz - 10 messages/user

4G: - OFDM - 30 users per 10 kHz - Up to 4 antennas - 10 messages/user

5G: - 10 MHz at 1800 MHz - 30 users per 1 MHz - Up to 16 antennas

Build Instructions

Debug Build: make debug

Release Build: make release

Clean: make clean

Run: ./bin/simulator input.txt

Input File Format

Example: 4G data 120

Exception Handling

-   OverCapacityException
-   FrequencyAllocationException
-   MessageOverflowException

OOP Concepts

-   Encapsulation
-   Inheritance
-   Polymorphism
-   Abstraction
-   Templates
-   Exception handling

submitted by,
KHAIZ C NAVAZ MT25121
ANN MARY VINU MT25100
K CHENTHAMIL MOORTHY MT25167


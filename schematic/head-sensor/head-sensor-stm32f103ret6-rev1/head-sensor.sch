EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:linStab-(1-GND,Adj,2-Out,3-in)
LIBS:nrf24l01
LIBS:stm32
LIBS:microsd-sdio
LIBS:rfid-rc522-module
LIBS:head-sensor-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Lazertag head sensor"
Date "14 jan 2015"
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L DGND #PWR?
U 1 1 54B3B0BA
P 5750 5850
F 0 "#PWR?" H 5750 5850 40  0001 C CNN
F 1 "DGND" H 5750 5780 40  0000 C CNN
F 2 "" H 5750 5850 60  0000 C CNN
F 3 "" H 5750 5850 60  0000 C CNN
	1    5750 5850
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 54B3B0DC
P 5750 1300
F 0 "#PWR?" H 5750 1260 30  0001 C CNN
F 1 "+3.3V" H 5750 1410 30  0000 C CNN
F 2 "" H 5750 1300 60  0000 C CNN
F 3 "" H 5750 1300 60  0000 C CNN
	1    5750 1300
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 54B3B2F9
P 5300 7250
F 0 "C?" H 5300 7350 40  0000 L CNN
F 1 "C" H 5306 7165 40  0000 L CNN
F 2 "~" H 5338 7100 30  0000 C CNN
F 3 "~" H 5300 7250 60  0000 C CNN
	1    5300 7250
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 54B3B308
P 5550 7250
F 0 "C?" H 5550 7350 40  0000 L CNN
F 1 "C" H 5556 7165 40  0000 L CNN
F 2 "~" H 5588 7100 30  0000 C CNN
F 3 "~" H 5550 7250 60  0000 C CNN
	1    5550 7250
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 54B3B321
P 5800 7250
F 0 "C?" H 5800 7350 40  0000 L CNN
F 1 "C" H 5806 7165 40  0000 L CNN
F 2 "~" H 5838 7100 30  0000 C CNN
F 3 "~" H 5800 7250 60  0000 C CNN
	1    5800 7250
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 54B3B330
P 6050 7250
F 0 "C?" H 6050 7350 40  0000 L CNN
F 1 "C" H 6056 7165 40  0000 L CNN
F 2 "~" H 6088 7100 30  0000 C CNN
F 3 "~" H 6050 7250 60  0000 C CNN
	1    6050 7250
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR?
U 1 1 54B3B401
P 5800 7650
F 0 "#PWR?" H 5800 7650 40  0001 C CNN
F 1 "DGND" H 5800 7580 40  0000 C CNN
F 2 "" H 5800 7650 60  0000 C CNN
F 3 "" H 5800 7650 60  0000 C CNN
	1    5800 7650
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 54B3B41A
P 5800 6850
F 0 "#PWR?" H 5800 6810 30  0001 C CNN
F 1 "+3.3V" H 5800 6960 30  0000 C CNN
F 2 "" H 5800 6850 60  0000 C CNN
F 3 "" H 5800 6850 60  0000 C CNN
	1    5800 6850
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 54B3B447
P 6300 7250
F 0 "C?" H 6300 7350 40  0000 L CNN
F 1 "C" H 6306 7165 40  0000 L CNN
F 2 "~" H 6338 7100 30  0000 C CNN
F 3 "~" H 6300 7250 60  0000 C CNN
	1    6300 7250
	1    0    0    -1  
$EndComp
$Comp
L CRYSTAL X?
U 1 1 54B3EBB5
P 3700 2550
F 0 "X?" H 3700 2700 60  0000 C CNN
F 1 "CRYSTAL" H 3700 2400 60  0000 C CNN
F 2 "~" H 3700 2550 60  0000 C CNN
F 3 "~" H 3700 2550 60  0000 C CNN
	1    3700 2550
	0    -1   -1   0   
$EndComp
$Comp
L C C?
U 1 1 54B3EBC4
P 3200 2200
F 0 "C?" H 3200 2300 40  0000 L CNN
F 1 "C" H 3206 2115 40  0000 L CNN
F 2 "~" H 3238 2050 30  0000 C CNN
F 3 "~" H 3200 2200 60  0000 C CNN
	1    3200 2200
	0    1    1    0   
$EndComp
$Comp
L C C?
U 1 1 54B3EBD3
P 3200 2900
F 0 "C?" H 3200 3000 40  0000 L CNN
F 1 "C" H 3206 2815 40  0000 L CNN
F 2 "~" H 3238 2750 30  0000 C CNN
F 3 "~" H 3200 2900 60  0000 C CNN
	1    3200 2900
	0    1    1    0   
$EndComp
$Comp
L DGND #PWR?
U 1 1 54B3ECC4
P 2900 2300
F 0 "#PWR?" H 2900 2300 40  0001 C CNN
F 1 "DGND" H 2900 2230 40  0000 C CNN
F 2 "" H 2900 2300 60  0000 C CNN
F 3 "" H 2900 2300 60  0000 C CNN
	1    2900 2300
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR?
U 1 1 54B3ECD3
P 2900 3000
F 0 "#PWR?" H 2900 3000 40  0001 C CNN
F 1 "DGND" H 2900 2930 40  0000 C CNN
F 2 "" H 2900 3000 60  0000 C CNN
F 3 "" H 2900 3000 60  0000 C CNN
	1    2900 3000
	1    0    0    -1  
$EndComp
$Comp
L LINSTAB-(1-GND/ADJ,2-OUT,3-IN) U?
U 1 1 54B3ED46
P 1600 6750
F 0 "U?" H 1600 6900 60  0000 C CNN
F 1 "LINSTAB-(1-GND/ADJ,2-OUT,3-IN)" H 1650 7100 60  0000 C CNN
F 2 "~" H 1600 6750 60  0000 C CNN
F 3 "~" H 1600 6750 60  0000 C CNN
	1    1600 6750
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 54B3ED64
P 2500 6750
F 0 "#PWR?" H 2500 6710 30  0001 C CNN
F 1 "+3.3V" H 2500 6860 30  0000 C CNN
F 2 "" H 2500 6750 60  0000 C CNN
F 3 "" H 2500 6750 60  0000 C CNN
	1    2500 6750
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 54B3EDAA
P 1600 7450
F 0 "#PWR?" H 1600 7450 30  0001 C CNN
F 1 "GND" H 1600 7380 30  0001 C CNN
F 2 "" H 1600 7450 60  0000 C CNN
F 3 "" H 1600 7450 60  0000 C CNN
	1    1600 7450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 54B3EDFD
P 2400 7450
F 0 "#PWR?" H 2400 7450 30  0001 C CNN
F 1 "GND" H 2400 7380 30  0001 C CNN
F 2 "" H 2400 7450 60  0000 C CNN
F 3 "" H 2400 7450 60  0000 C CNN
	1    2400 7450
	1    0    0    -1  
$EndComp
$Comp
L CP1 C?
U 1 1 54B3EE0C
P 2400 7100
F 0 "C?" H 2450 7200 50  0000 L CNN
F 1 "CP1" H 2450 7000 50  0000 L CNN
F 2 "~" H 2400 7100 60  0000 C CNN
F 3 "~" H 2400 7100 60  0000 C CNN
	1    2400 7100
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P?
U 1 1 54B40AAF
P 8350 2900
F 0 "P?" V 8300 2900 50  0000 C CNN
F 1 "USART1" V 8400 2900 50  0000 C CNN
F 2 "~" H 8350 2900 60  0000 C CNN
F 3 "~" H 8350 2900 60  0000 C CNN
	1    8350 2900
	1    0    0    1   
$EndComp
$Comp
L DGND #PWR?
U 1 1 54B40B72
P 8000 3150
F 0 "#PWR?" H 8000 3150 40  0001 C CNN
F 1 "DGND" H 8000 3080 40  0000 C CNN
F 2 "" H 8000 3150 60  0000 C CNN
F 3 "" H 8000 3150 60  0000 C CNN
	1    8000 3150
	1    0    0    -1  
$EndComp
$Comp
L NRF24L01 P?
U 1 1 54B40DF0
P 8350 4900
F 0 "P?" H 8350 4950 60  0000 C CNN
F 1 "NRF24L01" H 8350 5100 60  0000 C CNN
F 2 "~" H 8350 4900 60  0000 C CNN
F 3 "~" H 8350 4900 60  0000 C CNN
	1    8350 4900
	0    1    1    0   
$EndComp
$Comp
L DGND #PWR?
U 1 1 54B413E4
P 7600 4650
F 0 "#PWR?" H 7600 4650 40  0001 C CNN
F 1 "DGND" H 7600 4580 40  0000 C CNN
F 2 "" H 7600 4650 60  0000 C CNN
F 3 "" H 7600 4650 60  0000 C CNN
	1    7600 4650
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 54B41442
P 7700 4450
F 0 "#PWR?" H 7700 4410 30  0001 C CNN
F 1 "+3.3V" H 7700 4560 30  0000 C CNN
F 2 "" H 7700 4450 60  0000 C CNN
F 3 "" H 7700 4450 60  0000 C CNN
	1    7700 4450
	1    0    0    -1  
$EndComp
$Comp
L RFID-RC522-MODULE P?
U 1 1 54B414A2
P 9150 3850
F 0 "P?" H 9150 3900 60  0000 C CNN
F 1 "RFID-RC522-MODULE" H 9100 4000 60  0000 C CNN
F 2 "~" H 9150 3850 60  0000 C CNN
F 3 "~" H 9150 3850 60  0000 C CNN
	1    9150 3850
	0    1    1    0   
$EndComp
$Comp
L CONN_1 P?
U 1 1 54B414B1
P 7450 3250
F 0 "P?" H 7530 3250 40  0000 L CNN
F 1 "CONN_1" H 7450 3305 30  0001 C CNN
F 2 "~" H 7450 3250 60  0000 C CNN
F 3 "~" H 7450 3250 60  0000 C CNN
	1    7450 3250
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P?
U 1 1 54B414C0
P 7450 3350
F 0 "P?" H 7530 3350 40  0000 L CNN
F 1 "CONN_1" H 7450 3405 30  0001 C CNN
F 2 "~" H 7450 3350 60  0000 C CNN
F 3 "~" H 7450 3350 60  0000 C CNN
	1    7450 3350
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 54B417F7
P 8400 4350
F 0 "#PWR?" H 8400 4310 30  0001 C CNN
F 1 "+3.3V" H 8400 4460 30  0000 C CNN
F 2 "" H 8400 4350 60  0000 C CNN
F 3 "" H 8400 4350 60  0000 C CNN
	1    8400 4350
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 54B4180B
P 8400 4200
F 0 "#PWR?" H 8400 4160 30  0001 C CNN
F 1 "+3.3V" H 8400 4310 30  0000 C CNN
F 2 "" H 8400 4200 60  0000 C CNN
F 3 "" H 8400 4200 60  0000 C CNN
	1    8400 4200
	0    -1   -1   0   
$EndComp
$Comp
L DGND #PWR?
U 1 1 54B418DD
P 8500 4100
F 0 "#PWR?" H 8500 4100 40  0001 C CNN
F 1 "DGND" H 8500 4030 40  0000 C CNN
F 2 "" H 8500 4100 60  0000 C CNN
F 3 "" H 8500 4100 60  0000 C CNN
	1    8500 4100
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P?
U 1 1 54B41A98
P 7200 6200
F 0 "P?" V 7150 6200 50  0000 C CNN
F 1 "USART3" V 7250 6200 50  0000 C CNN
F 2 "~" H 7200 6200 60  0000 C CNN
F 3 "~" H 7200 6200 60  0000 C CNN
	1    7200 6200
	0    -1   1    0   
$EndComp
$Comp
L DGND #PWR?
U 1 1 54B41FDB
P 6850 5950
F 0 "#PWR?" H 6850 5950 40  0001 C CNN
F 1 "DGND" H 6850 5880 40  0000 C CNN
F 2 "" H 6850 5950 60  0000 C CNN
F 3 "" H 6850 5950 60  0000 C CNN
	1    6850 5950
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 54B42056
P 7150 5700
F 0 "#PWR?" H 7150 5660 30  0001 C CNN
F 1 "+3.3V" H 7150 5810 30  0000 C CNN
F 2 "" H 7150 5700 60  0000 C CNN
F 3 "" H 7150 5700 60  0000 C CNN
	1    7150 5700
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P?
U 1 1 54B420D3
P 3750 1850
F 0 "P?" H 3830 1850 40  0000 L CNN
F 1 "CONN_1" H 3750 1905 30  0001 C CNN
F 2 "~" H 3750 1850 60  0000 C CNN
F 3 "~" H 3750 1850 60  0000 C CNN
	1    3750 1850
	-1   0    0    1   
$EndComp
$Comp
L CONN_6 P?
U 1 1 54B421B0
P 7750 2200
F 0 "P?" V 7700 2200 60  0000 C CNN
F 1 "SensorsIn" V 7800 2200 60  0000 C CNN
F 2 "~" H 7750 2200 60  0000 C CNN
F 3 "~" H 7750 2200 60  0000 C CNN
	1    7750 2200
	1    0    0    -1  
$EndComp
$Comp
L MOSFET_N Q?
U 1 1 54B4344A
P 9300 1900
F 0 "Q?" H 9310 2070 60  0000 R CNN
F 1 "MOSFET_N" H 9310 1750 60  0000 R CNN
F 2 "~" H 9300 1900 60  0000 C CNN
F 3 "~" H 9300 1900 60  0000 C CNN
	1    9300 1900
	1    0    0    -1  
$EndComp
$Comp
L MOSFET_N Q?
U 1 1 54B43459
P 9600 2350
F 0 "Q?" H 9610 2520 60  0000 R CNN
F 1 "MOSFET_N" H 9610 2200 60  0000 R CNN
F 2 "~" H 9600 2350 60  0000 C CNN
F 3 "~" H 9600 2350 60  0000 C CNN
	1    9600 2350
	1    0    0    -1  
$EndComp
$Comp
L MOSFET_N Q?
U 1 1 54B43468
P 9900 2800
F 0 "Q?" H 9910 2970 60  0000 R CNN
F 1 "MOSFET_N" H 9910 2650 60  0000 R CNN
F 2 "~" H 9900 2800 60  0000 C CNN
F 3 "~" H 9900 2800 60  0000 C CNN
	1    9900 2800
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR?
U 1 1 54B4367F
P 9400 2200
F 0 "#PWR?" H 9400 2200 40  0001 C CNN
F 1 "AGND" H 9400 2130 50  0000 C CNN
F 2 "" H 9400 2200 60  0000 C CNN
F 3 "" H 9400 2200 60  0000 C CNN
	1    9400 2200
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR?
U 1 1 54B4368E
P 9700 2650
F 0 "#PWR?" H 9700 2650 40  0001 C CNN
F 1 "AGND" H 9700 2580 50  0000 C CNN
F 2 "" H 9700 2650 60  0000 C CNN
F 3 "" H 9700 2650 60  0000 C CNN
	1    9700 2650
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR?
U 1 1 54B4369D
P 10000 3100
F 0 "#PWR?" H 10000 3100 40  0001 C CNN
F 1 "AGND" H 10000 3030 50  0000 C CNN
F 2 "" H 10000 3100 60  0000 C CNN
F 3 "" H 10000 3100 60  0000 C CNN
	1    10000 3100
	1    0    0    -1  
$EndComp
$Comp
L MICROSD-SDIO P?
U 1 1 54B55F8B
P 1500 4200
F 0 "P?" H 1500 4500 60  0000 C CNN
F 1 "MICROSD-SDIO" H 1500 4800 60  0000 C CNN
F 2 "~" H 1500 4200 60  0000 C CNN
F 3 "~" H 1500 4200 60  0000 C CNN
	1    1500 4200
	0    -1   1    0   
$EndComp
$Comp
L DGND #PWR?
U 1 1 54B560BD
P 1800 4950
F 0 "#PWR?" H 1800 4950 40  0001 C CNN
F 1 "DGND" H 1800 4880 40  0000 C CNN
F 2 "" H 1800 4950 60  0000 C CNN
F 3 "" H 1800 4950 60  0000 C CNN
	1    1800 4950
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 54B561D7
P 1750 5400
F 0 "#PWR?" H 1750 5360 30  0001 C CNN
F 1 "+3.3V" H 1750 5510 30  0000 C CNN
F 2 "" H 1750 5400 60  0000 C CNN
F 3 "" H 1750 5400 60  0000 C CNN
	1    1750 5400
	0    -1   -1   0   
$EndComp
$Comp
L R R?
U 1 1 54B564A5
P 2650 5350
F 0 "R?" V 2730 5350 40  0000 C CNN
F 1 "R" V 2657 5351 40  0000 C CNN
F 2 "~" V 2580 5350 30  0000 C CNN
F 3 "~" H 2650 5350 30  0000 C CNN
	1    2650 5350
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 54B564B4
P 2850 5350
F 0 "R?" V 2930 5350 40  0000 C CNN
F 1 "R" V 2857 5351 40  0000 C CNN
F 2 "~" V 2780 5350 30  0000 C CNN
F 3 "~" H 2850 5350 30  0000 C CNN
	1    2850 5350
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 54B564C3
P 3050 5350
F 0 "R?" V 3130 5350 40  0000 C CNN
F 1 "R" V 3057 5351 40  0000 C CNN
F 2 "~" V 2980 5350 30  0000 C CNN
F 3 "~" H 3050 5350 30  0000 C CNN
	1    3050 5350
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 54B564D2
P 3250 5350
F 0 "R?" V 3330 5350 40  0000 C CNN
F 1 "R" V 3257 5351 40  0000 C CNN
F 2 "~" V 3180 5350 30  0000 C CNN
F 3 "~" H 3250 5350 30  0000 C CNN
	1    3250 5350
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 54B56750
P 2950 5750
F 0 "#PWR?" H 2950 5710 30  0001 C CNN
F 1 "+3.3V" H 2950 5860 30  0000 C CNN
F 2 "" H 2950 5750 60  0000 C CNN
F 3 "" H 2950 5750 60  0000 C CNN
	1    2950 5750
	-1   0    0    1   
$EndComp
$Comp
L R R?
U 1 1 54B569F7
P 1900 3450
F 0 "R?" V 1980 3450 40  0000 C CNN
F 1 "R" V 1907 3451 40  0000 C CNN
F 2 "~" V 1830 3450 30  0000 C CNN
F 3 "~" H 1900 3450 30  0000 C CNN
	1    1900 3450
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 54B56A06
P 1550 3450
F 0 "#PWR?" H 1550 3410 30  0001 C CNN
F 1 "+3.3V" H 1550 3560 30  0000 C CNN
F 2 "" H 1550 3450 60  0000 C CNN
F 3 "" H 1550 3450 60  0000 C CNN
	1    1550 3450
	0    -1   -1   0   
$EndComp
$Comp
L R R?
U 1 1 54B56B62
P 9850 1600
F 0 "R?" V 9930 1600 40  0000 C CNN
F 1 "R" V 9857 1601 40  0000 C CNN
F 2 "~" V 9780 1600 30  0000 C CNN
F 3 "~" H 9850 1600 30  0000 C CNN
	1    9850 1600
	0    -1   -1   0   
$EndComp
$Comp
L R R?
U 1 1 54B56B71
P 10150 2050
F 0 "R?" V 10230 2050 40  0000 C CNN
F 1 "R" V 10157 2051 40  0000 C CNN
F 2 "~" V 10080 2050 30  0000 C CNN
F 3 "~" H 10150 2050 30  0000 C CNN
	1    10150 2050
	0    -1   -1   0   
$EndComp
$Comp
L R R?
U 1 1 54B56B8A
P 10300 2400
F 0 "R?" V 10380 2400 40  0000 C CNN
F 1 "R" V 10307 2401 40  0000 C CNN
F 2 "~" V 10230 2400 30  0000 C CNN
F 3 "~" H 10300 2400 30  0000 C CNN
	1    10300 2400
	0    -1   -1   0   
$EndComp
$Comp
L CONN_4 P?
U 1 1 54B56B99
P 11050 1750
F 0 "P?" V 11000 1750 50  0000 C CNN
F 1 "CONN_4" V 11100 1750 50  0000 C CNN
F 2 "~" H 11050 1750 60  0000 C CNN
F 3 "~" H 11050 1750 60  0000 C CNN
	1    11050 1750
	1    0    0    1   
$EndComp
$Comp
L MOSFET_N Q?
U 1 1 54B5710D
P 1000 1650
F 0 "Q?" H 1010 1820 60  0000 R CNN
F 1 "MOSFET_N" H 1010 1500 60  0000 R CNN
F 2 "~" H 1000 1650 60  0000 C CNN
F 3 "~" H 1000 1650 60  0000 C CNN
	1    1000 1650
	-1   0    0    -1  
$EndComp
$Comp
L MOSFET_N Q?
U 1 1 54B5711C
P 1500 1650
F 0 "Q?" H 1510 1820 60  0000 R CNN
F 1 "MOSFET_N" H 1510 1500 60  0000 R CNN
F 2 "~" H 1500 1650 60  0000 C CNN
F 3 "~" H 1500 1650 60  0000 C CNN
	1    1500 1650
	-1   0    0    -1  
$EndComp
$Comp
L MOSFET_N Q?
U 1 1 54B5712B
P 2050 1650
F 0 "Q?" H 2060 1820 60  0000 R CNN
F 1 "MOSFET_N" H 2060 1500 60  0000 R CNN
F 2 "~" H 2050 1650 60  0000 C CNN
F 3 "~" H 2050 1650 60  0000 C CNN
	1    2050 1650
	-1   0    0    -1  
$EndComp
$Comp
L MOSFET_N Q?
U 1 1 54B5713A
P 2500 1650
F 0 "Q?" H 2510 1820 60  0000 R CNN
F 1 "MOSFET_N" H 2510 1500 60  0000 R CNN
F 2 "~" H 2500 1650 60  0000 C CNN
F 3 "~" H 2500 1650 60  0000 C CNN
	1    2500 1650
	-1   0    0    -1  
$EndComp
$Comp
L MOSFET_N Q?
U 1 1 54B57149
P 2950 1650
F 0 "Q?" H 2960 1820 60  0000 R CNN
F 1 "MOSFET_N" H 2960 1500 60  0000 R CNN
F 2 "~" H 2950 1650 60  0000 C CNN
F 3 "~" H 2950 1650 60  0000 C CNN
	1    2950 1650
	-1   0    0    -1  
$EndComp
$Comp
L MOSFET_N Q?
U 1 1 54B57158
P 800 2500
F 0 "Q?" H 810 2670 60  0000 R CNN
F 1 "MOSFET_N" H 810 2350 60  0000 R CNN
F 2 "~" H 800 2500 60  0000 C CNN
F 3 "~" H 800 2500 60  0000 C CNN
	1    800  2500
	-1   0    0    -1  
$EndComp
$Comp
L AGND #PWR?
U 1 1 54B5717D
P 900 1950
F 0 "#PWR?" H 900 1950 40  0001 C CNN
F 1 "AGND" H 900 1880 50  0000 C CNN
F 2 "" H 900 1950 60  0000 C CNN
F 3 "" H 900 1950 60  0000 C CNN
	1    900  1950
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR?
U 1 1 54B5718C
P 1400 1950
F 0 "#PWR?" H 1400 1950 40  0001 C CNN
F 1 "AGND" H 1400 1880 50  0000 C CNN
F 2 "" H 1400 1950 60  0000 C CNN
F 3 "" H 1400 1950 60  0000 C CNN
	1    1400 1950
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR?
U 1 1 54B5719B
P 1950 1950
F 0 "#PWR?" H 1950 1950 40  0001 C CNN
F 1 "AGND" H 1950 1880 50  0000 C CNN
F 2 "" H 1950 1950 60  0000 C CNN
F 3 "" H 1950 1950 60  0000 C CNN
	1    1950 1950
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR?
U 1 1 54B571AA
P 2400 1950
F 0 "#PWR?" H 2400 1950 40  0001 C CNN
F 1 "AGND" H 2400 1880 50  0000 C CNN
F 2 "" H 2400 1950 60  0000 C CNN
F 3 "" H 2400 1950 60  0000 C CNN
	1    2400 1950
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR?
U 1 1 54B571B9
P 2850 1950
F 0 "#PWR?" H 2850 1950 40  0001 C CNN
F 1 "AGND" H 2850 1880 50  0000 C CNN
F 2 "" H 2850 1950 60  0000 C CNN
F 3 "" H 2850 1950 60  0000 C CNN
	1    2850 1950
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR?
U 1 1 54B571C8
P 700 2800
F 0 "#PWR?" H 700 2800 40  0001 C CNN
F 1 "AGND" H 700 2730 50  0000 C CNN
F 2 "" H 700 2800 60  0000 C CNN
F 3 "" H 700 2800 60  0000 C CNN
	1    700  2800
	1    0    0    -1  
$EndComp
$Comp
L CONN_6 P?
U 1 1 54B571D7
P 1900 700
F 0 "P?" V 1850 700 60  0000 C CNN
F 1 "Vibroengines" V 1950 700 60  0000 C CNN
F 2 "~" H 1900 700 60  0000 C CNN
F 3 "~" H 1900 700 60  0000 C CNN
	1    1900 700 
	0    1    -1   0   
$EndComp
$Comp
L STM32F103RE U?
U 1 1 54B3B040
P 5600 3550
F 0 "U?" H 4250 5500 50  0000 C CNN
F 1 "STM32F103RE" H 6750 1600 50  0000 C CNN
F 2 "LQFP64" H 5600 3550 50  0000 C CNN
F 3 "" H 5600 3850 60  0000 C CNN
	1    5600 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 5600 5150 5700
Wire Wire Line
	5150 5700 5900 5700
Wire Wire Line
	5900 5700 5900 5600
Wire Wire Line
	5600 5600 5600 5700
Connection ~ 5600 5700
Wire Wire Line
	5450 5600 5450 5700
Connection ~ 5450 5700
Wire Wire Line
	5300 5600 5300 5700
Connection ~ 5300 5700
Wire Wire Line
	5150 1500 5150 1400
Wire Wire Line
	5150 1400 5900 1400
Wire Wire Line
	5900 1400 5900 1500
Wire Wire Line
	5600 1500 5600 1400
Connection ~ 5600 1400
Wire Wire Line
	5450 1500 5450 1400
Connection ~ 5450 1400
Wire Wire Line
	5300 1500 5300 1400
Connection ~ 5300 1400
Wire Wire Line
	5750 5850 5750 5700
Connection ~ 5750 5700
Wire Wire Line
	5750 1300 5750 1400
Connection ~ 5750 1400
Wire Wire Line
	5300 7050 5300 6950
Wire Wire Line
	5300 6950 6300 6950
Wire Wire Line
	6050 6950 6050 7050
Wire Wire Line
	5800 6850 5800 7050
Connection ~ 5800 6950
Wire Wire Line
	5550 7050 5550 6950
Connection ~ 5550 6950
Wire Wire Line
	5300 7450 5300 7550
Wire Wire Line
	5300 7550 6300 7550
Wire Wire Line
	6050 7550 6050 7450
Wire Wire Line
	5800 7450 5800 7650
Connection ~ 5800 7550
Wire Wire Line
	5550 7450 5550 7550
Connection ~ 5550 7550
Wire Wire Line
	6300 6950 6300 7050
Connection ~ 6050 6950
Wire Wire Line
	6300 7550 6300 7450
Connection ~ 6050 7550
Wire Wire Line
	3400 2200 4050 2200
Wire Wire Line
	3700 2250 3700 2200
Connection ~ 3700 2200
Wire Wire Line
	3950 2350 4050 2350
Wire Wire Line
	3950 2350 3950 2900
Wire Wire Line
	3950 2900 3400 2900
Wire Wire Line
	3700 2850 3700 2900
Connection ~ 3700 2900
Wire Wire Line
	2900 2300 2900 2200
Wire Wire Line
	2900 2200 3000 2200
Wire Wire Line
	2900 3000 2900 2900
Wire Wire Line
	2900 2900 3000 2900
Wire Wire Line
	2200 6750 2500 6750
Wire Wire Line
	1600 7350 1600 7450
Wire Wire Line
	2400 7300 2400 7450
Wire Wire Line
	2400 6900 2400 6750
Connection ~ 2400 6750
Wire Wire Line
	7150 2850 8000 2850
Wire Wire Line
	7150 2950 7900 2950
Wire Wire Line
	7900 2950 7900 2750
Wire Wire Line
	7900 2750 8000 2750
Wire Wire Line
	8000 3150 8000 3050
Wire Wire Line
	7800 4850 7150 4850
Wire Wire Line
	7150 4950 7800 4950
Wire Wire Line
	7150 5050 7450 5050
Wire Wire Line
	7450 5050 7450 5100
Wire Wire Line
	7450 5100 7650 5100
Wire Wire Line
	7650 5100 7650 5150
Wire Wire Line
	7650 5150 7800 5150
Wire Wire Line
	7150 5150 7550 5150
Wire Wire Line
	7550 5150 7550 5050
Wire Wire Line
	7550 5050 7800 5050
Wire Wire Line
	7800 4750 7500 4750
Wire Wire Line
	7500 4750 7500 4350
Wire Wire Line
	7500 4350 7150 4350
Wire Wire Line
	7150 4450 7400 4450
Wire Wire Line
	7400 4450 7400 5250
Wire Wire Line
	7400 5250 7800 5250
Wire Wire Line
	7600 4650 7600 4550
Wire Wire Line
	7600 4550 7800 4550
Wire Wire Line
	7700 4450 7700 4650
Wire Wire Line
	7700 4650 7800 4650
Wire Wire Line
	7150 3250 7300 3250
Wire Wire Line
	7150 3350 7300 3350
Wire Wire Line
	7150 3450 7650 3450
Wire Wire Line
	7650 3450 7650 3300
Wire Wire Line
	7650 3300 8600 3300
Wire Wire Line
	7150 3950 7750 3950
Wire Wire Line
	7750 3950 7750 3450
Wire Wire Line
	7750 3450 8600 3450
Wire Wire Line
	7150 4050 7850 4050
Wire Wire Line
	7850 4050 7850 3750
Wire Wire Line
	7850 3750 8600 3750
Wire Wire Line
	7150 4150 7950 4150
Wire Wire Line
	7950 4150 7950 3600
Wire Wire Line
	7950 3600 8600 3600
Wire Wire Line
	8400 4200 8600 4200
Wire Wire Line
	8400 4350 8600 4350
Wire Wire Line
	8500 4100 8500 4050
Wire Wire Line
	8500 4050 8600 4050
Wire Wire Line
	7150 4250 8050 4250
Wire Wire Line
	8050 4250 8050 3900
Wire Wire Line
	8050 3900 8600 3900
Wire Wire Line
	7250 5850 7250 4650
Wire Wire Line
	7250 4650 7150 4650
Wire Wire Line
	7150 4750 7350 4750
Wire Wire Line
	7350 4750 7350 5850
Wire Wire Line
	6850 5950 6850 5850
Wire Wire Line
	6850 5850 7050 5850
Wire Wire Line
	7150 5700 7150 5850
Wire Wire Line
	3900 1850 4050 1850
Wire Wire Line
	7150 1950 7400 1950
Wire Wire Line
	7150 2050 7400 2050
Wire Wire Line
	7150 2150 7400 2150
Wire Wire Line
	7150 2250 7400 2250
Wire Wire Line
	7150 2350 7400 2350
Wire Wire Line
	7150 2450 7400 2450
Wire Wire Line
	7150 2550 8900 2550
Wire Wire Line
	8900 2550 8900 1900
Wire Wire Line
	8900 1900 9100 1900
Wire Wire Line
	7150 2650 9200 2650
Wire Wire Line
	9200 2650 9200 2350
Wire Wire Line
	9200 2350 9400 2350
Wire Wire Line
	7150 3650 8550 3650
Wire Wire Line
	8550 3650 8550 2800
Wire Wire Line
	8550 2800 9700 2800
Wire Wire Line
	9400 2200 9400 2100
Wire Wire Line
	9700 2550 9700 2650
Wire Wire Line
	10000 3000 10000 3100
Wire Wire Line
	4050 4450 2150 4450
Wire Wire Line
	4050 4550 2150 4550
Wire Wire Line
	2150 4250 2350 4250
Wire Wire Line
	2350 4250 2350 4850
Wire Wire Line
	2350 4850 4050 4850
Wire Wire Line
	2150 3950 2450 3950
Wire Wire Line
	2450 3950 2450 4750
Wire Wire Line
	2450 4750 4050 4750
Wire Wire Line
	2150 3850 2550 3850
Wire Wire Line
	2550 3850 2550 4650
Wire Wire Line
	2550 4650 4050 4650
Wire Wire Line
	2150 4050 2250 4050
Wire Wire Line
	2250 4050 2250 3150
Wire Wire Line
	2250 3150 3500 3150
Wire Wire Line
	3500 3150 3500 3050
Wire Wire Line
	3500 3050 4050 3050
Wire Wire Line
	2650 5100 2650 4750
Connection ~ 2650 4750
Wire Wire Line
	2850 5100 2850 4650
Connection ~ 2850 4650
Wire Wire Line
	3050 5100 3050 4550
Connection ~ 3050 4550
Wire Wire Line
	3250 5100 3250 4450
Connection ~ 3250 4450
Wire Wire Line
	2650 5600 2650 5700
Wire Wire Line
	2650 5700 3250 5700
Wire Wire Line
	3250 5700 3250 5600
Wire Wire Line
	2950 5750 2950 5700
Connection ~ 2950 5700
Wire Wire Line
	3050 5600 3050 5700
Connection ~ 3050 5700
Wire Wire Line
	2850 5600 2850 5700
Connection ~ 2850 5700
Wire Wire Line
	1550 3450 1650 3450
Wire Wire Line
	2150 3450 2250 3450
Connection ~ 2250 3450
Wire Wire Line
	10000 2600 10000 2400
Wire Wire Line
	10000 2400 10050 2400
Wire Wire Line
	10550 2400 10600 2400
Wire Wire Line
	10600 2400 10600 1900
Wire Wire Line
	10600 1900 10700 1900
Wire Wire Line
	9700 2150 9700 2050
Wire Wire Line
	9700 2050 9900 2050
Wire Wire Line
	9400 1700 9400 1600
Wire Wire Line
	9400 1600 9600 1600
Wire Wire Line
	10400 2050 10500 2050
Wire Wire Line
	10500 2050 10500 1800
Wire Wire Line
	10500 1800 10700 1800
Wire Wire Line
	10100 1600 10300 1600
Wire Wire Line
	10300 1600 10300 1700
Wire Wire Line
	10300 1700 10700 1700
Wire Wire Line
	2850 1950 2850 1850
Wire Wire Line
	2400 1950 2400 1850
Wire Wire Line
	1950 1950 1950 1850
Wire Wire Line
	1400 1950 1400 1850
Wire Wire Line
	900  1950 900  1850
Wire Wire Line
	700  2800 700  2700
Wire Wire Line
	700  2300 700  1100
Wire Wire Line
	700  1100 1650 1100
Wire Wire Line
	1650 1100 1650 1050
Wire Wire Line
	900  1450 900  1200
Wire Wire Line
	900  1200 1750 1200
Wire Wire Line
	1750 1200 1750 1050
Wire Wire Line
	1400 1450 1400 1300
Wire Wire Line
	1400 1300 1850 1300
Wire Wire Line
	1850 1300 1850 1050
Wire Wire Line
	1950 1050 1950 1450
Wire Wire Line
	2050 1050 2050 1300
Wire Wire Line
	2050 1300 2400 1300
Wire Wire Line
	2400 1300 2400 1450
Wire Wire Line
	2150 1050 2150 1200
Wire Wire Line
	2150 1200 2850 1200
Wire Wire Line
	2850 1200 2850 1450
Wire Wire Line
	4050 4150 2600 4150
Wire Wire Line
	2600 4150 2600 3400
Wire Wire Line
	2600 3400 1100 3400
Wire Wire Line
	1100 3400 1100 2500
Wire Wire Line
	1100 2500 1000 2500
Wire Wire Line
	4050 4050 2650 4050
Wire Wire Line
	2650 4050 2650 3350
Wire Wire Line
	2650 3350 1300 3350
Wire Wire Line
	1300 3350 1300 1650
Wire Wire Line
	1300 1650 1200 1650
Wire Wire Line
	1700 1650 1800 1650
Wire Wire Line
	1800 1650 1800 3300
Wire Wire Line
	1800 3300 2700 3300
Wire Wire Line
	2700 3300 2700 3950
Wire Wire Line
	2700 3950 4050 3950
Wire Wire Line
	4050 3850 2750 3850
Wire Wire Line
	2750 3850 2750 3250
Wire Wire Line
	2750 3250 2300 3250
Wire Wire Line
	2300 3250 2300 1650
Wire Wire Line
	2300 1650 2250 1650
Wire Wire Line
	2700 1650 2700 3200
Wire Wire Line
	2700 3200 2800 3200
Wire Wire Line
	2800 3200 2800 3750
Wire Wire Line
	2800 3750 4050 3750
Wire Wire Line
	3150 1650 3450 1650
Wire Wire Line
	3450 1650 3450 3650
Wire Wire Line
	3450 3650 4050 3650
$Comp
L AGND #PWR?
U 1 1 54B58CD7
P 3500 6750
F 0 "#PWR?" H 3500 6750 40  0001 C CNN
F 1 "AGND" H 3500 6680 50  0000 C CNN
F 2 "" H 3500 6750 60  0000 C CNN
F 3 "" H 3500 6750 60  0000 C CNN
	1    3500 6750
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR?
U 1 1 54B58CE6
P 3800 6750
F 0 "#PWR?" H 3800 6750 40  0001 C CNN
F 1 "DGND" H 3800 6680 40  0000 C CNN
F 2 "" H 3800 6750 60  0000 C CNN
F 3 "" H 3800 6750 60  0000 C CNN
	1    3800 6750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 6750 3500 6550
Wire Wire Line
	3500 6550 4100 6550
Wire Wire Line
	3800 6550 3800 6750
$Comp
L GND #PWR?
U 1 1 54B58DDF
P 4100 6750
F 0 "#PWR?" H 4100 6750 30  0001 C CNN
F 1 "GND" H 4100 6680 30  0001 C CNN
F 2 "" H 4100 6750 60  0000 C CNN
F 3 "" H 4100 6750 60  0000 C CNN
	1    4100 6750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 6550 4100 6750
Connection ~ 3800 6550
$Comp
L +BATT #PWR?
U 1 1 54B58EDA
P 900 6750
F 0 "#PWR?" H 900 6700 20  0001 C CNN
F 1 "+BATT" H 900 6850 30  0000 C CNN
F 2 "" H 900 6750 60  0000 C CNN
F 3 "" H 900 6750 60  0000 C CNN
	1    900  6750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	900  6750 1050 6750
$Comp
L C C?
U 1 1 54B58B57
P 2000 5100
F 0 "C?" H 2000 5200 40  0000 L CNN
F 1 "C" H 2006 5015 40  0000 L CNN
F 2 "~" H 2038 4950 30  0000 C CNN
F 3 "~" H 2000 5100 60  0000 C CNN
	1    2000 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 4350 2200 4350
Wire Wire Line
	2200 4350 2200 4850
Wire Wire Line
	2200 4850 1800 4850
Wire Wire Line
	1800 4850 1800 4950
Wire Wire Line
	2000 4900 2000 4850
Connection ~ 2000 4850
Wire Wire Line
	2150 4150 2250 4150
Wire Wire Line
	2250 4150 2250 5400
Wire Wire Line
	2250 5400 1750 5400
Wire Wire Line
	2000 5300 2000 5400
Connection ~ 2000 5400
$Comp
L CONN_6 P?
U 1 1 54B638F6
P 7750 1200
F 0 "P?" V 7700 1200 60  0000 C CNN
F 1 "SensorsVcc" V 7800 1200 60  0000 C CNN
F 2 "~" H 7750 1200 60  0000 C CNN
F 3 "~" H 7750 1200 60  0000 C CNN
	1    7750 1200
	1    0    0    -1  
$EndComp
$Comp
L CONN_6 P?
U 1 1 54B63911
P 8600 1200
F 0 "P?" V 8550 1200 60  0000 C CNN
F 1 "SensorsGND" V 8650 1200 60  0000 C CNN
F 2 "~" H 8600 1200 60  0000 C CNN
F 3 "~" H 8600 1200 60  0000 C CNN
	1    8600 1200
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 54B63936
P 7250 1200
F 0 "#PWR?" H 7250 1160 30  0001 C CNN
F 1 "+3.3V" H 7250 1310 30  0000 C CNN
F 2 "" H 7250 1200 60  0000 C CNN
F 3 "" H 7250 1200 60  0000 C CNN
	1    7250 1200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7400 1450 7350 1450
Wire Wire Line
	7350 1450 7350 950 
Wire Wire Line
	7350 950  7400 950 
Wire Wire Line
	7400 1050 7350 1050
Connection ~ 7350 1050
Wire Wire Line
	7400 1150 7350 1150
Connection ~ 7350 1150
Wire Wire Line
	7400 1250 7350 1250
Connection ~ 7350 1250
Wire Wire Line
	7400 1350 7350 1350
Connection ~ 7350 1350
Wire Wire Line
	7250 1200 7350 1200
Connection ~ 7350 1200
$Comp
L DGND #PWR?
U 1 1 54B63F33
P 8150 1550
F 0 "#PWR?" H 8150 1550 40  0001 C CNN
F 1 "DGND" H 8150 1480 40  0000 C CNN
F 2 "" H 8150 1550 60  0000 C CNN
F 3 "" H 8150 1550 60  0000 C CNN
	1    8150 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 1550 8150 950 
Wire Wire Line
	8150 950  8250 950 
Wire Wire Line
	8250 1050 8150 1050
Connection ~ 8150 1050
Wire Wire Line
	8250 1150 8150 1150
Connection ~ 8150 1150
Wire Wire Line
	8250 1250 8150 1250
Connection ~ 8150 1250
Wire Wire Line
	8250 1350 8150 1350
Connection ~ 8150 1350
Wire Wire Line
	8250 1450 8150 1450
Connection ~ 8150 1450
$EndSCHEMATC

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
LIBS:stm32
LIBS:linStab-(1-GND,Adj,2-Out,3-in)
LIBS:rfid-rc522-module
LIBS:nrf24l01
LIBS:microsd-sdio
LIBS:rifle-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date "19 dec 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L STM32F103RE U3
U 1 1 548EF455
P 5100 3800
F 0 "U3" H 3750 5750 50  0000 C CNN
F 1 "STM32F103RE" H 6250 1850 50  0000 C CNN
F 2 "LQFP64" H 5100 3800 50  0000 C CNN
F 3 "" H 5100 4100 60  0000 C CNN
	1    5100 3800
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR01
U 1 1 548EF483
P 4650 1600
F 0 "#PWR01" H 4650 1560 30  0001 C CNN
F 1 "+3.3V" H 4650 1710 30  0000 C CNN
F 2 "" H 4650 1600 60  0000 C CNN
F 3 "" H 4650 1600 60  0000 C CNN
	1    4650 1600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 548EF50D
P 5400 6000
F 0 "#PWR02" H 5400 6000 30  0001 C CNN
F 1 "GND" H 5400 5930 30  0001 C CNN
F 2 "" H 5400 6000 60  0000 C CNN
F 3 "" H 5400 6000 60  0000 C CNN
	1    5400 6000
	1    0    0    -1  
$EndComp
$Comp
L LINSTAB-(1-GND/ADJ,2-OUT,3-IN) U1
U 1 1 548EFA16
P 1750 6750
F 0 "U1" H 1750 6900 60  0000 C CNN
F 1 "3,3 V LDO" H 1800 7100 60  0000 C CNN
F 2 "~" H 1750 6750 60  0000 C CNN
F 3 "~" H 1750 6750 60  0000 C CNN
	1    1750 6750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR03
U 1 1 548EFA25
P 1750 7500
F 0 "#PWR03" H 1750 7500 30  0001 C CNN
F 1 "GND" H 1750 7430 30  0001 C CNN
F 2 "" H 1750 7500 60  0000 C CNN
F 3 "" H 1750 7500 60  0000 C CNN
	1    1750 7500
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR04
U 1 1 548EFA55
P 2500 6750
F 0 "#PWR04" H 2500 6710 30  0001 C CNN
F 1 "+3.3V" H 2500 6860 30  0000 C CNN
F 2 "" H 2500 6750 60  0000 C CNN
F 3 "" H 2500 6750 60  0000 C CNN
	1    2500 6750
	0    1    1    0   
$EndComp
$Comp
L CP1 C2
U 1 1 548EFA93
P 2450 7100
F 0 "C2" H 2500 7200 50  0000 L CNN
F 1 "10uF" H 2500 7000 50  0000 L CNN
F 2 "~" H 2450 7100 60  0000 C CNN
F 3 "~" H 2450 7100 60  0000 C CNN
	1    2450 7100
	1    0    0    -1  
$EndComp
$Comp
L CP1 C1
U 1 1 548EFAB9
P 1100 7100
F 0 "C1" H 1150 7200 50  0000 L CNN
F 1 "470uF" H 1150 7000 50  0000 L CNN
F 2 "~" H 1100 7100 60  0000 C CNN
F 3 "~" H 1100 7100 60  0000 C CNN
	1    1100 7100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 548EFAC8
P 2450 7500
F 0 "#PWR05" H 2450 7500 30  0001 C CNN
F 1 "GND" H 2450 7430 30  0001 C CNN
F 2 "" H 2450 7500 60  0000 C CNN
F 3 "" H 2450 7500 60  0000 C CNN
	1    2450 7500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 548EFAD7
P 1100 7500
F 0 "#PWR06" H 1100 7500 30  0001 C CNN
F 1 "GND" H 1100 7430 30  0001 C CNN
F 2 "" H 1100 7500 60  0000 C CNN
F 3 "" H 1100 7500 60  0000 C CNN
	1    1100 7500
	1    0    0    -1  
$EndComp
$Comp
L CRYSTAL X1
U 1 1 548EFB83
P 2900 2600
F 0 "X1" H 2900 2750 60  0000 C CNN
F 1 "8MHz" H 2900 2450 60  0000 C CNN
F 2 "~" H 2900 2600 60  0000 C CNN
F 3 "~" H 2900 2600 60  0000 C CNN
	1    2900 2600
	-1   0    0    1   
$EndComp
$Comp
L C C4
U 1 1 548EFBB0
P 3250 2850
F 0 "C4" H 3250 2950 40  0000 L CNN
F 1 "20pF" H 3256 2765 40  0000 L CNN
F 2 "~" H 3288 2700 30  0000 C CNN
F 3 "~" H 3250 2850 60  0000 C CNN
	1    3250 2850
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 548EFBBF
P 2550 2850
F 0 "C3" H 2550 2950 40  0000 L CNN
F 1 "20pF" H 2556 2765 40  0000 L CNN
F 2 "~" H 2588 2700 30  0000 C CNN
F 3 "~" H 2550 2850 60  0000 C CNN
	1    2550 2850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR07
U 1 1 548EFBCE
P 2550 3100
F 0 "#PWR07" H 2550 3100 30  0001 C CNN
F 1 "GND" H 2550 3030 30  0001 C CNN
F 2 "" H 2550 3100 60  0000 C CNN
F 3 "" H 2550 3100 60  0000 C CNN
	1    2550 3100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 548EFC6E
P 3250 3100
F 0 "#PWR08" H 3250 3100 30  0001 C CNN
F 1 "GND" H 3250 3030 30  0001 C CNN
F 2 "" H 3250 3100 60  0000 C CNN
F 3 "" H 3250 3100 60  0000 C CNN
	1    3250 3100
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 548EFCEA
P 5200 7150
F 0 "C6" H 5200 7250 40  0000 L CNN
F 1 "100n" H 5206 7065 40  0000 L CNN
F 2 "~" H 5238 7000 30  0000 C CNN
F 3 "~" H 5200 7150 60  0000 C CNN
	1    5200 7150
	1    0    0    -1  
$EndComp
$Comp
L C C7
U 1 1 548EFCF9
P 5400 7150
F 0 "C7" H 5400 7250 40  0000 L CNN
F 1 "100n" H 5406 7065 40  0000 L CNN
F 2 "~" H 5438 7000 30  0000 C CNN
F 3 "~" H 5400 7150 60  0000 C CNN
	1    5400 7150
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 548EFD08
P 5600 7150
F 0 "C8" H 5600 7250 40  0000 L CNN
F 1 "100n" H 5606 7065 40  0000 L CNN
F 2 "~" H 5638 7000 30  0000 C CNN
F 3 "~" H 5600 7150 60  0000 C CNN
	1    5600 7150
	1    0    0    -1  
$EndComp
$Comp
L C C9
U 1 1 548EFD17
P 5800 7150
F 0 "C9" H 5800 7250 40  0000 L CNN
F 1 "100n" H 5806 7065 40  0000 L CNN
F 2 "~" H 5838 7000 30  0000 C CNN
F 3 "~" H 5800 7150 60  0000 C CNN
	1    5800 7150
	1    0    0    -1  
$EndComp
$Comp
L C C10
U 1 1 548EFD26
P 6000 7150
F 0 "C10" H 6000 7250 40  0000 L CNN
F 1 "100n" H 6006 7065 40  0000 L CNN
F 2 "~" H 6038 7000 30  0000 C CNN
F 3 "~" H 6000 7150 60  0000 C CNN
	1    6000 7150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 548EFECF
P 5600 7500
F 0 "#PWR09" H 5600 7500 30  0001 C CNN
F 1 "GND" H 5600 7430 30  0001 C CNN
F 2 "" H 5600 7500 60  0000 C CNN
F 3 "" H 5600 7500 60  0000 C CNN
	1    5600 7500
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR010
U 1 1 548EFEDE
P 5600 6800
F 0 "#PWR010" H 5600 6760 30  0001 C CNN
F 1 "+3.3V" H 5600 6910 30  0000 C CNN
F 2 "" H 5600 6800 60  0000 C CNN
F 3 "" H 5600 6800 60  0000 C CNN
	1    5600 6800
	1    0    0    -1  
$EndComp
Text Notes 5050 6450 0    60   ~ 0
Should be placed near\nMCU power pins
$Comp
L CONN_1 P6
U 1 1 548EFFD9
P 7000 3500
F 0 "P6" H 7080 3500 40  0000 L CNN
F 1 "SWD_IO" H 7000 3555 30  0001 C CNN
F 2 "~" H 7000 3500 60  0000 C CNN
F 3 "~" H 7000 3500 60  0000 C CNN
	1    7000 3500
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P7
U 1 1 548EFFE8
P 7000 3600
F 0 "P7" H 7080 3600 40  0000 L CNN
F 1 "SWD_CLK" H 7000 3655 30  0001 C CNN
F 2 "~" H 7000 3600 60  0000 C CNN
F 3 "~" H 7000 3600 60  0000 C CNN
	1    7000 3600
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P3
U 1 1 548F46F6
P 3250 2100
F 0 "P3" H 3330 2100 40  0000 L CNN
F 1 "Reset" H 3250 2155 30  0001 C CNN
F 2 "~" H 3250 2100 60  0000 C CNN
F 3 "~" H 3250 2100 60  0000 C CNN
	1    3250 2100
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR011
U 1 1 5490A7C3
P 7650 3400
F 0 "#PWR011" H 7650 3400 30  0001 C CNN
F 1 "GND" H 7650 3330 30  0001 C CNN
F 2 "" H 7650 3400 60  0000 C CNN
F 3 "" H 7650 3400 60  0000 C CNN
	1    7650 3400
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P9
U 1 1 5490A87A
P 8100 3150
F 0 "P9" V 8050 3150 50  0000 C CNN
F 1 "USART1" V 8150 3150 50  0000 C CNN
F 2 "~" H 8100 3150 60  0000 C CNN
F 3 "~" H 8100 3150 60  0000 C CNN
	1    8100 3150
	1    0    0    1   
$EndComp
$Comp
L CONN_4 P8
U 1 1 5490AA64
P 8100 2450
F 0 "P8" V 8050 2450 50  0000 C CNN
F 1 "USART2" V 8150 2450 50  0000 C CNN
F 2 "~" H 8100 2450 60  0000 C CNN
F 3 "~" H 8100 2450 60  0000 C CNN
	1    8100 2450
	1    0    0    1   
$EndComp
$Comp
L +3.3V #PWR012
U 1 1 5490AB5B
P 7650 2500
F 0 "#PWR012" H 7650 2460 30  0001 C CNN
F 1 "+3.3V" H 7650 2610 30  0000 C CNN
F 2 "" H 7650 2500 60  0000 C CNN
F 3 "" H 7650 2500 60  0000 C CNN
	1    7650 2500
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR013
U 1 1 5490AC63
P 7650 2700
F 0 "#PWR013" H 7650 2700 30  0001 C CNN
F 1 "GND" H 7650 2630 30  0001 C CNN
F 2 "" H 7650 2700 60  0000 C CNN
F 3 "" H 7650 2700 60  0000 C CNN
	1    7650 2700
	1    0    0    -1  
$EndComp
$Comp
L RFID-RC522-MODULE P10
U 1 1 5490ADB4
P 8700 4400
F 0 "P10" H 8700 4450 60  0000 C CNN
F 1 "RFID-RC522-MODULE" H 8650 4550 60  0000 C CNN
F 2 "~" H 8700 4400 60  0000 C CNN
F 3 "~" H 8700 4400 60  0000 C CNN
	1    8700 4400
	0    1    1    0   
$EndComp
$Comp
L +3.3V #PWR014
U 1 1 5490B041
P 7950 4750
F 0 "#PWR014" H 7950 4710 30  0001 C CNN
F 1 "+3.3V" H 7950 4860 30  0000 C CNN
F 2 "" H 7950 4750 60  0000 C CNN
F 3 "" H 7950 4750 60  0000 C CNN
	1    7950 4750
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #PWR015
U 1 1 5490B055
P 7950 4900
F 0 "#PWR015" H 7950 4860 30  0001 C CNN
F 1 "+3.3V" H 7950 5010 30  0000 C CNN
F 2 "" H 7950 4900 60  0000 C CNN
F 3 "" H 7950 4900 60  0000 C CNN
	1    7950 4900
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR016
U 1 1 5490B069
P 8050 4650
F 0 "#PWR016" H 8050 4650 30  0001 C CNN
F 1 "GND" H 8050 4580 30  0001 C CNN
F 2 "" H 8050 4650 60  0000 C CNN
F 3 "" H 8050 4650 60  0000 C CNN
	1    8050 4650
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P5
U 1 1 5490B292
P 6700 6550
F 0 "P5" V 6650 6550 50  0000 C CNN
F 1 "CONN_4" V 6750 6550 50  0000 C CNN
F 2 "~" H 6700 6550 60  0000 C CNN
F 3 "~" H 6700 6550 60  0000 C CNN
	1    6700 6550
	0    -1   1    0   
$EndComp
$Comp
L GND #PWR017
U 1 1 5490B4F1
P 5850 6200
F 0 "#PWR017" H 5850 6200 30  0001 C CNN
F 1 "GND" H 5850 6130 30  0001 C CNN
F 2 "" H 5850 6200 60  0000 C CNN
F 3 "" H 5850 6200 60  0000 C CNN
	1    5850 6200
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR018
U 1 1 5490B599
P 6650 6050
F 0 "#PWR018" H 6650 6010 30  0001 C CNN
F 1 "+3.3V" H 6650 6160 30  0000 C CNN
F 2 "" H 6650 6050 60  0000 C CNN
F 3 "" H 6650 6050 60  0000 C CNN
	1    6650 6050
	1    0    0    -1  
$EndComp
$Comp
L NRF24L01 P11
U 1 1 5490B7E2
P 8700 5600
F 0 "P11" H 8700 5650 60  0000 C CNN
F 1 "NRF24L01" H 8700 5800 60  0000 C CNN
F 2 "~" H 8700 5600 60  0000 C CNN
F 3 "~" H 8700 5600 60  0000 C CNN
	1    8700 5600
	0    1    1    0   
$EndComp
$Comp
L +3.3V #PWR019
U 1 1 5490BBDA
P 9300 4700
F 0 "#PWR019" H 9300 4660 30  0001 C CNN
F 1 "+3.3V" H 9300 4810 30  0000 C CNN
F 2 "" H 9300 4700 60  0000 C CNN
F 3 "" H 9300 4700 60  0000 C CNN
	1    9300 4700
	1    0    0    -1  
$EndComp
$Comp
L MOS_N Q3
U 1 1 5490BD7F
P 9550 3050
F 0 "Q3" H 9560 3220 60  0000 R CNN
F 1 "MOS_N" H 9560 2900 60  0000 R CNN
F 2 "~" H 9550 3050 60  0000 C CNN
F 3 "~" H 9550 3050 60  0000 C CNN
	1    9550 3050
	1    0    0    -1  
$EndComp
$Comp
L MOS_N Q4
U 1 1 5490BD8E
P 10150 3050
F 0 "Q4" H 10160 3220 60  0000 R CNN
F 1 "MOS_N" H 10160 2900 60  0000 R CNN
F 2 "~" H 10150 3050 60  0000 C CNN
F 3 "~" H 10150 3050 60  0000 C CNN
	1    10150 3050
	1    0    0    -1  
$EndComp
$Comp
L MOS_N Q5
U 1 1 5490BD9D
P 10750 3050
F 0 "Q5" H 10760 3220 60  0000 R CNN
F 1 "MOS_N" H 10760 2900 60  0000 R CNN
F 2 "~" H 10750 3050 60  0000 C CNN
F 3 "~" H 10750 3050 60  0000 C CNN
	1    10750 3050
	1    0    0    -1  
$EndComp
$Comp
L MOS_N Q2
U 1 1 5490BDB6
P 8900 3050
F 0 "Q2" H 8910 3220 60  0000 R CNN
F 1 "MOS_N" H 8910 2900 60  0000 R CNN
F 2 "~" H 8900 3050 60  0000 C CNN
F 3 "~" H 8900 3050 60  0000 C CNN
	1    8900 3050
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 5490BDC5
P 9000 2450
F 0 "R1" V 9080 2450 40  0000 C CNN
F 1 "R" V 9007 2451 40  0000 C CNN
F 2 "~" V 8930 2450 30  0000 C CNN
F 3 "~" H 9000 2450 30  0000 C CNN
	1    9000 2450
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 5490BDD4
P 9650 2450
F 0 "R2" V 9730 2450 40  0000 C CNN
F 1 "R" V 9657 2451 40  0000 C CNN
F 2 "~" V 9580 2450 30  0000 C CNN
F 3 "~" H 9650 2450 30  0000 C CNN
	1    9650 2450
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 5490BDE3
P 10250 2450
F 0 "R3" V 10330 2450 40  0000 C CNN
F 1 "R" V 10257 2451 40  0000 C CNN
F 2 "~" V 10180 2450 30  0000 C CNN
F 3 "~" H 10250 2450 30  0000 C CNN
	1    10250 2450
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 5490BDF2
P 10850 2450
F 0 "R4" V 10930 2450 40  0000 C CNN
F 1 "R" V 10857 2451 40  0000 C CNN
F 2 "~" V 10780 2450 30  0000 C CNN
F 3 "~" H 10850 2450 30  0000 C CNN
	1    10850 2450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR020
U 1 1 5490C0FA
P 9000 3400
F 0 "#PWR020" H 9000 3400 30  0001 C CNN
F 1 "GND" H 9000 3330 30  0001 C CNN
F 2 "" H 9000 3400 60  0000 C CNN
F 3 "" H 9000 3400 60  0000 C CNN
	1    9000 3400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR021
U 1 1 5490C109
P 9650 3400
F 0 "#PWR021" H 9650 3400 30  0001 C CNN
F 1 "GND" H 9650 3330 30  0001 C CNN
F 2 "" H 9650 3400 60  0000 C CNN
F 3 "" H 9650 3400 60  0000 C CNN
	1    9650 3400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR022
U 1 1 5490C118
P 10250 3400
F 0 "#PWR022" H 10250 3400 30  0001 C CNN
F 1 "GND" H 10250 3330 30  0001 C CNN
F 2 "" H 10250 3400 60  0000 C CNN
F 3 "" H 10250 3400 60  0000 C CNN
	1    10250 3400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR023
U 1 1 5490C127
P 10850 3400
F 0 "#PWR023" H 10850 3400 30  0001 C CNN
F 1 "GND" H 10850 3330 30  0001 C CNN
F 2 "" H 10850 3400 60  0000 C CNN
F 3 "" H 10850 3400 60  0000 C CNN
	1    10850 3400
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR024
U 1 1 5490C3A8
P 9850 1600
F 0 "#PWR024" H 9850 1690 20  0001 C CNN
F 1 "+5V" H 9850 1690 30  0000 C CNN
F 2 "" H 9850 1600 60  0000 C CNN
F 3 "" H 9850 1600 60  0000 C CNN
	1    9850 1600
	1    0    0    -1  
$EndComp
$Comp
L CONN_2 P13
U 1 1 5490C903
P 10300 1750
F 0 "P13" V 10250 1750 40  0000 C CNN
F 1 "IR_LED" V 10350 1750 40  0000 C CNN
F 2 "~" H 10300 1750 60  0000 C CNN
F 3 "~" H 10300 1750 60  0000 C CNN
	1    10300 1750
	1    0    0    1   
$EndComp
$Comp
L CONN_3 K3
U 1 1 5490CAA5
P 9200 1750
F 0 "K3" V 9150 1750 50  0000 C CNN
F 1 "IR_LASER" V 9250 1750 40  0000 C CNN
F 2 "~" H 9200 1750 60  0000 C CNN
F 3 "~" H 9200 1750 60  0000 C CNN
	1    9200 1750
	1    0    0    1   
$EndComp
$Comp
L +5V #PWR025
U 1 1 5490CABE
P 8700 1850
F 0 "#PWR025" H 8700 1940 20  0001 C CNN
F 1 "+5V" H 8700 1940 30  0000 C CNN
F 2 "" H 8700 1850 60  0000 C CNN
F 3 "" H 8700 1850 60  0000 C CNN
	1    8700 1850
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR026
U 1 1 5490CBA5
P 8500 1900
F 0 "#PWR026" H 8500 1900 30  0001 C CNN
F 1 "GND" H 8500 1830 30  0001 C CNN
F 2 "" H 8500 1900 60  0000 C CNN
F 3 "" H 8500 1900 60  0000 C CNN
	1    8500 1900
	1    0    0    -1  
$EndComp
$Comp
L CONN_5X2 P1
U 1 1 5491CD94
P 1400 3000
F 0 "P1" H 1400 3300 60  0000 C CNN
F 1 "Indication" V 1400 3000 50  0000 C CNN
F 2 "~" H 1400 3000 60  0000 C CNN
F 3 "~" H 1400 3000 60  0000 C CNN
	1    1400 3000
	-1   0    0    1   
$EndComp
$Comp
L MICROSD-SDIO P2
U 1 1 5491D40F
P 1500 5250
F 0 "P2" H 1500 5550 60  0000 C CNN
F 1 "MICROSD-SDIO" H 1500 5850 60  0000 C CNN
F 2 "~" H 1500 5250 60  0000 C CNN
F 3 "~" H 1500 5250 60  0000 C CNN
	1    1500 5250
	0    -1   1    0   
$EndComp
$Comp
L +3.3V #PWR027
U 1 1 5491DA7B
P 3050 4750
F 0 "#PWR027" H 3050 4710 30  0001 C CNN
F 1 "+3.3V" H 3050 4860 30  0000 C CNN
F 2 "" H 3050 4750 60  0000 C CNN
F 3 "" H 3050 4750 60  0000 C CNN
	1    3050 4750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR028
U 1 1 5491DB66
P 3050 5750
F 0 "#PWR028" H 3050 5750 30  0001 C CNN
F 1 "GND" H 3050 5680 30  0001 C CNN
F 2 "" H 3050 5750 60  0000 C CNN
F 3 "" H 3050 5750 60  0000 C CNN
	1    3050 5750
	1    0    0    -1  
$EndComp
$Comp
L LINSTAB-(1-GND/ADJ,2-OUT,3-IN) U2
U 1 1 5491DD91
P 3700 6750
F 0 "U2" H 3700 6900 60  0000 C CNN
F 1 "5,0 V LDO" H 3750 7100 60  0000 C CNN
F 2 "~" H 3700 6750 60  0000 C CNN
F 3 "~" H 3700 6750 60  0000 C CNN
	1    3700 6750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR029
U 1 1 5491DD97
P 3700 7500
F 0 "#PWR029" H 3700 7500 30  0001 C CNN
F 1 "GND" H 3700 7430 30  0001 C CNN
F 2 "" H 3700 7500 60  0000 C CNN
F 3 "" H 3700 7500 60  0000 C CNN
	1    3700 7500
	1    0    0    -1  
$EndComp
$Comp
L CP1 C5
U 1 1 5491DDA3
P 4400 7100
F 0 "C5" H 4450 7200 50  0000 L CNN
F 1 "470uF" H 4450 7000 50  0000 L CNN
F 2 "~" H 4400 7100 60  0000 C CNN
F 3 "~" H 4400 7100 60  0000 C CNN
	1    4400 7100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR030
U 1 1 5491DDAF
P 4400 7500
F 0 "#PWR030" H 4400 7500 30  0001 C CNN
F 1 "GND" H 4400 7430 30  0001 C CNN
F 2 "" H 4400 7500 60  0000 C CNN
F 3 "" H 4400 7500 60  0000 C CNN
	1    4400 7500
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR031
U 1 1 5491DDBB
P 2900 6050
F 0 "#PWR031" H 2900 6000 20  0001 C CNN
F 1 "+BATT" H 2900 6150 30  0000 C CNN
F 2 "" H 2900 6050 60  0000 C CNN
F 3 "" H 2900 6050 60  0000 C CNN
	1    2900 6050
	1    0    0    -1  
$EndComp
$Comp
L CONN_3 K2
U 1 1 5491E3E2
P 8100 1900
F 0 "K2" V 8050 1900 50  0000 C CNN
F 1 "Sound" V 8150 1900 40  0000 C CNN
F 2 "~" H 8100 1900 60  0000 C CNN
F 3 "~" H 8100 1900 60  0000 C CNN
	1    8100 1900
	1    0    0    1   
$EndComp
$Comp
L GND #PWR032
U 1 1 5491E3FB
P 7650 2100
F 0 "#PWR032" H 7650 2100 30  0001 C CNN
F 1 "GND" H 7650 2030 30  0001 C CNN
F 2 "" H 7650 2100 60  0000 C CNN
F 3 "" H 7650 2100 60  0000 C CNN
	1    7650 2100
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR033
U 1 1 5491E5D3
P 7650 1700
F 0 "#PWR033" H 7650 1650 20  0001 C CNN
F 1 "+BATT" H 7650 1800 30  0000 C CNN
F 2 "" H 7650 1700 60  0000 C CNN
F 3 "" H 7650 1700 60  0000 C CNN
	1    7650 1700
	1    0    0    -1  
$EndComp
$Comp
L MOSFET_N Q1
U 1 1 5491F2D3
P 8150 1050
F 0 "Q1" H 8160 1220 60  0000 R CNN
F 1 "MOSFET_N" H 8160 900 60  0000 R CNN
F 2 "~" H 8150 1050 60  0000 C CNN
F 3 "~" H 8150 1050 60  0000 C CNN
	1    8150 1050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR034
U 1 1 5491F2E2
P 8250 1400
F 0 "#PWR034" H 8250 1400 30  0001 C CNN
F 1 "GND" H 8250 1330 30  0001 C CNN
F 2 "" H 8250 1400 60  0000 C CNN
F 3 "" H 8250 1400 60  0000 C CNN
	1    8250 1400
	1    0    0    -1  
$EndComp
$Comp
L CONN_3 K1
U 1 1 5491F4C1
P 6850 900
F 0 "K1" V 6800 900 50  0000 C CNN
F 1 "Magazine" V 6900 900 40  0000 C CNN
F 2 "~" H 6850 900 60  0000 C CNN
F 3 "~" H 6850 900 60  0000 C CNN
	1    6850 900 
	0    1    -1   0   
$EndComp
$Comp
L GND #PWR035
U 1 1 5491F7A6
P 6950 1400
F 0 "#PWR035" H 6950 1400 30  0001 C CNN
F 1 "GND" H 6950 1330 30  0001 C CNN
F 2 "" H 6950 1400 60  0000 C CNN
F 3 "" H 6950 1400 60  0000 C CNN
	1    6950 1400
	1    0    0    -1  
$EndComp
$Comp
L CONN_2 P12
U 1 1 5491F8BC
P 9100 1050
F 0 "P12" V 9050 1050 40  0000 C CNN
F 1 "CONN_2" V 9150 1050 40  0000 C CNN
F 2 "~" H 9100 1050 60  0000 C CNN
F 3 "~" H 9100 1050 60  0000 C CNN
	1    9100 1050
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR036
U 1 1 5491F8CB
P 8650 1250
F 0 "#PWR036" H 8650 1200 20  0001 C CNN
F 1 "+BATT" H 8650 1350 30  0000 C CNN
F 2 "" H 8650 1250 60  0000 C CNN
F 3 "" H 8650 1250 60  0000 C CNN
	1    8650 1250
	-1   0    0    1   
$EndComp
$Comp
L CONN_5 P4
U 1 1 5491FAFA
P 6000 900
F 0 "P4" V 5950 900 50  0000 C CNN
F 1 "CONN_5" V 6050 900 50  0000 C CNN
F 2 "~" H 6000 900 60  0000 C CNN
F 3 "~" H 6000 900 60  0000 C CNN
	1    6000 900 
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR037
U 1 1 5491FB09
P 5800 1600
F 0 "#PWR037" H 5800 1600 30  0001 C CNN
F 1 "GND" H 5800 1530 30  0001 C CNN
F 2 "" H 5800 1600 60  0000 C CNN
F 3 "" H 5800 1600 60  0000 C CNN
	1    5800 1600
	1    0    0    -1  
$EndComp
$Comp
L CONN_2 P14
U 1 1 54929F23
P 1150 1350
F 0 "P14" V 1100 1350 40  0000 C CNN
F 1 "Power_in" V 1200 1350 40  0000 C CNN
F 2 "~" H 1150 1350 60  0000 C CNN
F 3 "~" H 1150 1350 60  0000 C CNN
	1    1150 1350
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR038
U 1 1 54929F32
P 1600 1550
F 0 "#PWR038" H 1600 1550 30  0001 C CNN
F 1 "GND" H 1600 1480 30  0001 C CNN
F 2 "" H 1600 1550 60  0000 C CNN
F 3 "" H 1600 1550 60  0000 C CNN
	1    1600 1550
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR039
U 1 1 54929F41
P 1600 1150
F 0 "#PWR039" H 1600 1100 20  0001 C CNN
F 1 "+BATT" H 1600 1250 30  0000 C CNN
F 2 "" H 1600 1150 60  0000 C CNN
F 3 "" H 1600 1150 60  0000 C CNN
	1    1600 1150
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR040
U 1 1 5492AB52
P 4550 6750
F 0 "#PWR040" H 4550 6840 20  0001 C CNN
F 1 "+5V" H 4550 6840 30  0000 C CNN
F 2 "" H 4550 6750 60  0000 C CNN
F 3 "" H 4550 6750 60  0000 C CNN
	1    4550 6750
	0    1    1    0   
$EndComp
$Comp
L GND #PWR041
U 1 1 5492AE1B
P 1950 1550
F 0 "#PWR041" H 1950 1550 30  0001 C CNN
F 1 "GND" H 1950 1480 30  0001 C CNN
F 2 "" H 1950 1550 60  0000 C CNN
F 3 "" H 1950 1550 60  0000 C CNN
	1    1950 1550
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR042
U 1 1 5492AF34
P 1900 1250
F 0 "#PWR042" H 1900 1340 20  0001 C CNN
F 1 "+5V" H 1900 1340 30  0000 C CNN
F 2 "" H 1900 1250 60  0000 C CNN
F 3 "" H 1900 1250 60  0000 C CNN
	1    1900 1250
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #PWR043
U 1 1 5492AF4D
P 1900 1350
F 0 "#PWR043" H 1900 1310 30  0001 C CNN
F 1 "+3.3V" H 1900 1460 30  0000 C CNN
F 2 "" H 1900 1350 60  0000 C CNN
F 3 "" H 1900 1350 60  0000 C CNN
	1    1900 1350
	0    -1   -1   0   
$EndComp
$Comp
L CONN_4 P15
U 1 1 5492B181
P 2450 1300
F 0 "P15" V 2400 1300 50  0000 C CNN
F 1 "Power_out" V 2500 1300 50  0000 C CNN
F 2 "~" H 2450 1300 60  0000 C CNN
F 3 "~" H 2450 1300 60  0000 C CNN
	1    2450 1300
	1    0    0    1   
$EndComp
$Comp
L +BATT #PWR044
U 1 1 5492B190
P 2000 1100
F 0 "#PWR044" H 2000 1050 20  0001 C CNN
F 1 "+BATT" H 2000 1200 30  0000 C CNN
F 2 "" H 2000 1100 60  0000 C CNN
F 3 "" H 2000 1100 60  0000 C CNN
	1    2000 1100
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 5492DE26
P 2700 5200
F 0 "R5" V 2780 5200 40  0000 C CNN
F 1 "0" V 2707 5201 40  0000 C CNN
F 2 "~" V 2630 5200 30  0000 C CNN
F 3 "~" H 2700 5200 30  0000 C CNN
	1    2700 5200
	0    -1   -1   0   
$EndComp
$Comp
L R R6
U 1 1 5493119F
P 7800 4000
F 0 "R6" V 7880 4000 40  0000 C CNN
F 1 "0" V 7807 4001 40  0000 C CNN
F 2 "~" V 7730 4000 30  0000 C CNN
F 3 "~" H 7800 4000 30  0000 C CNN
	1    7800 4000
	0    -1   -1   0   
$EndComp
$Comp
L R R9
U 1 1 54931B9C
P 2550 4900
F 0 "R9" V 2630 4900 40  0000 C CNN
F 1 "0" V 2557 4901 40  0000 C CNN
F 2 "~" V 2480 4900 30  0000 C CNN
F 3 "~" H 2550 4900 30  0000 C CNN
	1    2550 4900
	0    -1   -1   0   
$EndComp
$Comp
L R R7
U 1 1 54931F3D
P 2500 5100
F 0 "R7" V 2580 5100 40  0000 C CNN
F 1 "0" V 2507 5101 40  0000 C CNN
F 2 "~" V 2430 5100 30  0000 C CNN
F 3 "~" H 2500 5100 30  0000 C CNN
	1    2500 5100
	0    -1   -1   0   
$EndComp
$Comp
L R R8
U 1 1 54931F4C
P 7600 3700
F 0 "R8" V 7680 3700 40  0000 C CNN
F 1 "0" V 7607 3701 40  0000 C CNN
F 2 "~" V 7530 3700 30  0000 C CNN
F 3 "~" H 7600 3700 30  0000 C CNN
	1    7600 3700
	0    -1   -1   0   
$EndComp
$Comp
L R R10
U 1 1 54931F5B
P 2900 6400
F 0 "R10" V 2980 6400 40  0000 C CNN
F 1 "0" V 2907 6401 40  0000 C CNN
F 2 "~" V 2830 6400 30  0000 C CNN
F 3 "~" H 2900 6400 30  0000 C CNN
	1    2900 6400
	1    0    0    -1  
$EndComp
$Comp
L R R12
U 1 1 549342E3
P 6200 6100
F 0 "R12" V 6280 6100 40  0000 C CNN
F 1 "0" V 6207 6101 40  0000 C CNN
F 2 "~" V 6130 6100 30  0000 C CNN
F 3 "~" H 6200 6100 30  0000 C CNN
	1    6200 6100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4650 1600 4650 1750
Wire Wire Line
	4650 1650 5400 1650
Wire Wire Line
	5400 1650 5400 1750
Wire Wire Line
	5100 1750 5100 1650
Connection ~ 5100 1650
Wire Wire Line
	4950 1750 4950 1650
Connection ~ 4950 1650
Wire Wire Line
	4800 1750 4800 1650
Connection ~ 4800 1650
Connection ~ 4650 1650
Wire Wire Line
	4650 5850 4650 5900
Wire Wire Line
	4650 5900 5400 5900
Wire Wire Line
	5400 5850 5400 6000
Wire Wire Line
	5100 5850 5100 5900
Connection ~ 5100 5900
Wire Wire Line
	4950 5850 4950 5900
Connection ~ 4950 5900
Wire Wire Line
	4800 5850 4800 5900
Connection ~ 4800 5900
Connection ~ 5400 5900
Wire Wire Line
	1750 7350 1750 7500
Wire Wire Line
	2350 6750 2500 6750
Wire Wire Line
	2450 6900 2450 6750
Connection ~ 2450 6750
Wire Wire Line
	2450 7300 2450 7500
Wire Wire Line
	1100 6900 1100 6750
Wire Wire Line
	950  6750 1200 6750
Wire Wire Line
	1100 7300 1100 7500
Connection ~ 1100 6750
Wire Wire Line
	2550 2450 2550 2650
Wire Wire Line
	2550 2600 2600 2600
Wire Wire Line
	2550 3050 2550 3100
Wire Wire Line
	3200 2600 3550 2600
Wire Wire Line
	3250 2600 3250 2650
Connection ~ 3250 2600
Wire Wire Line
	3250 3100 3250 3050
Wire Wire Line
	2550 2450 3550 2450
Connection ~ 2550 2600
Wire Wire Line
	5200 6950 5200 6900
Wire Wire Line
	5200 6900 6000 6900
Wire Wire Line
	6000 6900 6000 6950
Wire Wire Line
	5800 6950 5800 6900
Connection ~ 5800 6900
Wire Wire Line
	5600 6800 5600 6950
Connection ~ 5600 6900
Wire Wire Line
	5400 6950 5400 6900
Connection ~ 5400 6900
Wire Wire Line
	5200 7350 5200 7400
Wire Wire Line
	5200 7400 6000 7400
Wire Wire Line
	6000 7400 6000 7350
Wire Wire Line
	5400 7350 5400 7400
Connection ~ 5400 7400
Wire Wire Line
	5600 7350 5600 7500
Connection ~ 5600 7400
Wire Wire Line
	5800 7350 5800 7400
Connection ~ 5800 7400
Wire Notes Line
	5050 6600 6150 6600
Wire Notes Line
	6150 6600 6150 7600
Wire Notes Line
	6150 7600 5050 7600
Wire Notes Line
	5050 7600 5050 6600
Wire Wire Line
	6650 3500 6850 3500
Wire Wire Line
	6850 3600 6650 3600
Wire Wire Line
	3400 2100 3550 2100
Wire Wire Line
	7750 3100 6650 3100
Wire Wire Line
	6650 3200 7650 3200
Wire Wire Line
	7650 3200 7650 3000
Wire Wire Line
	7650 3000 7750 3000
Wire Wire Line
	7650 3400 7650 3300
Wire Wire Line
	7650 3300 7750 3300
Wire Wire Line
	6650 2400 7750 2400
Wire Wire Line
	7650 2500 7750 2500
Wire Wire Line
	6650 2500 7400 2500
Wire Wire Line
	7400 2500 7400 2300
Wire Wire Line
	7400 2300 7750 2300
Wire Wire Line
	7650 2700 7650 2600
Wire Wire Line
	7650 2600 7750 2600
Wire Wire Line
	6650 4300 8150 4300
Wire Wire Line
	8150 4150 7900 4150
Wire Wire Line
	7900 4150 7900 4400
Wire Wire Line
	7900 4400 6650 4400
Wire Wire Line
	7900 3700 7900 3850
Wire Wire Line
	7900 3850 8150 3850
Wire Wire Line
	8150 4600 8050 4600
Wire Wire Line
	8050 4600 8050 4650
Wire Wire Line
	7950 4750 8150 4750
Wire Wire Line
	7950 4900 8150 4900
Wire Wire Line
	6650 4500 8000 4500
Wire Wire Line
	8000 4500 8000 4450
Wire Wire Line
	8000 4450 8150 4450
Wire Wire Line
	6750 6200 6750 4900
Wire Wire Line
	6750 4900 6650 4900
Wire Wire Line
	6650 5000 6850 5000
Wire Wire Line
	6850 5000 6850 6200
Wire Wire Line
	6650 6050 6650 6200
Wire Wire Line
	6650 5400 7600 5400
Wire Wire Line
	7600 5400 7600 5750
Wire Wire Line
	7600 5750 8150 5750
Wire Wire Line
	6650 5300 7700 5300
Wire Wire Line
	7700 5300 7700 5850
Wire Wire Line
	7700 5850 8150 5850
Wire Wire Line
	6650 5200 7800 5200
Wire Wire Line
	7800 5200 7800 5650
Wire Wire Line
	7800 5650 8150 5650
Wire Wire Line
	6650 5100 7900 5100
Wire Wire Line
	7900 5100 7900 5550
Wire Wire Line
	7900 5550 8150 5550
Wire Wire Line
	8150 5950 6950 5950
Wire Wire Line
	6950 5950 6950 4700
Wire Wire Line
	6950 4700 6650 4700
Wire Wire Line
	6650 4600 7100 4600
Wire Wire Line
	7100 4600 7100 5000
Wire Wire Line
	7100 5000 8000 5000
Wire Wire Line
	8000 5000 8000 5450
Wire Wire Line
	8000 5450 8150 5450
Wire Wire Line
	8050 5350 8150 5350
Wire Wire Line
	9000 2700 9000 2850
Wire Wire Line
	9650 2700 9650 2850
Wire Wire Line
	10250 2700 10250 2850
Wire Wire Line
	10850 2700 10850 2850
Wire Wire Line
	9000 3250 9000 3400
Wire Wire Line
	9650 3250 9650 3400
Wire Wire Line
	10250 3250 10250 3400
Wire Wire Line
	10850 3250 10850 3400
Wire Wire Line
	9000 2200 9000 2000
Wire Wire Line
	9000 2000 10850 2000
Wire Wire Line
	10850 2000 10850 2200
Wire Wire Line
	9650 2200 9650 2000
Connection ~ 9650 2000
Wire Wire Line
	10250 2200 10250 2000
Connection ~ 10250 2000
Wire Wire Line
	6650 2800 8550 2800
Wire Wire Line
	8550 2800 8550 3050
Wire Wire Line
	8550 3050 8700 3050
Wire Wire Line
	6650 2900 8400 2900
Wire Wire Line
	8400 2900 8400 3550
Wire Wire Line
	8400 3550 9200 3550
Wire Wire Line
	9200 3550 9200 3050
Wire Wire Line
	9200 3050 9350 3050
Wire Wire Line
	6650 3900 7300 3900
Wire Wire Line
	7300 3900 7300 3600
Wire Wire Line
	7300 3600 9800 3600
Wire Wire Line
	9800 3600 9800 3050
Wire Wire Line
	9800 3050 9950 3050
Wire Wire Line
	6650 4000 7450 4000
Wire Wire Line
	7450 4000 7450 3650
Wire Wire Line
	7450 3650 10400 3650
Wire Wire Line
	10400 3650 10400 3050
Wire Wire Line
	10400 3050 10550 3050
Wire Wire Line
	9950 1850 9850 1850
Wire Wire Line
	9850 1850 9850 2000
Connection ~ 9850 2000
Wire Wire Line
	9850 1600 9850 1650
Wire Wire Line
	9850 1650 9950 1650
Wire Wire Line
	8700 1850 8850 1850
Wire Wire Line
	8500 1900 8500 1750
Wire Wire Line
	8500 1750 8850 1750
Wire Wire Line
	8300 2800 8300 1650
Wire Wire Line
	8300 1650 8850 1650
Connection ~ 8300 2800
Wire Wire Line
	3550 3500 1850 3500
Wire Wire Line
	1850 3500 1850 3200
Wire Wire Line
	1850 3200 1800 3200
Wire Wire Line
	3550 3600 950  3600
Wire Wire Line
	950  3600 950  3200
Wire Wire Line
	950  3200 1000 3200
Wire Wire Line
	3550 3700 1900 3700
Wire Wire Line
	1900 3700 1900 3100
Wire Wire Line
	1900 3100 1800 3100
Wire Wire Line
	3550 3900 900  3900
Wire Wire Line
	900  3900 900  3100
Wire Wire Line
	900  3100 1000 3100
Wire Wire Line
	3550 4000 1950 4000
Wire Wire Line
	1950 4000 1950 3000
Wire Wire Line
	1950 3000 1800 3000
Wire Wire Line
	3550 4100 850  4100
Wire Wire Line
	850  4100 850  3000
Wire Wire Line
	850  3000 1000 3000
Wire Wire Line
	3550 4200 2000 4200
Wire Wire Line
	2000 4200 2000 2900
Wire Wire Line
	2000 2900 1800 2900
Wire Wire Line
	2150 5000 3550 5000
Wire Wire Line
	3550 5100 3450 5100
Wire Wire Line
	3450 5100 3450 5300
Wire Wire Line
	3450 5300 2150 5300
Wire Wire Line
	3550 3300 3350 3300
Wire Wire Line
	3350 3300 3350 5100
Wire Wire Line
	3550 4800 3250 4800
Wire Wire Line
	3250 4800 3250 5600
Wire Wire Line
	3550 4700 3150 4700
Wire Wire Line
	3150 4700 3150 5500
Wire Wire Line
	2150 5400 3050 5400
Wire Wire Line
	3050 5400 3050 5750
Wire Wire Line
	3700 7350 3700 7500
Wire Wire Line
	4300 6750 4550 6750
Wire Wire Line
	4400 6900 4400 6750
Connection ~ 4400 6750
Wire Wire Line
	4400 7300 4400 7500
Wire Wire Line
	2750 6750 3150 6750
Wire Wire Line
	3050 5200 3050 4750
Wire Wire Line
	3550 4300 800  4300
Wire Wire Line
	800  4300 800  2900
Wire Wire Line
	800  2900 1000 2900
Wire Wire Line
	3550 4400 2050 4400
Wire Wire Line
	2050 4400 2050 2800
Wire Wire Line
	2050 2800 1800 2800
Wire Wire Line
	3550 4500 700  4500
Wire Wire Line
	700  4500 700  2800
Wire Wire Line
	700  2800 1000 2800
Wire Wire Line
	7650 2100 7650 2000
Wire Wire Line
	7650 2000 7750 2000
Wire Wire Line
	6650 2600 7300 2600
Wire Wire Line
	7300 2600 7300 1900
Wire Wire Line
	7300 1900 7750 1900
Wire Wire Line
	7750 1800 7650 1800
Wire Wire Line
	7650 1800 7650 1700
Wire Wire Line
	8250 1400 8250 1250
Wire Wire Line
	6650 2300 7200 2300
Wire Wire Line
	7200 2300 7200 1050
Wire Wire Line
	6650 3300 6750 3300
Wire Wire Line
	6750 3300 6750 1250
Wire Wire Line
	6650 3400 6850 3400
Wire Wire Line
	6850 3400 6850 1250
Wire Wire Line
	6950 1250 6950 1400
Wire Wire Line
	8650 1250 8650 1150
Wire Wire Line
	8650 1150 8750 1150
Wire Wire Line
	8250 850  8250 750 
Wire Wire Line
	8250 750  8650 750 
Wire Wire Line
	8650 750  8650 950 
Wire Wire Line
	8650 950  8750 950 
Wire Wire Line
	5800 1300 5800 1600
Wire Wire Line
	3550 4600 3450 4600
Wire Wire Line
	3450 4600 3450 1400
Wire Wire Line
	3450 1400 5900 1400
Wire Wire Line
	5900 1400 5900 1300
Wire Wire Line
	6650 2200 6650 1800
Wire Wire Line
	6650 1800 6000 1800
Wire Wire Line
	6000 1800 6000 1300
Wire Wire Line
	6650 4800 7000 4800
Wire Wire Line
	7000 4800 7000 1700
Wire Wire Line
	7000 1700 6100 1700
Wire Wire Line
	6100 1700 6100 1300
Wire Wire Line
	6650 4100 7100 4100
Wire Wire Line
	7100 4100 7100 1550
Wire Wire Line
	7100 1550 6200 1550
Wire Wire Line
	6200 1550 6200 1300
Wire Wire Line
	1500 1250 1600 1250
Wire Wire Line
	1600 1250 1600 1150
Wire Wire Line
	1500 1450 1600 1450
Wire Wire Line
	1600 1450 1600 1550
Wire Wire Line
	1950 1550 1950 1450
Wire Wire Line
	1950 1450 2100 1450
Wire Wire Line
	1900 1250 2100 1250
Wire Wire Line
	1900 1350 2100 1350
Wire Wire Line
	2000 1100 2000 1150
Wire Wire Line
	2000 1150 2100 1150
Wire Wire Line
	8050 4000 8150 4000
Wire Wire Line
	7550 4000 7500 4000
Wire Wire Line
	7500 4000 7500 4200
Wire Wire Line
	7500 4200 6650 4200
Wire Wire Line
	2950 5200 3050 5200
Wire Wire Line
	2150 5200 2450 5200
Wire Wire Line
	2150 4900 2300 4900
Wire Wire Line
	2800 4900 3550 4900
Wire Wire Line
	2150 5100 2250 5100
Wire Wire Line
	3350 5100 2750 5100
Wire Wire Line
	7350 3700 6650 3700
Wire Wire Line
	7850 3700 7900 3700
Wire Wire Line
	2900 6750 2900 6650
Wire Wire Line
	2900 6150 2900 6050
Wire Wire Line
	950  6750 950  6250
Wire Wire Line
	950  6250 2750 6250
Wire Wire Line
	2750 6250 2750 6750
Connection ~ 2900 6750
Wire Wire Line
	8050 5350 8050 5300
Wire Wire Line
	8050 5300 9300 5300
Wire Wire Line
	9300 5300 9300 4700
Wire Wire Line
	5850 6200 5850 6100
Wire Wire Line
	5850 6100 5950 6100
Wire Wire Line
	6450 6100 7150 6100
Wire Wire Line
	6550 6100 6550 6200
Wire Wire Line
	8150 5250 7150 5250
Wire Wire Line
	7150 5250 7150 6100
Connection ~ 6550 6100
$Comp
L R R11
U 1 1 54943426
P 7550 1050
F 0 "R11" V 7630 1050 40  0000 C CNN
F 1 "0" V 7557 1051 40  0000 C CNN
F 2 "~" V 7480 1050 30  0000 C CNN
F 3 "~" H 7550 1050 30  0000 C CNN
	1    7550 1050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7200 1050 7300 1050
Wire Wire Line
	7800 1050 7950 1050
$Comp
L R R13
U 1 1 54943703
P 2600 5500
F 0 "R13" V 2680 5500 40  0000 C CNN
F 1 "0" V 2607 5501 40  0000 C CNN
F 2 "~" V 2530 5500 30  0000 C CNN
F 3 "~" H 2600 5500 30  0000 C CNN
	1    2600 5500
	0    -1   -1   0   
$EndComp
$Comp
L R R14
U 1 1 54943717
P 2600 5600
F 0 "R14" V 2680 5600 40  0000 C CNN
F 1 "0" V 2607 5601 40  0000 C CNN
F 2 "~" V 2530 5600 30  0000 C CNN
F 3 "~" H 2600 5600 30  0000 C CNN
	1    2600 5600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2150 5500 2350 5500
Wire Wire Line
	2150 5600 2350 5600
Wire Wire Line
	3150 5500 2850 5500
Wire Wire Line
	3250 5600 2850 5600
$EndSCHEMATC

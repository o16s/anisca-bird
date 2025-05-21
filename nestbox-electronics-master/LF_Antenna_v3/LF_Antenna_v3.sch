EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	3050 2500 2650 2500
Wire Wire Line
	2650 2500 2650 2750
Wire Wire Line
	2150 2750 2650 2750
Wire Wire Line
	2150 2850 2650 2850
Wire Wire Line
	2650 2850 2650 3100
Wire Wire Line
	2650 3100 3050 3100
$Comp
L Device:L L1
U 1 1 604FD420
P 3050 2800
F 0 "L1" H 3103 2846 50  0000 L CNN
F 1 "LF-ANT" H 3103 2755 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" H 3050 2800 50  0001 C CNN
F 3 "~" H 3050 2800 50  0001 C CNN
F 4 "OCTANIS-JZCOIL-120" H 3050 2800 50  0001 C CNN "MPN"
	1    3050 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 2500 3050 2650
Wire Wire Line
	3050 2950 3050 3100
$Comp
L Connector:Conn_01x02_Male J1
U 1 1 61CEEDFF
P 1950 2750
F 0 "J1" H 2058 2931 50  0000 C CNN
F 1 "Conn_01x02_Male" H 2058 2840 50  0000 C CNN
F 2 "lib_fp_global:WAGO-2060" H 1950 2750 50  0001 C CNN
F 3 "~" H 1950 2750 50  0001 C CNN
	1    1950 2750
	1    0    0    -1  
$EndComp
$EndSCHEMATC

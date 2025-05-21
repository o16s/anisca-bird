EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 6
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Nestbox_v2-rescue:C-device C610
U 1 1 60E9101B
P 9200 3100
F 0 "C610" V 8948 3100 50  0000 C CNN
F 1 "10u" V 9039 3100 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9238 2950 50  0001 C CNN
F 3 "" H 9200 3100 50  0001 C CNN
F 4 "" H 750 0   50  0001 C CNN "MPN"
	1    9200 3100
	0    1    1    0   
$EndComp
$Comp
L Nestbox_v2-rescue:GND-power1 #PWR0612
U 1 1 60E9101C
P 8700 4600
F 0 "#PWR0612" H 8700 4350 50  0001 C CNN
F 1 "GND" H 8705 4427 50  0000 C CNN
F 2 "" H 8700 4600 50  0001 C CNN
F 3 "" H 8700 4600 50  0001 C CNN
	1    8700 4600
	1    0    0    -1  
$EndComp
$Comp
L Nestbox_v2-rescue:C-device C609
U 1 1 5AF73543
P 8450 3400
F 0 "C609" V 8702 3400 50  0000 C CNN
F 1 "100n" V 8611 3400 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8488 3250 50  0001 C CNN
F 3 "" H 8450 3400 50  0001 C CNN
F 4 "" H 750 0   50  0001 C CNN "MPN"
	1    8450 3400
	0    -1   -1   0   
$EndComp
$Comp
L Nestbox_v2-rescue:C-device C608
U 1 1 60E9101E
P 8450 3100
F 0 "C608" V 8198 3100 50  0000 C CNN
F 1 "1u" V 8289 3100 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8488 2950 50  0001 C CNN
F 3 "" H 8450 3100 50  0001 C CNN
F 4 "" H 750 0   50  0001 C CNN "MPN"
	1    8450 3100
	0    1    1    0   
$EndComp
$Comp
L Nestbox_v2-rescue:C-device C611
U 1 1 60E9101F
P 9200 3350
F 0 "C611" V 8948 3350 50  0000 C CNN
F 1 "10n" V 9039 3350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 9238 3200 50  0001 C CNN
F 3 "" H 9200 3350 50  0001 C CNN
F 4 "" V 9200 3350 60  0001 C CNN "MPN"
	1    9200 3350
	0    1    1    0   
$EndComp
$Comp
L Nestbox_v2-rescue:GND-power1 #PWR0614
U 1 1 60E91020
P 8800 4600
F 0 "#PWR0614" H 8800 4350 50  0001 C CNN
F 1 "GND" H 8805 4427 50  0000 C CNN
F 2 "" H 8800 4600 50  0001 C CNN
F 3 "" H 8800 4600 50  0001 C CNN
	1    8800 4600
	1    0    0    -1  
$EndComp
$Comp
L Nestbox_v2-rescue:GND-power1 #PWR0617
U 1 1 60E91021
P 9400 3100
F 0 "#PWR0617" H 9400 2850 50  0001 C CNN
F 1 "GND" V 9405 2972 50  0000 R CNN
F 2 "" H 9400 3100 50  0001 C CNN
F 3 "" H 9400 3100 50  0001 C CNN
	1    9400 3100
	0    -1   -1   0   
$EndComp
$Comp
L Nestbox_v2-rescue:GND-power1 #PWR0618
U 1 1 60E91022
P 9400 3350
F 0 "#PWR0618" H 9400 3100 50  0001 C CNN
F 1 "GND" V 9405 3222 50  0000 R CNN
F 2 "" H 9400 3350 50  0001 C CNN
F 3 "" H 9400 3350 50  0001 C CNN
	1    9400 3350
	0    -1   -1   0   
$EndComp
$Comp
L Nestbox_v2-rescue:GND-power1 #PWR0610
U 1 1 60E91023
P 8200 3400
F 0 "#PWR0610" H 8200 3150 50  0001 C CNN
F 1 "GND" V 8205 3272 50  0000 R CNN
F 2 "" H 8200 3400 50  0001 C CNN
F 3 "" H 8200 3400 50  0001 C CNN
	1    8200 3400
	0    1    1    0   
$EndComp
$Comp
L Nestbox_v2-rescue:GND-power1 #PWR0609
U 1 1 60E91024
P 8200 3100
F 0 "#PWR0609" H 8200 2850 50  0001 C CNN
F 1 "GND" V 8205 2972 50  0000 R CNN
F 2 "" H 8200 3100 50  0001 C CNN
F 3 "" H 8200 3100 50  0001 C CNN
	1    8200 3100
	0    1    1    0   
$EndComp
Text Notes 8950 2700 0    60   ~ 0
cut-off frequency not too low to allow 1 kHz PWM sampling\n
$Comp
L Nestbox_v2-rescue:C-device C604
U 1 1 5AF750AA
P 5550 4000
F 0 "C604" H 5665 4046 50  0000 L CNN
F 1 "100n 1% DNP" H 5350 3900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5588 3850 50  0001 C CNN
F 3 "" H 5550 4000 50  0001 C CNN
	1    5550 4000
	1    0    0    -1  
$EndComp
$Comp
L Nestbox_v2-rescue:R-device R607
U 1 1 5AF7525C
P 4600 3800
F 0 "R607" V 4550 4000 50  0000 C CNN
F 1 "0R" V 4600 3800 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4530 3800 50  0001 C CNN
F 3 "" H 4600 3800 50  0001 C CNN
	1    4600 3800
	0    1    1    0   
$EndComp
$Comp
L Nestbox_v2-rescue:R-device R608
U 1 1 5AF752AD
P 4600 4200
F 0 "R608" V 4550 4400 50  0000 C CNN
F 1 "0R" V 4600 4200 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4530 4200 50  0001 C CNN
F 3 "" H 4600 4200 50  0001 C CNN
	1    4600 4200
	0    1    1    0   
$EndComp
$Comp
L Nestbox_v2-rescue:C-device C605
U 1 1 5AF75450
P 6450 4000
F 0 "C605" H 6565 4046 50  0000 L CNN
F 1 "100n DNP" H 6565 3955 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6488 3850 50  0001 C CNN
F 3 "" H 6450 4000 50  0001 C CNN
F 4 "" H 0   0   50  0001 C CNN "MPN"
	1    6450 4000
	-1   0    0    -1  
$EndComp
$Comp
L Nestbox_v2-rescue:R-device R617
U 1 1 5AF767DC
P 9950 3900
F 0 "R617" V 9900 4100 50  0000 C CNN
F 1 "47R" V 9950 3900 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9880 3900 50  0001 C CNN
F 3 "" H 9950 3900 50  0001 C CNN
F 4 "" H 750 0   50  0001 C CNN "MPN"
F 5 "" H 750 0   50  0001 C CNN "SKU"
	1    9950 3900
	0    1    1    0   
$EndComp
$Comp
L Nestbox_v2-rescue:R-device R616
U 1 1 5AF7680A
P 9950 3800
F 0 "R616" V 9900 4000 50  0000 C CNN
F 1 "47R" V 9950 3800 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9880 3800 50  0001 C CNN
F 3 "" H 9950 3800 50  0001 C CNN
F 4 "" H 750 0   50  0001 C CNN "MPN"
F 5 "" H 750 0   50  0001 C CNN "SKU"
	1    9950 3800
	0    1    1    0   
$EndComp
$Comp
L Nestbox_v2-rescue:R-device R615
U 1 1 5AF7683A
P 9950 3700
F 0 "R615" V 9900 3900 50  0000 C CNN
F 1 "47R" V 9950 3700 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9880 3700 50  0001 C CNN
F 3 "" H 9950 3700 50  0001 C CNN
F 4 "" H 750 0   50  0001 C CNN "MPN"
F 5 "" H 750 0   50  0001 C CNN "SKU"
	1    9950 3700
	0    1    1    0   
$EndComp
Text Notes 4300 2250 0    60   ~ 0
X2YTM capacitors were used for the RC low-pass filters. \nA single X2YTM capacitor can replace multiple capacitors \nrequired for common-mode and differential filtering. In \naddition to saving board space, these capacitors have \nlow equivalent series inductance (ESL) and excellent \ncommon-mode capacitor matching, required to \nprevent the conversion of common-mode signals \nto differential signals. \n
Text Notes 2900 3500 0    60   ~ 0
!! high RC resistors result \nin high input offset from \ndifferential bias currents !!
Text Notes 2200 6300 0    60   ~ 0
Typically common-mode filters become mismatched because of high \ncapacitor tolerances. Higher common-mode signal frequencies are then\n filtered differently and result in a differential signal seen by the ADC \ninputs. This effect can also be reduced by ensuring that the differential\n filter has a lower cutoff frequency (~1/10 ) than the common-mode \nfilter’s cutoff frequency. \n
Text Label 4100 3150 0    60   ~ 0
EXC_A
Text Label 4100 4850 0    60   ~ 0
EXC_B
$Comp
L Nestbox_v2-rescue:VAA-power1 #PWR0613
U 1 1 60E9102E
P 8800 3050
F 0 "#PWR0613" H 8800 2900 50  0001 C CNN
F 1 "VAA" H 8817 3223 50  0000 C CNN
F 2 "" H 8800 3050 50  0001 C CNN
F 3 "" H 8800 3050 50  0001 C CNN
	1    8800 3050
	1    0    0    -1  
$EndComp
$Comp
L Nestbox_v2-rescue:R-device R612
U 1 1 60E91034
P 6950 4300
F 0 "R612" H 7020 4346 50  0000 L CNN
F 1 "0R" H 7020 4255 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6880 4300 50  0001 C CNN
F 3 "" H 6950 4300 50  0001 C CNN
	1    6950 4300
	0    1    1    0   
$EndComp
$Comp
L Nestbox_v2-rescue:ADS1120-PW U601
U 1 1 60E91019
P 8700 4000
F 0 "U601" H 8700 4678 50  0000 C CNN
F 1 "ADS1220" H 9150 4450 50  0000 C CNN
F 2 "Package_SO:TSSOP-16_4.4x5mm_P0.65mm" H 8950 4550 50  0001 L CNN
F 3 "" H 8200 4400 50  0001 C CNN
F 4 "ADS1220IPW" H 750 0   50  0001 C CNN "MPN"
	1    8700 4000
	1    0    0    -1  
$EndComp
$Comp
L Nestbox_v2-rescue:R-device R611
U 1 1 60E91035
P 6950 3700
F 0 "R611" H 6750 3750 50  0000 L CNN
F 1 "0R" H 6750 3650 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6880 3700 50  0001 C CNN
F 3 "" H 6950 3700 50  0001 C CNN
	1    6950 3700
	0    1    1    0   
$EndComp
$Comp
L Nestbox_v2-rescue:R-device R610
U 1 1 60E91037
P 5700 4850
F 0 "R610" H 5500 4900 50  0000 L CNN
F 1 "0R" H 5500 4800 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5630 4850 50  0001 C CNN
F 3 "" H 5700 4850 50  0001 C CNN
F 4 "" H 750 0   50  0001 C CNN "MPN"
	1    5700 4850
	0    1    1    0   
$EndComp
Text Label 7600 3900 0    60   ~ 0
AIN1
Text Label 7600 4100 0    60   ~ 0
AIN2
Text Label 5600 3800 0    60   ~ 0
AIN2
Text Label 5600 4200 0    60   ~ 0
AIN1
Wire Wire Line
	8800 3050 8800 3100
Wire Wire Line
	8800 3100 9050 3100
Connection ~ 8800 3100
Wire Wire Line
	8700 2900 8700 3100
Wire Wire Line
	8800 4600 8800 4500
Wire Wire Line
	8700 4600 8700 4500
Wire Wire Line
	8600 3100 8700 3100
Connection ~ 8700 3100
Wire Wire Line
	9050 3350 8800 3350
Connection ~ 8800 3350
Wire Wire Line
	8600 3400 8700 3400
Connection ~ 8700 3400
Wire Wire Line
	9400 3350 9350 3350
Wire Wire Line
	8200 3400 8300 3400
Wire Wire Line
	8200 3100 8300 3100
Wire Wire Line
	9350 3100 9400 3100
Wire Wire Line
	10100 3700 10500 3700
Wire Wire Line
	10100 3800 10500 3800
Wire Wire Line
	10100 3900 10500 3900
Wire Wire Line
	9400 4000 10500 4000
Wire Wire Line
	9800 3900 9400 3900
Wire Wire Line
	9400 3800 9800 3800
Wire Wire Line
	9800 3700 9400 3700
Wire Wire Line
	4450 3800 4200 3800
Wire Wire Line
	4450 4200 4200 4200
Wire Wire Line
	6450 4200 6450 4150
Connection ~ 6450 4200
Wire Wire Line
	4200 4050 4200 4200
Wire Wire Line
	9400 4200 10500 4200
Wire Wire Line
	9550 4350 9550 4300
Wire Wire Line
	9550 4300 9400 4300
Connection ~ 4050 3150
Wire Wire Line
	7100 4300 7350 4300
Wire Wire Line
	6800 3700 6700 3700
Wire Wire Line
	6700 3700 6700 4200
Connection ~ 6700 4200
Wire Wire Line
	6800 4300 6750 4300
Wire Wire Line
	6750 4300 6750 3800
Connection ~ 6750 3800
Wire Wire Line
	7100 3700 7350 3700
Connection ~ 5550 3800
Connection ~ 5550 4200
Wire Wire Line
	5550 4200 5550 4150
Wire Wire Line
	5550 3800 5550 3850
Wire Wire Line
	6450 3800 6450 3850
Connection ~ 6450 3800
Wire Wire Line
	8000 3900 7600 3900
Wire Wire Line
	8000 4100 7600 4100
Wire Wire Line
	5850 3150 6450 3150
Wire Wire Line
	6450 4200 6450 4850
Wire Wire Line
	6450 4850 5850 4850
Text HLabel 10500 3700 2    60   Input ~ 0
SPI_SCK
Text HLabel 10500 3800 2    60   Input ~ 0
SPI_MOSI
Text HLabel 10500 3900 2    60   Input ~ 0
SPI_MISO
Text HLabel 10500 4000 2    60   Input ~ 0
SPI_CS_N
Text HLabel 10500 4200 2    60   Input ~ 0
DRDY_N
Wire Wire Line
	4050 3750 4050 3150
$Comp
L Nestbox_v2-rescue:R-device R609
U 1 1 60E91036
P 5700 3150
F 0 "R609" V 5500 3250 50  0000 L CNN
F 1 "0R" H 5500 3100 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5630 3150 50  0001 C CNN
F 3 "" H 5700 3150 50  0001 C CNN
F 4 "" H 750 0   50  0001 C CNN "MPN"
	1    5700 3150
	0    1    1    0   
$EndComp
Text Notes 3500 5450 0    60   ~ 0
using a first-order RC filter with a cutoff \nfrequency set at the output data rate or \n10x higher is generally a good start\n\n—> ca 20 - 200 Hz when in exact mode\n
Wire Wire Line
	2800 4150 2800 4850
Connection ~ 2800 3150
Wire Wire Line
	2800 3150 2800 3050
$Comp
L Nestbox_v2-rescue:C-device C602
U 1 1 60E91031
P 2800 4000
F 0 "C602" H 2915 4046 50  0000 L CNN
F 1 "1u DNP" H 2915 3955 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2838 3850 50  0001 C CNN
F 3 "" H 2800 4000 50  0001 C CNN
F 4 "" H 500 0   50  0001 C CNN "MPN"
	1    2800 4000
	1    0    0    -1  
$EndComp
Connection ~ 2800 4850
Wire Wire Line
	2800 4850 2800 4950
$Comp
L Nestbox_v2-rescue:GND-power1 #PWR0608
U 1 1 60E91032
P 2800 5450
F 0 "#PWR0608" H 2800 5200 50  0001 C CNN
F 1 "GND" H 2805 5277 50  0000 C CNN
F 2 "" H 2800 5450 50  0001 C CNN
F 3 "" H 2800 5450 50  0001 C CNN
	1    2800 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 5250 2800 5450
$Comp
L Nestbox_v2-rescue:C-device C603
U 1 1 60E91030
P 2800 5100
F 0 "C603" H 2915 5146 50  0000 L CNN
F 1 "100n 1% DNP" H 2950 5050 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2838 4950 50  0001 C CNN
F 3 "" H 2800 5100 50  0001 C CNN
F 4 "" H 250 0   50  0001 C CNN "MPN"
	1    2800 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 2550 2800 2750
$Comp
L Nestbox_v2-rescue:GND-power1 #PWR0607
U 1 1 60E91033
P 2800 2550
F 0 "#PWR0607" H 2800 2300 50  0001 C CNN
F 1 "GND" H 2805 2377 50  0000 C CNN
F 2 "" H 2800 2550 50  0001 C CNN
F 3 "" H 2800 2550 50  0001 C CNN
	1    2800 2550
	-1   0    0    1   
$EndComp
$Comp
L Nestbox_v2-rescue:C-device C601
U 1 1 60E9102F
P 2800 2900
F 0 "C601" H 2915 2946 50  0000 L CNN
F 1 "100n 1% DNP" H 2100 2900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2838 2750 50  0001 C CNN
F 3 "" H 2800 2900 50  0001 C CNN
F 4 "" H 300 0   50  0001 C CNN "MPN"
	1    2800 2900
	-1   0    0    1   
$EndComp
Wire Wire Line
	6450 3150 6450 3800
Wire Wire Line
	2800 3150 2800 3850
Text Notes 5050 2700 0    60   ~ 0
performance "seems" better with 0R resistor here!
Wire Wire Line
	8800 3100 8800 3350
Wire Wire Line
	8700 3100 8700 3400
Wire Wire Line
	8800 3350 8800 3500
Wire Wire Line
	8700 3400 8700 3500
Wire Wire Line
	6450 4200 6700 4200
Wire Wire Line
	4050 3150 5550 3150
Wire Wire Line
	6700 4200 8000 4200
Wire Wire Line
	6750 3800 8000 3800
Wire Wire Line
	5550 3800 5800 3800
Wire Wire Line
	5550 4200 5800 4200
Wire Wire Line
	6450 3800 6750 3800
Wire Wire Line
	2800 3150 4050 3150
Wire Wire Line
	4750 3800 5550 3800
Wire Wire Line
	4750 4200 5550 4200
Text Notes 1250 1900 0    60   ~ 0
!! high RC resistors result \nin high input offset from \ndifferential bias currents !!
$Comp
L power:GND #PWR0604
U 1 1 60EAA644
P 1750 3700
F 0 "#PWR0604" H 1750 3450 50  0001 C CNN
F 1 "GND" H 1755 3527 50  0000 C CNN
F 2 "" H 1750 3700 50  0001 C CNN
F 3 "" H 1750 3700 50  0001 C CNN
	1    1750 3700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0606
U 1 1 60EAA64A
P 1750 5450
F 0 "#PWR0606" H 1750 5200 50  0001 C CNN
F 1 "GND" H 1755 5277 50  0000 C CNN
F 2 "" H 1750 5450 50  0001 C CNN
F 3 "" H 1750 5450 50  0001 C CNN
	1    1750 5450
	1    0    0    -1  
$EndComp
$Comp
L power:VAA #PWR0603
U 1 1 60EAA650
P 1750 2450
F 0 "#PWR0603" H 1750 2300 50  0001 C CNN
F 1 "VAA" H 1767 2623 50  0000 C CNN
F 2 "" H 1750 2450 50  0001 C CNN
F 3 "" H 1750 2450 50  0001 C CNN
	1    1750 2450
	1    0    0    -1  
$EndComp
$Comp
L power:VAA #PWR0605
U 1 1 60EAA656
P 1750 4150
F 0 "#PWR0605" H 1750 4000 50  0001 C CNN
F 1 "VAA" H 1767 4323 50  0000 C CNN
F 2 "" H 1750 4150 50  0001 C CNN
F 3 "" H 1750 4150 50  0001 C CNN
	1    1750 4150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R605
U 1 1 60EAA65C
P 2050 3150
F 0 "R605" V 2000 3350 50  0000 C CNN
F 1 "0R" V 2050 3150 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1980 3150 50  0001 C CNN
F 3 "" H 2050 3150 50  0001 C CNN
	1    2050 3150
	0    1    1    0   
$EndComp
$Comp
L Device:R R606
U 1 1 60EAA662
P 2050 4850
F 0 "R606" V 2000 5050 50  0000 C CNN
F 1 "0R" V 2050 4850 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1980 4850 50  0001 C CNN
F 3 "" H 2050 4850 50  0001 C CNN
	1    2050 4850
	0    1    1    0   
$EndComp
Wire Wire Line
	1750 2450 1750 2500
Wire Wire Line
	1750 3100 1750 3150
Connection ~ 1750 3150
Wire Wire Line
	1750 4800 1750 4850
Connection ~ 1750 4850
Wire Wire Line
	1750 3600 1750 3700
Wire Wire Line
	1750 4150 1750 4200
Wire Wire Line
	1750 5350 1750 5450
Wire Wire Line
	1900 3150 1750 3150
Wire Wire Line
	1750 4850 1900 4850
Wire Wire Line
	1100 3400 1250 3400
Wire Wire Line
	1100 2900 1250 2900
Wire Wire Line
	1100 4600 1250 4600
Wire Wire Line
	1100 5150 1250 5150
Text HLabel 1100 2900 0    60   Input ~ 0
EXC_A_P
Text HLabel 1100 3400 0    60   Input ~ 0
EXC_A_N
Text HLabel 1100 5150 0    60   Input ~ 0
EXC_B_N
Text HLabel 1100 4600 0    60   Input ~ 0
EXC_B_P
$Comp
L Device:Q_PMOS_GSD Q603
U 1 1 60EAA6A4
P 1650 4600
F 0 "Q603" H 1850 4650 50  0000 L CNN
F 1 "DMG2301L-7" H 1850 4550 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 1850 4700 50  0001 C CNN
F 3 "" H 1650 4600 50  0001 C CNN
F 4 "DMG2301L-7" H 1650 4600 50  0001 C CNN "MPN"
	1    1650 4600
	1    0    0    1   
$EndComp
$Comp
L Device:Q_PMOS_GSD Q601
U 1 1 60EAA6AA
P 1650 2900
F 0 "Q601" H 1850 2950 50  0000 L CNN
F 1 "DMG2301L-7" H 1850 2850 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 1850 3000 50  0001 C CNN
F 3 "" H 1650 2900 50  0001 C CNN
F 4 "DMG2301L-7" H 1650 2900 50  0001 C CNN "MPN"
	1    1650 2900
	1    0    0    1   
$EndComp
$Comp
L Device:Q_NMOS_GSD Q602
U 1 1 60EAA6B0
P 1650 3400
F 0 "Q602" H 1850 3450 50  0000 L CNN
F 1 "NMOS 30V 4A" H 1850 3350 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 1850 3500 50  0001 C CNN
F 3 "" H 1650 3400 50  0001 C CNN
F 4 "SI2316DS-T1-E3" H 1650 3400 50  0001 C CNN "MPN"
	1    1650 3400
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD Q604
U 1 1 60EAA6B6
P 1650 5150
F 0 "Q604" H 1850 5200 50  0000 L CNN
F 1 "NMOS 30V 4A" H 1850 5100 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 1850 5250 50  0001 C CNN
F 3 "" H 1650 5150 50  0001 C CNN
F 4 "SI2316DS-T1-E3" H 1650 5150 50  0001 C CNN "MPN"
	1    1650 5150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R604
U 1 1 60EAA6BC
P 1250 5350
F 0 "R604" H 1050 5400 50  0000 L CNN
F 1 "82k" H 1050 5300 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1180 5350 50  0001 C CNN
F 3 "" H 1250 5350 50  0001 C CNN
	1    1250 5350
	-1   0    0    1   
$EndComp
$Comp
L Device:R R603
U 1 1 60EAA6C2
P 1250 4400
F 0 "R603" H 1050 4450 50  0000 L CNN
F 1 "82k" H 1050 4350 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1180 4400 50  0001 C CNN
F 3 "" H 1250 4400 50  0001 C CNN
	1    1250 4400
	-1   0    0    1   
$EndComp
$Comp
L Device:R R602
U 1 1 60EAA6C8
P 1250 3600
F 0 "R602" H 1050 3650 50  0000 L CNN
F 1 "82k" H 1050 3550 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1180 3600 50  0001 C CNN
F 3 "" H 1250 3600 50  0001 C CNN
	1    1250 3600
	-1   0    0    1   
$EndComp
$Comp
L Device:R R601
U 1 1 60EAA6CE
P 1250 2700
F 0 "R601" H 1050 2750 50  0000 L CNN
F 1 "82k" H 1050 2650 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1180 2700 50  0001 C CNN
F 3 "" H 1250 2700 50  0001 C CNN
	1    1250 2700
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0602
U 1 1 60EAA6D4
P 1250 5500
F 0 "#PWR0602" H 1250 5250 50  0001 C CNN
F 1 "GND" H 1255 5327 50  0000 C CNN
F 2 "" H 1250 5500 50  0001 C CNN
F 3 "" H 1250 5500 50  0001 C CNN
	1    1250 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 5200 1250 5150
Connection ~ 1250 5150
Wire Wire Line
	1250 4600 1250 4550
Connection ~ 1250 4600
Wire Wire Line
	1250 3450 1250 3400
Connection ~ 1250 3400
$Comp
L power:GND #PWR0601
U 1 1 60EAA6E0
P 1250 3750
F 0 "#PWR0601" H 1250 3500 50  0001 C CNN
F 1 "GND" H 1255 3577 50  0000 C CNN
F 2 "" H 1250 3750 50  0001 C CNN
F 3 "" H 1250 3750 50  0001 C CNN
	1    1250 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 2850 1250 2900
Connection ~ 1250 2900
Wire Wire Line
	1750 3150 1750 3200
Wire Wire Line
	1750 4850 1750 4950
Wire Wire Line
	1250 5150 1450 5150
Wire Wire Line
	1250 4600 1450 4600
Wire Wire Line
	1250 3400 1450 3400
Wire Wire Line
	1250 2900 1450 2900
Wire Wire Line
	2800 3150 2200 3150
Wire Wire Line
	2200 4850 2800 4850
$Comp
L Octanis3:SIT1602BI-11-XXS-4.096000 U602
U 1 1 60F042CF
P 8700 5550
F 0 "U602" H 9144 5596 50  0000 L CNN
F 1 "SIT1602BI 4.096 MHz" H 9144 5505 50  0000 L CNN
F 2 "Oscillator:Oscillator_SMD_EuroQuartz_XO32-4Pin_3.2x2.5mm" H 8700 5550 50  0001 C CNN
F 3 "https://abracon.com/Oscillators/ASDMB.pdf" H 9000 6000 50  0001 C CNN
F 4 "SIT1602BI-21-XXE-4.096000" H 8700 5550 50  0001 C CNN "MPN"
	1    8700 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 2500 1250 2500
Wire Wire Line
	1250 2500 1250 2550
Connection ~ 1750 2500
Wire Wire Line
	1750 2500 1750 2700
Wire Wire Line
	1750 4200 1250 4200
Wire Wire Line
	1250 4200 1250 4250
Connection ~ 1750 4200
Wire Wire Line
	1750 4200 1750 4400
$Comp
L Nestbox_v2-rescue:C-device C607
U 1 1 60BA1A20
P 7350 4000
F 0 "C607" H 7465 4046 50  0000 L CNN
F 1 "100n DNP" H 7465 3955 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7388 3850 50  0001 C CNN
F 3 "" H 7350 4000 50  0001 C CNN
F 4 "" H 900 0   50  0001 C CNN "MPN"
	1    7350 4000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7350 3850 7350 3700
Connection ~ 7350 3700
Wire Wire Line
	7350 3700 8000 3700
Wire Wire Line
	7350 4150 7350 4300
Connection ~ 7350 4300
Wire Wire Line
	7350 4300 8000 4300
Wire Wire Line
	10400 4300 9550 4300
Wire Wire Line
	10400 4300 10400 5550
Connection ~ 9550 4300
Text Label 9800 4300 0    60   ~ 0
ADC_CLK
$Comp
L Nestbox_v2-rescue:GND-power1 #PWR0616
U 1 1 60BC499C
P 8800 5850
F 0 "#PWR0616" H 8800 5600 50  0001 C CNN
F 1 "GND" H 8805 5677 50  0000 C CNN
F 2 "" H 8800 5850 50  0001 C CNN
F 3 "" H 8800 5850 50  0001 C CNN
	1    8800 5850
	1    0    0    -1  
$EndComp
$Comp
L Nestbox_v2-rescue:C-device C606
U 1 1 60BC5628
P 6800 5550
F 0 "C606" V 7052 5550 50  0000 C CNN
F 1 "100n" V 6961 5550 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6838 5400 50  0001 C CNN
F 3 "" H 6800 5550 50  0001 C CNN
F 4 "" H -900 2150 50  0001 C CNN "MPN"
	1    6800 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 5400 6800 5250
Wire Wire Line
	6800 5250 8150 5250
Wire Wire Line
	8800 5850 6800 5850
Wire Wire Line
	6800 5850 6800 5700
Connection ~ 8800 5850
Wire Wire Line
	8300 5550 8150 5550
Text HLabel 8000 5550 0    60   Input ~ 0
ADC_CLK_STBY_N
$Comp
L Nestbox_v2-rescue:R-device R614
U 1 1 60BDFBA0
P 9550 4500
F 0 "R614" H 9620 4546 50  0000 L CNN
F 1 "DNP" H 9620 4455 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9480 4500 50  0001 C CNN
F 3 "" H 9550 4500 50  0001 C CNN
	1    9550 4500
	1    0    0    -1  
$EndComp
$Comp
L Nestbox_v2-rescue:GND-power1 #PWR0619
U 1 1 60BE0B5B
P 9550 4650
F 0 "#PWR0619" H 9550 4400 50  0001 C CNN
F 1 "GND" H 9555 4477 50  0000 C CNN
F 2 "" H 9550 4650 50  0001 C CNN
F 3 "" H 9550 4650 50  0001 C CNN
	1    9550 4650
	1    0    0    -1  
$EndComp
$Comp
L Nestbox_v2-rescue:R-device R613
U 1 1 60BE6C4D
P 8150 5400
F 0 "R613" V 8100 5600 50  0000 C CNN
F 1 "DNP" V 8150 5400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 8080 5400 50  0001 C CNN
F 3 "" H 8150 5400 50  0001 C CNN
F 4 "" H -1050 1500 50  0001 C CNN "MPN"
F 5 "" H -1050 1500 50  0001 C CNN "SKU"
	1    8150 5400
	-1   0    0    1   
$EndComp
Connection ~ 8150 5550
Wire Wire Line
	8150 5550 8000 5550
Connection ~ 8150 5250
Wire Wire Line
	8150 5250 8800 5250
$Comp
L Connector_Generic_Shielded:Conn_01x04_Shielded J601
U 1 1 60D21A1E
P 3350 1700
F 0 "J601" H 3500 1750 50  0000 C CNN
F 1 "LOAD_CELL_CONN_01X04" H 3850 1900 50  0000 C CNN
F 2 "lib_fp:MB08MBSAFF04RA" H 3350 1700 50  0001 C CNN
F 3 "" H 3350 1700 50  0001 C CNN
F 4 "M8S-04PMMR-SF8001" H 3350 1700 50  0001 C CNN "MPN"
F 5 "MB08MBSAFF04RA" H 3350 1700 50  0001 C CNN "MPN2"
	1    3350 1700
	-1   0    0    -1  
$EndComp
Text Notes 2900 4750 0    60   ~ 0
Connector Numbering is\naccording to Finecables \ncable assembly \nA2330-CP-T0005
Wire Wire Line
	2800 4850 4050 4850
Wire Wire Line
	4200 3800 4200 3850
Wire Wire Line
	4050 3950 4050 4850
Connection ~ 4050 4850
Wire Wire Line
	4050 4850 5550 4850
Text Label 3950 1600 2    60   ~ 0
EXC_A
Wire Wire Line
	3950 1600 3550 1600
Text Label 4200 3800 0    60   ~ 0
green
Text Label 4200 4200 0    60   ~ 0
white
Text Label 3950 1700 2    60   ~ 0
green
Wire Wire Line
	3950 1700 3550 1700
Text Label 3950 1800 2    60   ~ 0
EXC_B
Wire Wire Line
	3950 1800 3550 1800
Wire Wire Line
	3550 1900 3950 1900
Text Label 3950 1900 2    60   ~ 0
white
Text Notes 2900 1450 0    60   ~ 0
Connector Numbering \nNot confirmed.
Wire Wire Line
	10200 5550 10400 5550
Wire Wire Line
	9900 5550 9100 5550
$Comp
L Nestbox_v2-rescue:R-device R618
U 1 1 60BB7878
P 10050 5550
F 0 "R618" H 10120 5596 50  0000 L CNN
F 1 "0R" H 10120 5505 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9980 5550 50  0001 C CNN
F 3 "" H 10050 5550 50  0001 C CNN
	1    10050 5550
	0    -1   -1   0   
$EndComp
$Comp
L power:VCC #PWR0611
U 1 1 60BC62EC
P 8700 2900
F 0 "#PWR0611" H 8700 2750 50  0001 C CNN
F 1 "VCC" H 8717 3073 50  0000 C CNN
F 2 "" H 8700 2900 50  0001 C CNN
F 3 "" H 8700 2900 50  0001 C CNN
	1    8700 2900
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0615
U 1 1 60BC6AA8
P 8800 5250
F 0 "#PWR0615" H 8800 5100 50  0001 C CNN
F 1 "VCC" H 8817 5423 50  0000 C CNN
F 2 "" H 8800 5250 50  0001 C CNN
F 3 "" H 8800 5250 50  0001 C CNN
	1    8800 5250
	1    0    0    -1  
$EndComp
Connection ~ 8800 5250
$Comp
L Nestbox_v2-rescue:GND-power1 #PWR0107
U 1 1 60F7A4BC
P 3350 2100
F 0 "#PWR0107" H 3350 1850 50  0001 C CNN
F 1 "GND" H 3355 1927 50  0000 C CNN
F 2 "" H 3350 2100 50  0001 C CNN
F 3 "" H 3350 2100 50  0001 C CNN
	1    3350 2100
	1    0    0    -1  
$EndComp
$EndSCHEMATC

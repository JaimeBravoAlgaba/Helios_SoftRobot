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
$Comp
L Device:D_Photo D1
U 1 1 628A8F35
P 6000 4000
F 0 "D1" H 5950 4295 50  0001 C CNN
F 1 "D_Photo" H 5950 4203 50  0000 C CNN
F 2 "SFH2704:SFH2704" H 5950 4203 50  0001 C CNN
F 3 "~" H 5950 4000 50  0001 C CNN
	1    6000 4000
	1    0    0    -1  
$EndComp
Text GLabel 5500 4000 0    50   Input ~ 0
F1
Text GLabel 6500 4000 2    50   Output ~ 0
F2
Text GLabel 7150 3200 0    50   Output ~ 0
L2
Text GLabel 7150 3100 0    50   Input ~ 0
L1
Wire Wire Line
	6500 4000 6100 4000
Wire Wire Line
	5800 4000 5500 4000
$Comp
L Connector_Generic:Conn_02x02_Counter_Clockwise J1
U 1 1 628AA131
P 7450 3600
F 0 "J1" H 7500 3817 50  0001 C CNN
F 1 "Conn_02x02_Counter_Clockwise" H 7500 3725 50  0000 C CNN
F 2 "SFH2704:I-075" H 7450 3600 50  0001 C CNN
F 3 "~" H 7450 3600 50  0001 C CNN
	1    7450 3600
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 628AB1F2
P 7550 3100
F 0 "J2" H 7630 3092 50  0001 L CNN
F 1 "Conn_01x02" H 7630 3046 50  0000 L CNB
F 2 "SFH2704:I-075_wires" H 7550 3100 50  0001 C CNN
F 3 "~" H 7550 3100 50  0001 C CNN
	1    7550 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 3100 7150 3100
Wire Wire Line
	7350 3200 7150 3200
Text GLabel 6950 3600 0    50   Input ~ 0
F1
Wire Wire Line
	7250 3600 6950 3600
Text GLabel 8150 3600 2    50   Output ~ 0
L2
Wire Wire Line
	8150 3600 7750 3600
Text GLabel 7050 3700 0    50   Input ~ 0
F2
Wire Wire Line
	7250 3700 7050 3700
Text GLabel 7950 3700 2    50   Output ~ 0
L1
Wire Wire Line
	7750 3700 7950 3700
$EndSCHEMATC

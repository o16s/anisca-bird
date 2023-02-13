
Device Status
===================================


While the anisca device is running, ystem status data is continuously logged and stored on the onboard memory ("offline"). Some logs are also transmitted wirelessly via SIGFOX ("uplink" - if coverage is available).

Logging frequency:

* offline: every 3 minutes
* uplink: 3x per day

For more details on the logged data fields see: :ref:`Status <Status Message>`.


Battery Voltage
--------

The battery voltage is measured directly on the mainboard using a resistive divider and an onboard ADC. The reference voltage is VCC, generated from the 3.3V buck converter.

The ADC value is scaled in order to indicate the battery voltage in mV.

Limits and Thresholds
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Rechargeable NiMH batteries should not be discharged below 0.9V per cell. Depending on the measured battery voltage, the device may decide to go to deep-sleep mode in order to protect the batteries.

Below are the hard-coded voltage values that are used to characterize the batteries' charging state. Note that 4x NiMH are powering the system (nominal 4 x 1.2 V)

* VBAT_FULL: 5350 mV. 
  Batteries are considered to be 100% charged
* VBAT_EMPTY: 3600 mV. 
  Batteries are considered to be 0% charged
* VBAT_SHUTDOWN: 3800 mV. When below this voltage, the system will go to deep sleep and weight logging and message transmission is disabled.
* VBAT_STARTUP_MIN: 4200 mV. At boot, the battery voltage must be at least this value in order for the device to complete start-up, else it will go directly to deep sleep. This means, the inserted batteries must be charged to at least this value.



Uptime 
--------
Number of days since last reboot. A reboot happens when the batteries get replaced and upon an unexpected error. If the number of days displayed in the uptime field is smaller than the number of days since the last battery replacement, an unexpected error may have occured. Please service the device if this is observed multiple times. 


Memory usage
--------
The onboard-memory can store up to 3 million log messages. The memory usage is reported as percentage (100 = memory full). The memory can be erased using the anisca App. 


Clock difference
--------
The on-board clock may drift by several seconds per day. If the device has SIGFOX connectivity and the clock difference is too high, a clock update is performed once per day. The clock can also be updated by connecting to the device via USB and using the anisca App.
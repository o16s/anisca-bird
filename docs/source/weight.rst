
Weight Measurement
===================================


Any bird weighing event is recorded and stored to memory as a new log entry. In addition, continuous offset measurements and calibration data are logged. The most important logs are also transmitted wirelessly via SIGFOX (if coverage is available).


Bird Weight
--------
Anisca Bird continuously measures the weigth present on the wooden perch ("Detection"). It does so in a low-power fashion until the measured weight is above WAKE_UP_THRESHOLD. The device then attempts to read the RFID tag of a potential bird present. If successful, the weight is measured continuously ("Acquisition") for as long as it stays above WAKE_UP_THRESHOLD.

Detection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

During detection, the following hard-coded constants are used:

* Low-power detection interval: 1 second
* WAKE_UP_THRESHOLD: 50 grams [#f1]_
* RFID read attempts: max. 10x in 3 second intervals. If unsuccessful after this, system goes back to sleep.


Acquisition
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
After a bird was detected and a valid RFID was read out, its weight is sampled continuously. Multiple samples are taken into account 
because the bird may be moving and hence an instantaneous measurement would not provide a reliable weight value. 

The final **raw weight** value is obtained as a result of 3 acquisition steps:

1. **Sub-samples**: The load cell ADC is sampling the weight at 10 Hz. 
2. **Samples**: Always 10 sub-samples are averaged to create one sample. A standard deviation on the 10 sub-samples is calculated. 

    a. If the sub-sample standard deviation for a given group of 10 exceeds MAX_SUB_STDEV, the sample gets discarded. This ensures only samples where the bird was sitting still will be considered.
    b. If a sample is MAX_SAMPLE_DIFF larger than the previous sample, then the previous sample gets discarded. This ensures all samples to be on a relatifely flat line and removes outliers.

3. **Median**: A maximum of N_SAMPLES is collected as described in points 1 & 2. If the bird leaves before reaching N_SAMPLES, a smaller number of samples will be available.

    a. The median value over all collected samples is calculated. This is the retained **raw weight** value.
    b. The standard deviation over all collected samples is calculated. This is the retained **raw stddev** value.


Hard-coded parameters:

* MAX_SUB_STDEV: 5 grams [#f1]_
* MAX_SAMPLE_DIFF: 5 grams [#f1]_
* N_SAMPLES: 19



Processing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The **raw weight** and **raw stddev** values obtained above are unitless ADC values. They need to be converted to grams using calibration data in order to present meaningful weight data. This is done as follows:

1. The last recorded **raw offset** value is subtracted from the **raw weight**, in order to receive the **net raw weight**
2. **net raw weight** and  **raw stddev** are divided by the most recently measured **calibration slope** to obtain weight and standard deviation of the sample in grams.


Logging
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
For more details on what data is logged or transmitted, see: :ref:`Weight <Weight Message>`.





Offset measurement
--------

The **raw offset** is measured in the exact same way as the bird weight, except that it happens in a periodic manner whenever the weight detection stays below the WAKE_UP_THRESHOLD. 

The following parameters are hard-coded:

* N_OFFSET_SAMPLES: 11
* Offset acquisition & logging period: 3 minutes
* Offset uplink period (transmitted via SIGFOX): 1-2 hours


Calibration 
--------

The calibration is a linear regression between raw ADC values and the weight placed on the perch in grams. We denote the following terms:

* **Slope (a)**: the multiplication factor between grams and raw ADC value
* **Intercept (b)** the calculated raw ADC value when zero weight is placed on the perch. Theoretically, this is the offset value during calibration.
* **R^2** the R-square of the linear regression.


When performing a calibration routine, both a and b are calculated automatically by the device and stored for internal use. All subsequent weight measurements are processed using these most up-to-date calibraion values.

It is known that the calibration values can change over time as the physical state of the sytstem changes. Therefore, frequent re-calibrations of the system are advised.


Temperature Dependency
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is known that notably the calibration slope is temperature dependent. This dependency is not compensated for by the device. It is adviseed to perform a temperature compensation of the logged weight values once the relationship between temperature and slope has been established in a post-processing step. 


.. rubric:: Footnotes
.. [#f1] Note: the values in grams are internally converted into raw ADC values by using the last available calibration slope.
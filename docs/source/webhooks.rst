
Webhooks
===================================


The primary method that Anisca Bird uses to deliver data is through the use of webhooks.
A webhook URL will be called via a POST request with the message encoded as a JSON body. 


Messages
--------

Your webhook URL should be prepared to receive the following messages:


``weight``: Bird detected and weighed
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This message is sent when a ringed bird is detected on the perch. A detection is valid if the RFID tag was successful read.

.. http:post:: example.com/webhook

    **Example request body**:

    .. sourcecode:: json

        {
            "timestamp": 1648642396,
            "serial": "CODEABFFEE",
            "device_id": "secret-red-wake",
            "type": "weight",

            "activation": {

                "latlng": "46.535,6.569",
                "site_id": "rdc20",
                "activation_date": "2022-05-02 17:32:50"
            },

            "body": {

                "duration": 12,
                "flags":"ab",
                "weight": 120.5,
                "uid": "0600c9a463",
                "raw_adc_pos": 56020,
                "stddev": 1,
                "temperature": -10
            }
        }

    :<json integer timestamp: Epoch timestamp (UTC+0): seconds since January 1, 1970 12:00:00 AM
    :<json string serial: device serial number
    :<json string device_id: human readable hash of serial number that is also printed on device label
    :<json string type: message type (one of ``weight``, ``status``, ``offset``, or ``calibration``)
    :<json string activation: when and where this device was last activated

    :<json integer duration: number of seconds that owl is on perch (max. 240)
    :<json integer flags: reserved
    :<json integer weight: weight estimate in 0.01 grams (unsigned value)
    :<json integer uid: RFID UID lower 4 bytes
    :<json integer raw_adc_pos: raw ADC value from "positive excitation" (incl. offset)
    :<json integer stddev: weight measurement series standard deviation in 0.1 grams
    :<json integer temperature: temperature in degrees Celsius (signed value)





``status``: Device status
~~~~~~~~~~~~~~~~~~~~~~~~~~

Device status is sent twice a day.

.. http:post:: example.com/webhook

    **Example request body**:

    .. sourcecode:: json

        {
            "timestamp": 1648642396,
            "serial": "CODEABFFEE",
            "device_id": "secret-red-wake",
            "type": "status",

            "activation": {

                "latlng": "46.535,6.569",
                "site_id": "rdc20",
                "activation_date": "2022-05-02 17:32:50"
            },

            "body": {

                "header": "ff",
                "events_with_id": 29,
                "events_unknown_id": 16,
                "flags": "ab",
                "vbat_mv": 4620,
                "memory_used": 56,
                "days_since_boot": 120
            }
        }

    :<json integer timestamp: Epoch timestamp (UTC+0): seconds since January 1, 1970 12:00:00 AM
    :<json string serial: device serial number
    :<json string device_id: human readable hash of serial number that is also printed on device label
    :<json string type: message type (one of ``weight``, ``status``, ``offset``, or ``calibration``)
    :<json string activation: when and where this device was last activated

    :<json integer header: internal device status header (0xFF)
    :<json integer events_with_id: number of events with successful RFID detection since last status update [0-255]. The value of 255 means >=255.
    :<json integer events_unknown_id: number of events with UN-successful RFID detection since last status update [0-255]. The value of 255 means >=255.
    :<json integer flags: reserved
    :<json integer vbat_mv: Battery voltage in mV
    :<json integer memory_used: Percentage of Flash memory used [0-100]
    :<json integer days_since_boot: Number of days since last reboot [0-255]. The value of 255 means >=255.




``offset``: Tare weight
~~~~~~~~~~~~~~~~~~~~~~~~~~

This is the tare weight (unladen weight) and is sent every 2 hours.

.. http:post:: example.com/webhook

    **Example request body**:

    .. sourcecode:: json

        {
            "timestamp": 1648642396,
            "serial": "CODEABFFEE",
            "device_id": "secret-red-wake",
            "type": "offset",
            
            "activation": {

                "latlng": "46.535,6.569",
                "site_id": "rdc20",
                "activation_date": "2022-05-02 17:32:50"
            },

            "body": {

                "header": "fa",
                "flags": "ac",
                "weight": 31660,
                "raw_adc_pos": 4999,
                "raw_adc_neg": 4986,
                "stddev": 0,
                "temperature": 20
            }
        }

    :<json integer timestamp: Epoch timestamp (UTC+0): seconds since January 1, 1970 12:00:00 AM
    :<json string serial: device serial number
    :<json string device_id: human readable hash of serial number that is also printed on device label
    :<json string type: message type (one of ``weight``, ``status``, ``offset``, or ``calibration``)
    :<json string activation: when and where this device was last activated

    :<json integer header: internal device status header (0xFA)
    :<json integer flags: reserved
    :<json integer raw_adc_pos: raw tare ADC value from "positive excitation"
    :<json integer raw_adc_neg: raw tare ADC value from "negative excitation"
    :<json integer stddev: tare measurement series standard deviation in 0.1 grams
    :<json integer temperature: temperature in degrees Celsius (signed value)


``calibration``: Calibration result
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This message is sent after the user has performed a calibration.

.. http:post:: example.com/webhook

    **Example request body**:

    .. sourcecode:: json

        {
            "timestamp": 1648642396,
            "serial": "CODEABFFEE",
            "device_id": "secret-red-wake",
            "type": "calibration",

            "activation": {

                "latlng": "46.535,6.569",
                "site_id": "rdc20",
                "activation_date": "2022-05-02 17:32:50"
            },

            "body": {

                "slope": 29,
                "intercept": 25,
                "temperature": 2,
                "r2": 1.0
            }
        }

    :<json integer timestamp: Epoch timestamp (UTC+0): seconds since January 1, 1970 12:00:00 AM
    :<json string serial: device serial number
    :<json string device_id: human readable hash of serial number that is also printed on device label
    :<json string type: message type (one of ``weight``, ``status``, ``offset``, or ``calibration``)
    :<json string activation: when and where this device was last activated

    :<json integer slope: linear regression slope: (raw ADC counts per 10mg)
    :<json integer intercept: linear regression intercept: (raw ADC value)
    :<json float r2: linear regression R^2
    :<json integer temperature: temperature in degrees Celsius (signed value)
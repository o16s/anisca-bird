# Anisca Bird

![Anisca Bird](docs/source/220414_OCT_AniscaBird294.jpg)

## Overview

Anisca Bird is a smart weighing and tracking device developed by Octanis Instruments in collaboration with the Swiss Ornithological Institute. The system replaces a bird's regular roosting place with a weighing and tracking device that can register a bird's identity and body weight throughout the year without causing stress to the animal.

This repository contains the firmware, PCB design files, and mechanical designs to build and operate the Anisca Bird system.

## Key Features

- **RFID Identification**: Automatically reads passive RFID tags attached to birds
- **Weight Measurement**: Precision load cell with 1g resolution when calibrated
- **Data Storage**: Onboard memory for up to 18 years of weight records
- **Wireless Connectivity**: Optional Sigfox connectivity for remote data transmission
- **USB Connection**: Easy data transfer via USB-C port
- **Long Battery Life**: Up to 1 year with the external battery box
- **Modular Design**: Easy to modify, repair, or upgrade

## Repository Structure

- **`/docs`**: Documentation files for the Anisca Bird system
- **`/mechanical`**: Mechanical design files (STEP format)
- **`/nestbox-electronics-master`**: PCB design files for the main board, RFID antenna, and battery box
  - **`/LF_Antenna_v3`**: RFID antenna PCB design
  - **`/Nestbox_v3_3*`**: Main board iterations
  - **`/battery_box`**: Battery box PCB design
  - **`/lib_fp.pretty`**: KiCad footprint libraries
  - **`/lib_sch`**: KiCad schematic libraries
- **`/nestbox-firmware-REV3.3`**: Firmware source code
  - **`/nestbox-fw`**: Core firmware files
  - **`/Core`**: Main application code
  - **`/Drivers`**: Hardware driver files
  - **`/Middlewares`**: Middleware components
  - **`/USB_DEVICE`**: USB communication stack

## Hardware Overview

The Anisca Bird system consists of three main components:

1. **Reader**: Contains the main PCB with microcontroller, load cell interface, RFID reader, flash memory, and connectivity options. Housed in a water-resistant aluminum case.

2. **Perch**: Varnished plywood perch with integrated 125kHz RFID antenna for tag reading with up to 10cm range.

3. **Battery Box**: External battery case with four 10Ah NiMH D-cells providing up to 1 year of operation.

## Firmware Overview

The firmware is built for STM32L433 microcontroller and provides the following functionality:

- RFID tag detection and decoding
- Weight measurement and analysis
- Low-power operation for extended battery life
- USB communication for data retrieval and configuration
- Optional Sigfox connectivity for remote data transmission

## License

Where not otherwise specified, the [GPL License](LICENSE) applies.

## Contact

For commercial support or to request an Anisca Bird system, please contact:

Octanis Instruments GmbH  
Europa-Strasse 33, 8152 Glattbrugg, Switzerland  
info@octanis.ch  
www.octanis.ch
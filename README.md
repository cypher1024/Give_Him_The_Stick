# Give him the stick. Don't give him the stick!
## A CAN bus translator for cheap MCP2515 modules from eBay

**Contents:**
- CAN_Camera_Converter - this is probably what you want. It translates CAN frames
- Haltech_Simulator - creates Haltech-style frames for testing

**Getting started:**
- Clone this repo or download it as a .zip and extract it
- Download and install [this Arduino MCP2515 library](https://github.com/autowp/arduino-mcp2515#library-installation) 
- Compile the firmware and flash it to your arduino of choice
- Swap a 20 MHz crystal and 15 pF capacitors onto the MCP2515 module (the standard 8 MHz crystal isn't fast enough for 1 Mbps). If you want to use the standard 8 MHz crystal for testing, you'll need to set the CLOCK constant to MCP_8MHZ
- Connect the MCP2515 module. If you're using an Uno, use the following pins:
  - Arduino GND -> MCP2515 module GND
  - Arduino D10 -> MCP2515 module CS
  - Arduino D11 -> MCP2515 module SI
  - Arduino D12 -> MCP2515 module SO
  - Arduino D11 -> MCP2515 module SCK

Datasheet for the MCP2515: http://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf

# Give him the stick. Don't give him the stick!
## A CAN bus translator for cheap MCP2515 modules from eBay

**Contents:**
- CAN_Camera_Converter - this is probably what you want. It translates CAN frames
- Haltech_Simulator - creates Haltech-style frames for testing

**Getting started:**
- Clone this repo or download it as a .zip and extract it
- Download and install [this Arduino MCP2515 library](https://github.com/autowp/arduino-mcp2515#library-installation) 
- Compile the firmware and flash it to your arduino of choice
- Swap a 20 MHz crystal and 15 pF capacitors onto the MCP2515 modules (the standard 8 MHz crystal isn't fast enough for 1 Mbps). If you want to use the standard 8 MHz crystal for testing, you'll need to set the CLOCK constant to MCP_8MHZ
- Connect the MCP2515 modules. If you're using an Uno, use the following pins:
  - Uno GND -> Both MCP2515 GND
  - Uno  D9 -> Transmitter MCP2515 CS
  - Uno D10 -> Receiver MCP2515 CS
  - Uno D11 -> Both MCP2515 module SI
  - Uno D12 -> Both MCP2515 module SO
  - Uno D11 -> Both MCP2515 module SCK
- For a Mega 2560:
  - Mega 2560 GND -> Both MCP2515 GND
  - Mega 2560  D9 -> Transmitter MCP2515 CS
  - Mega 2560 D10 -> Receiver MCP2515 CS
  - Mega 2560 D50 -> Both MCP2515 module SO
  - Mega 2560 D51 -> Both MCP2515 module SI
  - Mega 2560 D52 -> Both MCP2515 module SCK

Datasheet for the MCP2515: http://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf

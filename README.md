# Give him the stick. Don't give him the stick!
## A CAN bus translator for cheap MCP2515 modules from eBay

**Contents:**
- CAN_Camera_Converter - this is probably what you want. It translates CAN frames
- Haltech_Simulator - Creates Haltech-style frames for testing

**Getting started:**
- Clone this repo or download it as a .zip and extract it
- Download and install [this Arduino MCP2515 library](https://github.com/autowp/arduino-mcp2515#library-installation) 
- Compile the firmware and flash it to your arduino of choice
- Connect the MCP2515 module. If you're using an Uno, use the following pins:
  - Arduino GND -> MCP2515 module GND
  - Arduino D10 -> MCP2515 module CS
  - Arduino D11 -> MCP2515 module SI
  - Arduino D12 -> MCP2515 module SO
  - Arduino D11 -> MCP2515 module SCK

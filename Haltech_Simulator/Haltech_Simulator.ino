#include <SPI.h>
#include <mcp2515.h>  // https://github.com/autowp/arduino-mcp2515

// Send messages as fast as possible to test bandwidth
//#define PORKCHOP_SANDWICHES

const CAN_SPEED SPEED = CAN_1000KBPS;
const CAN_CLOCK CLOCK = MCP_20MHZ;

// Pin assignments
const uint8_t MCP2515_CHIP_SELECT = 10;

struct can_frame rpm_throttle_packet;
struct can_frame speed_packet;
struct can_frame gear_packet;
MCP2515 mcp2515(MCP2515_CHIP_SELECT);


void setup() {
    rpm_throttle_packet.can_id  = 0x360;
    rpm_throttle_packet.can_dlc = 8;
    rpm_throttle_packet.data[0] = 0x13; // RPM
    rpm_throttle_packet.data[1] = 0x37; // RPM
    rpm_throttle_packet.data[2] = 0x0;
    rpm_throttle_packet.data[3] = 0x0;
    rpm_throttle_packet.data[4] = 0xBE; // Throttle
    rpm_throttle_packet.data[5] = 0xEF; // Throttle
    rpm_throttle_packet.data[6] = 0x0;
    rpm_throttle_packet.data[7] = 0x0;
    
    speed_packet.can_id  = 0x36C;
    speed_packet.can_dlc = 8;
    speed_packet.data[0] = 0x0;
    speed_packet.data[1] = 0x0;
    speed_packet.data[2] = 0x05; // Speed
    speed_packet.data[3] = 0x13; // Speed
    speed_packet.data[4] = 0x0;
    speed_packet.data[5] = 0x0;
    speed_packet.data[6] = 0x0;
    speed_packet.data[7] = 0x0;
    
    gear_packet.can_id  = 0x370;
    gear_packet.can_dlc = 8;
    gear_packet.data[0] = 0x0;
    gear_packet.data[1] = 0x0;
    gear_packet.data[2] = 0xCA; // Gear
    gear_packet.data[3] = 0xFE; // Gear
    gear_packet.data[4] = 0x0;
    gear_packet.data[5] = 0x0;
    gear_packet.data[6] = 0x0;
    gear_packet.data[7] = 0x0;
    
    while (!Serial);
    Serial.begin(115200);
    
    mcp2515.reset();
    mcp2515.setBitrate(SPEED, CLOCK);
    mcp2515.setNormalMode();

    Serial.println(F("Finished setup"));
}


void loop(){
    #ifdef PORKCHOP_SANDWICHES
        mcp2515.sendMessage(&rpm_throttle_packet);
        mcp2515.sendMessage(&speed_packet);
        mcp2515.sendMessage(&gear_packet);
        return;
    #else
        static uint32_t last_loop = 0;
        if (millis() - last_loop >= 1000){
            last_loop = millis();
            
            mcp2515.sendMessage(&rpm_throttle_packet);
            Serial.print(F("\nSent: ")); print_message(&rpm_throttle_packet);
            
            mcp2515.sendMessage(&speed_packet);
            Serial.print(F("Sent: ")); print_message(&speed_packet);
            
            mcp2515.sendMessage(&gear_packet);
            Serial.print(F("Sent: ")); print_message(&gear_packet);
        }
    #endif
}

#include <SPI.h>
#include <mcp2515.h>  // https://github.com/autowp/arduino-mcp2515

// Use the 'receiver' module for both receiving and transmitting
#define SINGLE_MODULE_MODE

// Master debugging switch
#define SERIAL_DEBUG_ENABLED

// Limit messages about CAN frames to 1Hz
// #define DEBUG_RATE_LIMIT

// Timing output on pin 2
// #define TIMING_PULSES_ENABLED

// CAN bus configuration
const CAN_SPEED RECEIVER_SPEED = CAN_1000KBPS;
const CAN_CLOCK RECEIVER_CLOCK = MCP_20MHZ;
const CAN_SPEED TRANSMITTER_SPEED = CAN_1000KBPS;
const CAN_CLOCK TRANSMITTER_CLOCK = MCP_20MHZ;

// IDs to allow through the filter
const uint32_t ID_LIST[] = {
    0x360,  // Haltech RPM & throttle frame
    0x36C,  // Haltech speed frame
    0x370   // Haltech gear frame
};

// Pin assignments
const uint8_t TIMING_PULSE_OUTPUT = 2;
#define       TIMING_PULSE_REGISTER PORTD
#define       TIMING_PULSE_PINMASK  PD2
const uint8_t TRANSMITTER_CHIP_SELECT = 9;
const uint8_t RECEIVER_CHIP_SELECT = 10;
const uint8_t UNO_SPI_MOSI = 11;
const uint8_t UNO_SPI_MISO = 12;
const uint8_t UNO_SPI_SCK  = 13;
const uint8_t MEGA_SPI_MISO = 50;
const uint8_t MEGA_SPI_MOSI = 51;
const uint8_t MEGA_SPI_SCK  = 52;

MCP2515 receiver(RECEIVER_CHIP_SELECT);
MCP2515 transmitter(TRANSMITTER_CHIP_SELECT);

#ifdef SERIAL_DEBUG_ENABLED
    bool show_message = true;
#endif


void set_id_filters() {
    // This builds a filter and mask based on the common bits from every ID value
    // e.g. 10100111001  ID 0
    //      00110110100  ID 1
    //      00001110101  ID 2
    //      01000110010  Mask
    //      00000110000  Filter
    // Note that filters and their masks are always 29 bits, even when applied to
    // standard 11 bit ID frames. 16 of those bits will match the first 16 bits of the
    // data if their associated mask bits are set. It's best to avoid that
    // For an 11 bit ID:
    // <--  MSB                  LSB -->
    //                       [   id    ]
    // 010101010101010101111 00011110000
    
    receiver.setConfigMode();  // Must be in config mode to set filters
    uint32_t id_filter = 0;
    uint32_t id_mask = 0;
    for (uint8_t id_num = 0; id_num < sizeof(ID_LIST) / 4; id_num++) {
        #ifdef SERIAL_DEBUG_ENABLED
            Serial.print(F("Adding ID 0x")); Serial.print(ID_LIST[id_num], HEX);
            Serial.println(F(" to the filter..."));
        #endif
        
        if (id_num == 0) {
            id_filter = ID_LIST[id_num];
            id_mask = 0b11111111111;
        }
        else {
            for (uint8_t bit_num = 0; bit_num < 11; bit_num++) {
                if (bitRead(id_mask, bit_num)) {
                    // Mask bit for this position is still enabled
                    if (bitRead(id_filter, bit_num) != bitRead(ID_LIST[id_num], bit_num)) {
                        // Bit at position bit_num differs from that bit in the filter
                        // Disable that mask bit
                        bitClear(id_mask, bit_num);
                    }
                }
            }
        }
    }
    
    // Set the filters & masks for both RX buffers to the same thing
    // This can be changed if we need more throughput (or finer filtering) later
    receiver.setFilterMask(MCP2515::MASK0, false, id_mask); // RXB0
    receiver.setFilter(MCP2515::RXF0, false, id_filter);    // RXB0
    receiver.setFilterMask(MCP2515::MASK1, false, id_mask); // RXB1
    receiver.setFilter(MCP2515::RXF2, false, id_filter);    // RXB1
}


void setup() {
    //  Set up the serial port (if required)
    #ifdef SERIAL_DEBUG_ENABLED
      while (!Serial);
      Serial.begin(115200);
    #endif

    // Set up the timing pin
    #ifdef TIMING_PULSES_ENABLED
      pinMode(TIMING_PULSE_OUTPUT, OUTPUT);
      digitalWrite(TIMING_PULSE_OUTPUT, HIGH);
    #endif

    // Set up MCP2515 modules
    receiver.reset();
    MCP2515::ERROR set_receiver_bitrate_result = receiver.setBitrate(RECEIVER_SPEED, RECEIVER_CLOCK);
    set_id_filters();
    receiver.setNormalMode();
    #ifndef SINGLE_MODULE_MODE
        transmitter.reset();
        MCP2515::ERROR set_transmitter_bitrate_result = transmitter.setBitrate(TRANSMITTER_SPEED, TRANSMITTER_CLOCK);
        transmitter.setNormalMode();
    #else
        MCP2515::ERROR set_transmitter_bitrate_result = MCP2515::ERROR_OK;
    #endif

    #ifdef SERIAL_DEBUG_ENABLED
        if (set_receiver_bitrate_result == MCP2515::ERROR_OK && set_transmitter_bitrate_result == MCP2515::ERROR_OK) {
            Serial.println(F("Sucessfully set bitrate"));
        }
        else if (set_receiver_bitrate_result != MCP2515::ERROR_OK){
            Serial.println(F("Failed to set receiver bitrate. Is the MCP2515 connected?"));
        }
        else{
            Serial.println(F("Failed to set transmitter bitrate. Is the MCP2515 connected?"));
        }
        Serial.println(F("Finished setup"));
    #endif
}


void loop() {
    #ifdef SERIAL_DEBUG_ENABLED
        #ifdef DEBUG_RATE_LIMIT
          static uint32_t last_debug_message_time = 0;
          if (millis() - last_debug_message_time >= 1000) {
            show_message = true;
            last_debug_message_time = millis();
          }
          else {
            show_message = false;
          }
        #endif
    #endif

    // Check for a new frame
    static struct can_frame incoming_frame;
    if (receiver.readMessage(&incoming_frame) == MCP2515::ERROR_OK) {
        #ifdef SERIAL_DEBUG_ENABLED
            if (show_message) {
              Serial.println(); Serial.print(F("Received: "));
              Serial.print(F("ID: ")); Serial.print(incoming_frame.can_id, HEX);
              Serial.print(F(", DLC:")); Serial.print(incoming_frame.can_dlc, HEX);
              Serial.print(F(", Data: "));
              for (int i = 0; i < incoming_frame.can_dlc; i++)  {
                  Serial.print(incoming_frame.data[i], HEX);
                  Serial.print(F(" "));
              }
              Serial.println();
            }
        #endif
        
        // Process the new frame
        #ifdef SINGLE_MODULE_MODE
            handle_haltech_frame(&incoming_frame, &receiver);
        #else
            handle_haltech_frame(&incoming_frame, &transmitter);
        #endif
    }

    // Output timing pulse
    #ifdef TIMING_PULSES_ENABLED
        bitSet(TIMING_PULSE_REGISTER, TIMING_PULSE_PINMASK);
        delayMicroseconds(10);
        bitClear(TIMING_PULSE_REGISTER, TIMING_PULSE_PINMASK);
    #endif
}

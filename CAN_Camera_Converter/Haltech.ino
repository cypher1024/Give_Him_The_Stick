
void handle_haltech_rpm_throttle_frame(const struct can_frame *haltech_frame, MCP2515 *can_interface){
    struct can_frame motec_frame;
    motec_frame.can_id  = 0x640;
    motec_frame.can_dlc = 8;
    motec_frame.data[0] = haltech_frame->data[0];  // RPM
    motec_frame.data[1] = haltech_frame->data[1];  // RPM
    motec_frame.data[2] = 0x0;
    motec_frame.data[3] = 0x0;
    motec_frame.data[4] = 0x0; 
    motec_frame.data[5] = 0x0;  
    motec_frame.data[6] = haltech_frame->data[4];  // Throttle
    motec_frame.data[7] = haltech_frame->data[5];  // Throttle
    can_interface->sendMessage(&motec_frame);
    #ifdef SERIAL_DEBUG_ENABLED
        if (show_message){
            uint16_t rpm = ((uint16_t)haltech_frame->data[0] << 8) | haltech_frame->data[1];
            uint16_t throttle = ((uint16_t)haltech_frame->data[4] << 8) | haltech_frame->data[5];
            Serial.print(F("Sent translated RPM (0x")); Serial.print(rpm, HEX); Serial.print(F(") & throttle (0x"));
            Serial.print(throttle, HEX); Serial.println(F(") frame"));
        }
    #endif
}


void handle_haltech_speed_frame(const struct can_frame *haltech_frame, MCP2515 *can_interface){
    struct can_frame motec_frame;
    motec_frame.can_id  = 0x659;
    motec_frame.can_dlc = 8;
    motec_frame.data[0] = 0x0;
    motec_frame.data[1] = 0x0;
    motec_frame.data[2] = 0x0;
    motec_frame.data[3] = 0x0;
    motec_frame.data[4] = haltech_frame->data[2];  // Speed
    motec_frame.data[5] = haltech_frame->data[3];  // Speed
    motec_frame.data[6] = 0x0;
    motec_frame.data[7] = 0x0;
    can_interface->sendMessage(&motec_frame);
    #ifdef SERIAL_DEBUG_ENABLED
        if (show_message){
            uint16_t speed = (((uint16_t)haltech_frame->data[2]) << 8) | haltech_frame->data[3];
            Serial.print(F("Sent translated speed (0x")); Serial.print(speed, HEX); Serial.println(F(") frame"));
        }
    #endif
}


void handle_haltech_gear_frame(const struct can_frame *haltech_frame, MCP2515 *can_interface){
    struct can_frame motec_frame;
    motec_frame.can_id  = 0x64D;
    motec_frame.can_dlc = 8;
    motec_frame.data[0] = 0x0;
    motec_frame.data[1] = 0x0;
    motec_frame.data[2] = 0x0;
    motec_frame.data[3] = 0x0;
    motec_frame.data[4] = 0x0; 
    motec_frame.data[5] = 0x0;  
    motec_frame.data[6] = haltech_frame->data[3] << 4;  // Gear
    motec_frame.data[7] = 0x0;
    can_interface->sendMessage(&motec_frame);
    #ifdef SERIAL_DEBUG_ENABLED
        if (show_message){
            Serial.print(F("Sent translated gear (0x")); Serial.print(haltech_frame->data[3]&0xF, HEX); 
            Serial.println(F(") frame"));
        }
    #endif
}


// Returns true if we've processed this frame and sent a translated version
bool handle_haltech_frame(const struct can_frame *incoming_frame, MCP2515 *can_interface){
    // Reject any packets with the wrong size payload
    if (incoming_frame->can_dlc != 8){
        return false;
    }
    
    switch (incoming_frame->can_id){
        case 0x360:
            handle_haltech_rpm_throttle_frame(incoming_frame, can_interface);
            return true;
        case 0x36C:
            handle_haltech_speed_frame(incoming_frame, can_interface);
            return true;
        case 0x370:
            handle_haltech_gear_frame(incoming_frame, can_interface);
            return true;
    }
    return false;
}


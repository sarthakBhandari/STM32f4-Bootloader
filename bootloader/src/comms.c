#include "comms.h"
#include "core/uart.h"
#include "core/crc8.h"

#define PACKET_BUFFER_LENGTH    (8)

typedef enum comms_state_t {
    CommsState_length,
    CommsState_Data,
    CommsState_CRC
} comms_state_t;

static uint8_t data_byte_count = 0;
static comms_state_t state = CommsState_length;

static comms_packet_t temporary_packet = {.length = 0 , .data = {0}, .crc = 0};
static comms_packet_t retx_packet = {.length = 0, .data = {0}, .crc = 0};
static comms_packet_t ack_packet = {.length = 0, .data = {0}, .crc = 0};
static comms_packet_t last_transmitted_packet = {.length = 0, .data = {0}, .crc = 0};

static comms_packet_t packet_buffer[PACKET_BUFFER_LENGTH];
static uint32_t packet_read_index = 0;
static uint32_t packet_write_index = 0;
static uint32_t packet_buffer_mask = PACKET_BUFFER_LENGTH - 1;

void comms_setup(void){
    retx_packet.length = 1;
    retx_packet.data[0] = PACKET_RETX_DATA0;
    for (uint8_t i = 1; i < PACKET_DATA_BYTES; i++){
        retx_packet.data[i] = 0xff;
    }
    retx_packet.crc = comms_compute_crc(&retx_packet);

    ack_packet.length = 1;
    ack_packet.data[0] = PACKET_ACK_DATA0;
    for (uint8_t i = 1; i < PACKET_DATA_BYTES; i++){
        ack_packet.data[i] = 0xff;
    }
    ack_packet.crc = comms_compute_crc(&ack_packet);

}

static void comms_packet_copy(const comms_packet_t *source, comms_packet_t *dest){
    dest->length = source->length;
    for (uint8_t i = 0; i < PACKET_DATA_BYTES; i++){
        dest->data[i] = source->data[i];
    }
    dest->crc = source->crc;
}

static bool comms_is_single_byte_packet(const comms_packet_t* packet, const uint8_t byte){
    if (packet->length != 1){
        return false;
    }
    if(packet->data[0] != byte){
        return false;
    }
    for (uint8_t i = 1; i < PACKET_LENGTH_BYTES; i++){
        if (packet->data[i] != 0xff){
            return false;
        }
    }
    return true;
}

void comms_update(void){
    if (uart_data_available()){
        switch (state){
            case CommsState_length:
                temporary_packet.length = uart_read_byte();
            break;
            case CommsState_Data:
                temporary_packet.data[data_byte_count++] = uart_read_byte();
                if (data_byte_count >= PACKET_DATA_BYTES){
                    data_byte_count = 0;
                    state = CommsState_CRC;
                }
            break;
            case CommsState_CRC:
                temporary_packet.crc = uart_read_byte();
                if (comms_compute_crc(&temporary_packet) != temporary_packet.crc){
                    comms_write(&retx_packet);
                    state = CommsState_length;
                    break;
                }

                if (comms_is_single_byte_packet(&temporary_packet, PACKET_RETX_DATA0)){
                    comms_write(&last_transmitted_packet);
                    state = CommsState_length;
                    break;
                }

                if (comms_is_single_byte_packet(&temporary_packet, PACKET_ACK_DATA0)){
                    state = CommsState_length;
                    break;
                }

                uint32_t next_write_index = (packet_write_index + 1) & packet_buffer_mask;
                if (next_write_index != packet_read_index){
                    __asm__("BKPT #0");
                }

                comms_packet_copy(&temporary_packet, &packet_buffer[packet_write_index]);
                packet_write_index = next_write_index;
                comms_write(&ack_packet);

            break;
            default: 
                state = CommsState_length;
        }
    }
}

void comms_write(comms_packet_t *packet){
    uart_write((uint8_t *)(packet), PACKET_SIZE);
}

void comms_read(comms_packet_t *packet){
    comms_packet_copy(&packet_buffer[packet_read_index], packet);
    packet_read_index = (packet_read_index + 1) & packet_buffer_mask;
}

bool comms_packet_available(void){
    return packet_write_index != packet_read_index;
}

uint8_t comms_compute_crc(comms_packet_t *packet){
    return crc8((uint8_t *)packet, PACKET_SIZE - PACKET_CRC_BYTES);
}
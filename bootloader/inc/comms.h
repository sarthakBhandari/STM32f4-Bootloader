#ifndef INC_COMMS_H
#define INC_COMMS_H

#include "common-defines.h"

#define PACKET_LENGTH_BYTES (1)
#define PACKET_DATA_BYTES   (16)
#define PACKET_CRC_BYTES    (1)
#define PACKET_SIZE       (PACKET_LENGTH_BYTES + PACKET_DATA_BYTES + PACKET_CRC_BYTES)

#define PACKET_RETX_DATA0   (0x19)
#define PACKET_ACK_DATA0    (0x15)

typedef struct comms_packet_t {
    uint8_t length;
    uint8_t data[PACKET_DATA_BYTES];
    uint8_t crc;
} comms_packet_t;

void comms_setup(void);

void comms_update(void);

void comms_write(comms_packet_t* packet);

void comms_read(comms_packet_t* packet);

bool comms_packet_available(void);

uint8_t comms_compute_crc(comms_packet_t* packet);

#endif
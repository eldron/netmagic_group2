#ifndef __FFT_H
#define __FFT_H

// full forwarding table
#pragma pack(push)
#pragma pack(1)
typedef struct {
	uint8_t input_port;
	uint8_t d_mac[6];
	uint8_t s_mac[6];
	uint16_t type;
	uint8_t protocol;
	uint32_t s_ip;
	uint32_t d_ip;
	uint16_t s_port;
	uint16_t d_port;
	uint8_t input_port_mask;
	uint8_t dmac_mask[6];
	uint8_t smac_mask[6];
	uint16_t type_mask;
	uint8_t protocol_mask;
	uint32_t sip_mask;
	uint32_t dip_mask;
	uint16_t sport_mask;
	uint16_t dport_mask;
	uint8_t valid:2;
	uint8_t base_action:2;
	uint8_t postprocessor_bitmap:4;
	uint8_t output;
	uint8_t user_define[2];
} FFT;
#pragma pack(pop)
#endif
#ifndef __UTILS_H
#define __UTILS_H

#include "group2.h"

// print a byte in binary form
void binary_print_byte(uint8_t value){
	uint8_t tmp = 1;
	int i = 7;
	for(i = 7;i >= 0;i--){
		if(value & (tmp << i)){
			print("1");
		} else {
			print("0");
		}
	}
}

char dec_to_hex(uint8_t value){
	if(0 <= value && value <= 9){
		return '0' + value;
	} else {
		return 'A' + value - 10;
	}
}

uint8_t cal_byte_value(char * s){
	uint8_t value = 0;
	uint8_t one = 1;
	int i = 0;
	for(i = 0;i < 8;i++){
		if(s[i] == '1'){
			value += one << (7 - i);
		}
	}

	return value;
}

// 0 1  3 4  6 7  9 10  12 13  15 16
void cal_mac(char * s, Mac * mac){
	int i;
	for(i = 0;i < 6;i++){
		uint8_t value = 0;
		char c = s[3 * i];
		if('0' <= c && c <= '9'){
			value += 16 * (c - '0');
		} else if('a' <= c && c <= 'f'){
			value += 16 * (c - 'a' + 10);
		} else {
			value += 16 * (c - 'A' + 10);
		}
		
		c = s[3 * i + 1];
		if('0' <= c && c <= '9'){
			value += c - '0';
		} else if('a' <= c && c <= 'f'){
			value += c - 'a';
		} else {
			value += c - 'A';
		}

		mac->mac[i] = value;
	}
}

// print a decimal value in hex form
void hex_print_decimal(uint8_t value){
	printf("%c", dec_to_hex(value / 16));
	printf("%c", dec_to_hex(value % 16));
}
#endif
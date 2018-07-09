/*
  Xylolibrary.h - Library for sending commands to Xylobands
  Created by Stefan Krupop
*/
#ifndef XYLOLIBRARY_H
#define XYLOLIBRARY_H

#include "Arduino.h"
#include <SPI.h>

class XyloShield {
	public:
		XyloShield(uint8_t slaveSelectPin = 10, uint8_t interruptPin = 3, uint8_t sdnPin = 4);
		bool begin();
		bool sendRaw(const uint8_t* data, size_t length);
		bool sendRaw(const uint8_t* data, size_t length, uint8_t repeatCnt);
		bool sendPacket(uint8_t site, uint16_t zones, const uint8_t* data, size_t length);
		bool wake();
		bool sleep();
		bool keepAlive();

		// To be OR'ed together
		static const uint16_t ZONE_1 = 0x1;
		static const uint16_t ZONE_2 = 0x2;
		static const uint16_t ZONE_3 = 0x4;
		static const uint16_t ZONE_4 = 0x8;
		static const uint16_t ZONE_5 = 0x10;
		static const uint16_t ZONE_6 = 0x20;
		static const uint16_t ZONE_7 = 0x40;
		static const uint16_t ZONE_8 = 0x80;
		static const uint16_t ZONE_9 = 0x100;
		static const uint16_t ZONE_10 = 0x200;
		static const uint16_t ZONE_11 = 0x400;
		static const uint16_t ZONE_12 = 0x800;

		static const uint16_t ZONES_ALL = 0xfff;
	private:
		void setProperty(uint16_t property, uint8_t value);
		void doAPICall(const uint8_t* data, const uint8_t len, uint8_t* out, uint8_t outLen);
		uint8_t waitForResponse(uint8_t* out, uint8_t outLen, uint8_t useTimeout);
		uint8_t getResponse(uint8_t* buf, uint8_t len);
		uint8_t getFRR(uint8_t reg);
		uint8_t getState();
		void setState(uint8_t newState);
		void clearFIFO();

		uint8_t _sSelPin;
		uint8_t _intPin;
		uint8_t _sdnPin;

		uint8_t _packetCnt;

		static const uint8_t SI446X_CMD_PART_INFO = 0x01;
		static const uint8_t SI446X_CMD_SET_PROPERTY = 0x11;
		static const uint8_t SI446X_CMD_GPIO_PIN_CFG = 0x13;
		static const uint8_t SI446X_CMD_FIFO_INFO = 0x15;
		static const uint8_t SI446X_CMD_START_TX = 0x31;
		static const uint8_t SI446X_CMD_CHANGE_STATE = 0x34;
		static const uint8_t SI446X_CMD_READ_CMD_BUFF = 0x44;
		static const uint8_t SI446X_CMD_READ_FRR_B = 0x51;
		static const uint8_t SI446X_CMD_WRITE_TX_FIFO = 0x66;

		static const uint8_t SI446X_PROP_GROUP_PKT = 0x12;
		static const uint16_t SI446X_PKT_FIELD_2_LENGTH_LOW = (SI446X_PROP_GROUP_PKT << 8) | (0x12);

		static const uint8_t SI446X_FIFO_CLEAR_RX = 0x02;
		static const uint8_t SI446X_FIFO_CLEAR_TX = 0x01;

		static const uint8_t SI446X_STATE_READY = 0x03;
		static const uint8_t SI446X_STATE_READY2 = 0x04;
		static const uint8_t SI446X_STATE_TX_TUNE = 0x05;
		static const uint8_t SI446X_STATE_RX_TUNE = 0x06;
		static const uint8_t SI446X_STATE_TX = 0x07;
		static const uint8_t SI446X_STATE_RX = 0x08;

		static const uint8_t SI446X_GPIO_LOW = 0x2;
		static const uint8_t SI446X_GPIO_HIGH = 0x3;

		static const uint8_t radio_config[];

		static const uint8_t PACKET_REPEAT_CNT = 4;
		static const uint8_t PACKET_REPEAT_DELAY = 80; // ms
};

#endif
/*
  Xylolibrary.h - Library for sending commands to Xylobands
  Created by Stefan Krupop
*/
#include "XyloLibrary.h"

#include "radio_config.h"

const uint8_t XyloShield::radio_config[] = RADIO_CONFIGURATION_DATA_ARRAY;

XyloShield::XyloShield(uint8_t slaveSelectPin, uint8_t interruptPin, uint8_t sdnPin) : 
	_sSelPin(slaveSelectPin), _intPin(interruptPin), _sdnPin(sdnPin) {
}

bool XyloShield::begin() {
	digitalWrite(_sSelPin, HIGH);
	pinMode(_sSelPin, OUTPUT);
	pinMode(_sdnPin, OUTPUT);
	pinMode(_intPin, INPUT_PULLUP);

	SPI.begin();

	// Reset
	digitalWrite(_sdnPin, HIGH);
	delay(50);
	digitalWrite(_sdnPin, LOW);
	delay(50);

	// Check if RF chip is found
	uint8_t data[8] = {
		SI446X_CMD_PART_INFO, 0, 0, 0, 0, 0, 0, 0
	};
	doAPICall(data, 1, data, 8);
	uint16_t part = (data[1] << 8) | data[2];
	if (part != 0x4463) {
		return false;
	}

	// Configure transceiver
	uint8_t buf[17];
	for (uint16_t i = 0; i < sizeof(radio_config); ++i) {
		memcpy(buf, &radio_config[i], sizeof(buf));
		doAPICall(&buf[1], buf[0], NULL, 0);
		i += buf[0];
	}

	return true;
}

bool XyloShield::sendRaw(const uint8_t* data, size_t length) {
	if (getState() == SI446X_STATE_TX) {
		// Already transmitting
		return false;
	}

	setState(SI446X_STATE_READY);
	clearFIFO();

	// Load data to FIFO
	digitalWrite(_sSelPin, LOW);
	SPI.transfer(SI446X_CMD_WRITE_TX_FIFO);
	SPI.transfer(length);
	for (uint8_t i = 0; i < length; ++i) {
		SPI.transfer(data[i]);
	}
	digitalWrite(_sSelPin, HIGH);

	// Set packet length
	setProperty(SI446X_PKT_FIELD_2_LENGTH_LOW, length);

	// Set the antenna switch pins using the GPIO, assuming we have an RFM module with antenna switch
	uint8_t gpioConfig[] = {
		SI446X_CMD_GPIO_PIN_CFG,
		SI446X_GPIO_LOW,
		SI446X_GPIO_HIGH
	};
	doAPICall(gpioConfig, sizeof(gpioConfig), NULL, 0);

	// Begin transmit
	uint8_t startTx[] = {
		SI446X_CMD_START_TX,
		0x00,
		(uint8_t)(SI446X_STATE_READY << 4)
	};
	doAPICall(startTx, sizeof(startTx), NULL, 0);

	return true;
}

void XyloShield::setProperty(uint16_t property, uint8_t value) {
	uint8_t data[5] = {
		SI446X_CMD_SET_PROPERTY,
		(uint8_t)(property >> 8),
		1,
		(uint8_t)property,
		value
	};

	doAPICall(data, sizeof(data), NULL, 0);
}

void XyloShield::doAPICall(const uint8_t* data, const uint8_t len, uint8_t* out, uint8_t outLen) {
	if(waitForResponse(NULL, 0, 1)) {
		digitalWrite(_sSelPin, LOW);
		for (uint8_t i = 0; i < len; i++) {
			SPI.transfer(data[i]);
		}
		digitalWrite(_sSelPin, HIGH);

		if (out != NULL) {
			waitForResponse(out, outLen, 1);
		}
	}
}

uint8_t XyloShield::waitForResponse(uint8_t* out, uint8_t outLen, uint8_t useTimeout) {
	uint16_t timeout = 50000;
	while (!getResponse(out, outLen)) {
		delayMicroseconds(10);
		if (useTimeout && !--timeout) {
			return 0;
		}
	}
	return 1;
}

uint8_t XyloShield::getResponse(uint8_t* buf, uint8_t len) {
	uint8_t cts = 0;

	digitalWrite(_sSelPin, LOW);
	// Send command
	SPI.transfer(SI446X_CMD_READ_CMD_BUFF);

	// Get CTS value
	cts = (SPI.transfer(0xFF) == 0xFF);

	if (cts) {
		// Get response data
		for (uint8_t i = 0; i < len ; ++i) {
			buf[i] = SPI.transfer(0xFF);
		}
	}
	digitalWrite(_sSelPin, HIGH);

	return cts;
}

uint8_t XyloShield::getFRR(uint8_t reg) {
	uint8_t frr = 0;
	digitalWrite(_sSelPin, LOW);
	SPI.transfer(reg);
	frr = SPI.transfer(0xFF);
	digitalWrite(_sSelPin, HIGH);
	return frr;
}

uint8_t XyloShield::getState() {
	uint8_t state = getFRR(SI446X_CMD_READ_FRR_B);
	if (state == SI446X_STATE_TX_TUNE) {
		state = SI446X_STATE_TX;
	} else if (state == SI446X_STATE_RX_TUNE) {
		state = SI446X_STATE_RX;
	} else if (state == SI446X_STATE_READY2) {
		state = SI446X_STATE_READY;
	}
	return state;
}

// Set new state
void XyloShield::setState(uint8_t newState) {
	uint8_t data[] = {
		SI446X_CMD_CHANGE_STATE,
		newState
	};
	doAPICall(data, sizeof(data), NULL, 0);
}

void XyloShield::clearFIFO() {
	static const uint8_t clearFifo[] = {
		SI446X_CMD_FIFO_INFO,
		SI446X_FIFO_CLEAR_RX | SI446X_FIFO_CLEAR_TX
	};
	doAPICall(clearFifo, sizeof(clearFifo), NULL, 0);
}

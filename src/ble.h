#pragma once

#include <Arduino.h>
#include <M5StickC.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class BLESerial {
	public:
		BLESerial();
		void setup();
		void update();

		inline void connected() { _connected = true; }
		inline void disconnected() { _connected = false; }

	private:
		BLEServer* server;
		BLECharacteristic *txChar, *rxChar;
		BLEService *service;

		bool _connected = false;
		unsigned long last_send;
};

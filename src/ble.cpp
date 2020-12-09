#include "ble.h"

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // 0x2A3D

class SerialCallbacks : public BLEServerCallbacks {

	public:
		SerialCallbacks(BLESerial& s) : serial(&s) { }

	void onConnect(BLEServer *server) {
		M5.Lcd.setCursor(0, 25);
		M5.Lcd.println("BLE Online");
		serial->connected();
	}

	void onDisconnect(BLEServer *server) {
		M5.Lcd.setCursor(0, 25);
		M5.Lcd.println("BLE Offline");
		serial->disconnected();
	}

	private:
		BLESerial *serial;
};

class ReceiveCallbacks : public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic *rx) {
		// Process Command
	}
};

BLESerial::BLESerial() {

}

void BLESerial::setup() {
	BLEDevice::init("pixywand");

	server = BLEDevice::createServer();
	server->setCallbacks(new SerialCallbacks(*this));

	service = server->createService(SERVICE_UUID);

	txChar = service->createCharacteristic(
		CHARACTERISTIC_UUID_TX,
		BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
	);

	txChar->addDescriptor(new BLE2902());

	rxChar = service->createCharacteristic(CHARACTERISTIC_UUID_RX,
			BLECharacteristic::PROPERTY_WRITE);
	rxChar->setCallbacks(new ReceiveCallbacks());
	service->start();
	BLEAdvertising* ad = server->getAdvertising();
	ad->setAppearance(193); // GENERIC_SPORTS_WATCH
	ad->start();
}

void BLESerial::update() {
	if (!_connected /* || !hasData */ ) return;
	if (millis() - last_send < 50) return;

	txChar->setValue("128,54,-90\n");
	txChar->notify();
	last_send = millis();
}

#include <SoftwareSerial.h>

const int PIN_RX = 3;
const int PIN_TX = 2;
const int PIN_RELAY = 4;

const long INTERVAL_CHECK = 5000;
const long DURATION_5_MIN = 300000;
const long DURATION_30_MIN = 1800000;
const long DURATION_60_MIN = 3600000;

SoftwareSerial validator(PIN_RX, PIN_TX);

unsigned long lastByteTime = 0;
unsigned long relayStartTime = 0;
long relayActiveDuration = 0;
bool isRelayActive = false;
unsigned int byteCount = 0;

const int MAX_BYTES_VALUE = 256;
int byteFrequency[MAX_BYTES_VALUE] = {0};

void setup() {
	Serial.begin(9600);
	validator.begin(9600);

	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(PIN_RELAY, OUTPUT);
	digitalWrite(PIN_RELAY, HIGH);

	Serial.println("NV9 System Ready");
}

void loop() {
	if (validator.available()) {
		int receivedByte = validator.read();

		if (receivedByte >= 0 && receivedByte < MAX_BYTES_VALUE) {
			byteFrequency[receivedByte]++;
		}

		lastByteTime = millis();
		byteCount++;

		if (receivedByte == 0xFF) {
			return;
		}
	}

	unsigned long currentTime = millis();

	if (currentTime - lastByteTime >= INTERVAL_CHECK && byteCount > 0) {
		Serial.print("Bytes received: ");
		Serial.println(byteCount);

		if (byteCount > 4 && byteCount <= 20) {
			activateRelay(DURATION_5_MIN);
		} else if (byteCount >= 30 && byteCount <= 75) {
			activateRelay(DURATION_30_MIN);
		} else if (byteCount >= 90 && byteCount <= 200) {
			activateRelay(DURATION_60_MIN);
		}

		resetByteFrequency();
		byteCount = 0;
		lastByteTime = currentTime;
	}

	// Conditie sparta pe doua linii pentru a respecta limita de 80 chars
	if (isRelayActive && 
		(currentTime - relayStartTime >= relayActiveDuration)) {
		
		digitalWrite(PIN_RELAY, HIGH);
		isRelayActive = false;
		Serial.println("Relay Deactivated");
	}
}

void activateRelay(long duration) {
	relayActiveDuration = duration;
	relayStartTime = millis();
	digitalWrite(PIN_RELAY, LOW);
	isRelayActive = true;
	Serial.println("Relay Activated");
}

void resetByteFrequency() {
	for (int i = 0; i < MAX_BYTES_VALUE; i++) {
		byteFrequency[i] = 0;
	}
}
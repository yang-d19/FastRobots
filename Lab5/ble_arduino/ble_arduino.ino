
#include "BLECStringCharacteristic.h"
#include "EString.h"
#include "RobotCommand.h"
#include <ArduinoBLE.h>

//////////// BLE UUIDs ////////////
#define BLE_UUID_TEST_SERVICE "e1e57366-5298-47b4-9844-5af935d8a194"

#define BLE_UUID_RX_STRING "9750f60b-9c9c-4158-b620-02ec9521cd99"

#define BLE_UUID_TX_FLOAT "27616294-3063-4ecc-b60b-3470ddef2938"
#define BLE_UUID_TX_STRING "f235a225-6735-4d73-94cb-ee5dfce9ba83"
//////////// BLE UUIDs ////////////

//////////// Global Variables ////////////
BLEService testService(BLE_UUID_TEST_SERVICE);

BLECStringCharacteristic rx_characteristic_string(BLE_UUID_RX_STRING, BLEWrite, MAX_MSG_SIZE);

BLEFloatCharacteristic tx_characteristic_float(BLE_UUID_TX_FLOAT, BLERead | BLENotify);
BLECStringCharacteristic tx_characteristic_string(BLE_UUID_TX_STRING, BLERead | BLENotify, MAX_MSG_SIZE);

// RX
RobotCommand robot_cmd(":|");

// TX
EString tx_estring_value;
float tx_float_value = 0.0;

long interval = 500;
static long previousMillis = 0;
unsigned long currentMillis = 0;

const int buffer_size = 100;

int end_pt = 0;
int time_stamps[buffer_size];
float temperatures[buffer_size];

const char* arr_5B = "iiiii";
const char* arr_10B = "iiiiiiiiii";
const char* arr_20B = "iiiiiiiiiiiiiiiiiiii";
const char* arr_40B = "iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii";
const char* arr_80B = "iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii";
const char* arr_120B = "iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii";
const char* arr_240B = "iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii";
//////////// Global Variables ////////////



//////////// Temperature Sensor Related ///////////

#define RESOLUTION_BITS (16)      // choose resolution (explained in depth below)

#ifdef ADCPIN
#define EXTERNAL_ADC_PIN ADCPIN   // ADCPIN is the lowest analog capable pin exposed on the variant
#endif                            // - if no exposed pins are analog capable this will be undefined
                                  // - to use another pin provide an analog capable pin number such as:
                                  //   - A0 -> A9 (when analog pins are named sequentially from 0)
                                  //   - A11 -> A13, A16, A29, A31 -> A35 (when pins are named after Apollo3 pads)
                                  //   - A variant-specific pin number (when none of the above apply)

void sensor_setup() {
  // Serial.begin(115200);
  // Serial.println("Apollo3 - analogRead");

  analogReadResolution(RESOLUTION_BITS);    // set the resolution of analogRead results
                                            //  - maximum: 16 bits (padded with trailing zeroes)
                                            //  - ADC:     14 bits (maximum ADC resolution)
                                            //  - default: 10 bits (standard Arduino setting)
                                            //  - minimum:  1 bit

  analogWriteResolution(RESOLUTION_BITS);   // match resolution for analogWrite
}

float read_temp_f() {

  int vcc_3 = analogReadVCCDiv3();    // reads VCC across a 1/3 voltage divider
  int vss = analogReadVSS();          // ideally 0
  int temp_raw = analogReadTemp();    // raw ADC counts from die temperature sensor
  
  float temp_f = getTempDegF();       // computed die temperature in deg F
  float vcc_v = getVCCV();            // computed supply voltage in V

  return temp_f;

  // Serial.printf("temp (counts): %d, vcc/3 (counts): %d, vss (counts): %d, time (ms) %d\n", temp_raw, vcc_3, vss, millis());
}

enum CommandTypes {
    PING,
    SEND_TWO_INTS,
    SEND_THREE_FLOATS,
    ECHO,
    DANCE,
    SET_VEL,
    GET_TIME_MILLIS,
    SEND_TIME_DATA,
    GET_TEMP_READINGS,
    GET_REPLY_LEN
};

void handle_command() {   
    // Set the command string from the characteristic value
    robot_cmd.set_cmd_string(rx_characteristic_string.value(),
                             rx_characteristic_string.valueLength());

    bool success;
    int cmd_type = -1;

    // Get robot command type (an integer)
    /* NOTE: THIS SHOULD ALWAYS BE CALLED BEFORE get_next_value()
     * since it uses strtok internally (refer RobotCommand.h and 
     * https://www.cplusplus.com/reference/cstring/strtok/)
     */
    success = robot_cmd.get_command_type(cmd_type);

    // Check if the last tokenization was successful and return if failed
    if (!success) {
        return;
    }

    // Handle the command type accordingly
    switch (cmd_type) {
        /*
         * Write "PONG" on the GATT characteristic BLE_UUID_TX_STRING
         */
        case PING:
            tx_estring_value.clear();
            tx_estring_value.append("PONG");
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            Serial.print("Sent back: ");
            Serial.println(tx_estring_value.c_str());

            break;
        
        case GET_REPLY_LEN:
            int len;

            success = robot_cmd.get_next_value(len);
            if (!success)
                return;
            
            Serial.print("Request reply of length: ");
            Serial.println(len);

            tx_estring_value.clear();
            switch (len) {
              case 5:
                tx_estring_value.set(arr_5B);
                break;
              case 10:
                tx_estring_value.set(arr_10B);
                break;
              case 20:
                tx_estring_value.set(arr_20B);
                break;
              case 40:
                tx_estring_value.set(arr_40B);
                break;
              case 80:
                tx_estring_value.set(arr_80B);
                break;
              case 120:
                tx_estring_value.set(arr_120B);
                break;
              case 240:
                tx_estring_value.set(arr_240B);
                break;
              default:
                Serial.println("No correspond length");
                break;
            }
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

        /*
         * Extract two integers from the command string
         */
        case SEND_TWO_INTS:
            int int_a, int_b;

            // Extract the next value from the command string as an integer
            success = robot_cmd.get_next_value(int_a);
            if (!success)
                return;

            // Extract the next value from the command string as an integer
            success = robot_cmd.get_next_value(int_b);
            if (!success)
                return;

            Serial.print("Two Integers: ");
            Serial.print(int_a);
            Serial.print(", ");
            Serial.println(int_b);
            
            break;
        /*
         * Extract three floats from the command string
         */
        case SEND_THREE_FLOATS:
            /*
             * Your code goes here.
             */

            break;
        /*
         * Add a prefix and postfix to the string value extracted from the command string
         */
        case ECHO:

            char char_arr[MAX_MSG_SIZE];

            // Extract the next value from the command string as a character array
            success = robot_cmd.get_next_value(char_arr);
            if (!success)
                return;

            /*
             * Your code goes here.
             */
            tx_estring_value.clear();
            tx_estring_value.append("Robot says: ");
            tx_estring_value.append(char_arr);
            tx_estring_value.append(" ^_^");
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            Serial.print("Sent back after ECHO command: ");
            Serial.println(tx_estring_value.c_str());
            
            break;
        /*
         * DANCE
         */
        case DANCE:
            Serial.println("Look Ma, I'm Dancin'!");

            break;
        
        /*
         * SET_VEL
         */
        case SET_VEL:

            break;
        
        /* 
         * Return current time in milliseconds
         */
        case GET_TIME_MILLIS: {

            const int t = millis();
            tx_estring_value.clear();
            tx_estring_value.append("T: ");
            tx_estring_value.append(t);
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            break;
        }

        case SEND_TIME_DATA: {
            for (int i = 0; i < end_pt; i++) {
              tx_estring_value.clear();
              tx_estring_value.append("T: ");
              tx_estring_value.append(time_stamps[i]);
              tx_characteristic_string.writeValue(tx_estring_value.c_str());
            }
            // reset the buffer
            end_pt = 0;

            break;
        }

        case GET_TEMP_READINGS: {
            for (int i = 0; i < end_pt; i++) {
              tx_estring_value.clear();

              tx_estring_value.append(time_stamps[i]);
              tx_estring_value.append("|");
              tx_estring_value.append(temperatures[i]);

              tx_characteristic_string.writeValue(tx_estring_value.c_str());
            }
            // reset the buffer
            end_pt = 0;

            break;
        }
        
        /* 
         * The default case may not capture all types of invalid commands.
         * It is safer to validate the command string on the central device (in python)
         * before writing to the characteristic.
         */
        default:
            Serial.print("Invalid Command Type: ");
            Serial.println(cmd_type);
            break;
    }
}

void setup() {
    Serial.begin(115200);

    sensor_setup();

    BLE.begin();

    // Set advertised local name and service
    BLE.setDeviceName("Artemis BLE");
    BLE.setLocalName("Artemis BLE");
    BLE.setAdvertisedService(testService);

    // Add BLE characteristics
    testService.addCharacteristic(tx_characteristic_float);
    testService.addCharacteristic(tx_characteristic_string);
    testService.addCharacteristic(rx_characteristic_string);

    // Add BLE service
    BLE.addService(testService);

    // Initial values for characteristics
    // Set initial values to prevent errors when reading for the first time on central devices
    tx_characteristic_float.writeValue(0.0);

    /*
     * An example using the EString
     */

    // Clear the contents of the EString before using it
    // tx_estring_value.clear();

    // // Append the string literal "[->"
    // tx_estring_value.append("[->");

    // // Append the float value
    // tx_estring_value.append(9.0);

    // // Append the string literal "<-]"
    // tx_estring_value.append("<-]");

    // // Write the value to the characteristic
    // tx_characteristic_string.writeValue(tx_estring_value.c_str());

    // Output MAC Address
    Serial.print("Advertising BLE with MAC: ");
    Serial.println(BLE.address());

    BLE.advertise();
}

void write_data() {
    currentMillis = millis();
    if (currentMillis - previousMillis > 0) {

        // tx_float_value = tx_float_value + 0.5;
        // tx_characteristic_float.writeValue(tx_float_value);

        // if (tx_float_value > 10000) {
        //     tx_float_value = 0;
        // }

        // 1. Send time millisec as fast as possible
        tx_estring_value.clear();
        tx_estring_value.append("T: ");
        tx_estring_value.append((const int)currentMillis);
        tx_characteristic_string.writeValue(tx_estring_value.c_str());

        // 2. Don't directly send, save to array instead
        // if (end_pt < buffer_size) {
        //   time_stamps[end_pt] = currentMillis;
        //   temperatures[end_pt] = getTempDegF();
        //   end_pt++;
        // }

        previousMillis = currentMillis;
    }
}

void read_data() {
    // Query if the characteristic value has been written by another BLE device
    if (rx_characteristic_string.written()) {
        handle_command();
    }
}

void send_time_stamp() {
  const int t = millis();
  // Send time stamp directly
  tx_estring_value.clear();
  tx_estring_value.append("T: ");
  tx_estring_value.append(t);
  tx_characteristic_string.writeValue(tx_estring_value.c_str());
}

void store_time_stamp() {
  const int t = millis();
  // Store time stamp into the array
  if (end_pt < buffer_size) {
    time_stamps[end_pt] = t;
    end_pt++;
  }
}

void store_time_temperature() {
  const int t = millis();
  // Store time stamp into the array
  if (end_pt < buffer_size) {
    time_stamps[end_pt] = t;
    // to be fix, read from temp sensor
    temperatures[end_pt] = 0;
    end_pt++;
  }
}

void loop() {
    // Listen for connections
    BLEDevice central = BLE.central();

    // If a central is connected to the peripheral
    if (central) {
        Serial.print("Connected to: ");
        Serial.println(central.address());

        // While central is connected
        while (central.connected()) {
            // Send data
            write_data();

            // Read data
            read_data();

            // Send time stamp as fast as possible
            // send_time_stamp();
        }

        Serial.println("Disconnected");
    }
}

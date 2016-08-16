#include <VirtualWire.h> // inclusion de la librairie VirtualWire

#define STX       0x02
#define ETX       0x03
#define PRECISION 1 

byte message[10];
 
void setup()
{
    Serial.begin(9600);
    Serial.println("Tuto VirtualWire");
 
    vw_setup(2000);

    prepareMessage();
}
 
void loop()
{
    get_instructions_from_serial_port();
    
    vw_send(message, 10);
    vw_wait_tx();
//    delay(10);

    dumpMessage(false);
}

/**
 * Read instructions from serial port and calculate cheksum.
 * Received data must be composed as following :
 * << STX (1B) | YAW (2B) | PITCH (2B) | ROLL (2B) | THROTTLE (1B) | CHECKSUM (1B) | ETX (1B) >>
 *
 * The received CHECKSUM byte is dummy as it's calculated here.
 */
byte* get_instructions_from_serial_port()
{
    int i;

    if (Serial.available()) {
        delay(2);
        message[0] = Serial.read();

        if (message[0] == STX) {
            i = 1;
          
            while(Serial.available()) {
                delay(1);
                message[i] = Serial.read();

                if (message[i] == ETX && i == 9) {
                    break;
                }

                i++;
            }

            calculateChecksum();
        }
    }
}

/**
 * Dump received message on serial port : send it back to smartphone.
 *
 * @param boolean html : display message as HTML if true, as CSV otherwise.
 */
void dumpMessage(boolean html) {
    float yaw      = getYaw();
    float pitch    = getPitch();
    float roll     = getRoll();
    float throttle = getThrottle();
  
    if (html) {
       Serial.print("Yaw : ");
       Serial.print(yaw);
       Serial.print(" Throttle : ");
       Serial.print(throttle);
       Serial.print("<br>Pitch : ");
       Serial.print(pitch);
       Serial.print(" Roll : ");
       Serial.print(roll);
       Serial.println("<br/>");
    } else {
       Serial.print(yaw);
       Serial.print(";");
       Serial.print(pitch);
       Serial.print(";");
       Serial.print(roll);
       Serial.print(";");        
       Serial.println(throttle);
    }
}

/**
 * Initialize message to send with default values :
 *     - yaw        = 180°
 *     - pitch      = 0°
 *     - roll       = 0°
 *     - throttle   = 0°
 */
void prepareMessage()
{
    uint16_t yaw_i;
    uint16_t pitch_i;
    uint16_t roll_i;
    byte     yaw_b;
    byte     pitch_b; 
    byte     roll_b;
    byte     throttle_b;
    byte     checksum;
    float yaw      = 180;
    float pitch    = 0;
    float roll     = 0;
    float throttle = 0;

    // Normalized values (2 bytes)
    yaw_i   = yaw;
    pitch_i = (pitch + 45) * 10;
    roll_i  = (roll + 45) * 10;
    
    // Byte values  
    message[0] = STX;  
    message[1] = yaw_i >> 8;
    message[2] = yaw_i & 0xFF;
    message[3] = pitch_i >> 8;
    message[4] = pitch_i & 0xFF;
    message[5] = roll_i >> 8;
    message[6] = roll_i & 0xFF;
    message[7] = byte(throttle);
    message[9] = ETX;

    calculateChecksum();
}

/**
 * Calculate checksum and store it in "message" array.
 */
void calculateChecksum()
{
    byte checksum = (message[0] ^ message[1] ^ message[2] ^ message[3] ^ message[4] ^ message[5] ^ message[6] ^ message[7]);

    message[8] = checksum;    
}

/**
 * Extract yaw value from received datas
 * 
 * @return float : yaw value in [0, 360]°
 */
float getYaw()
{
    //                 MSB            LSB
    uint16_t yaw = (message[1] << 8) | message[2];

    // Limit cases
    if (yaw > 360) {
        yaw = 360;
    } else if (yaw < 0) {
        yaw = 0;
    }

    return float(yaw);
}

/**
 * Extract pitch value from received datas.
 *
 * @return float : pitch value in [-45.0, +45.0]°
 */
float getPitch()
{
    //                 MSB            LSB
    uint16_t pitch = (message[3] << 8) | message[4];

    // Limit cases
    if (pitch > 900) {
        pitch = 900;
    } else if (pitch < 0) {
        pitch = 0;
    }

    float pitch_float = pitch / pow(10, PRECISION);
    pitch_float -= 45;

    return pitch_float;
}

/**
 * Extract roll value from received datas.
 *
 * @return float : pitch value in [-45.0, +45.0]°
 */
float getRoll()
{
    //                MSB            LSB
    uint16_t roll = (message[5] << 8) | message[6];

    // Limit cases
    if (roll > 900) {
        roll = 900;
    } else if (roll < 0) {
        roll = 0;
    }

    float roll_float = roll / pow(10, PRECISION);
    roll_float -= 45;

    return roll_float;
}

/**
 * Extract throttle value from received datas.
 *
 * @return float
 */
float getThrottle()
{
    float throttle = float(message[7]);

    if (throttle < 0) {
        throttle = 0;
    } else if (throttle > 100) {
        throttle = 100;
    }

    return throttle;
} 



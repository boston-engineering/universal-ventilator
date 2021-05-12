//
// Created by Andrew Hoener on 5/10/21.
//

#include <functional>
#include "TftTouch.h"

volatile bool has_new_touch = false;

void handle_interrupt()
{
    has_new_touch = true;
}

TftTouch::TftTouch(uint8_t int_pin, uint8_t rst_pin)
{
    interrupt_pin = int_pin;
    reset_pin = rst_pin;
}

void TftTouch::init()
{

    Serial.println("Setting pin modes & interrupts");

    pinMode(interrupt_pin, INPUT);
    pinMode(reset_pin, OUTPUT);
    reset();
    delay(100);

#ifdef digitalPinToInterrupt
    attachInterrupt(digitalPinToInterrupt(interrupt_pin), handle_interrupt, FALLING);
#else
    attachInterrupt(0, handleInterrupt, FALLING);
#endif

    Serial.println("Beginning wire");
    Wire.begin();
    Wire.beginTransmission(FT_I2C_ADDRESS);
    Wire.write(FT_DEVICE_MODE);
    Wire.write(0);
    Wire.endTransmission(FT_I2C_ADDRESS);

    Serial.println("Setup done.");
    // find_address();
    // print_info();
}

bool TftTouch::touched()
{
    if (has_new_touch) {
        has_new_touch = false;
        return true;
    }
    return false;
}

void TftTouch::print_info()
{
    byte registers[0xfe];
    memset(registers, 0, 0xfe);
    Wire.beginTransmission(FT_I2C_ADDRESS);
    Wire.write(0x00);
    Wire.endTransmission(FT_I2C_ADDRESS);
    Wire.requestFrom(FT_I2C_ADDRESS, 0xfe);
    int register_number = 0;
    // get all register bytes when available
    Serial.println("[");
    while (Wire.available()) {
        registers[register_number] = Wire.read();
        Serial.print("\t");
        Serial.println(registers[register_number]);
        delay(5);
        register_number++;
    }
    Serial.println("]");
    delay(10);
    // Might be that the interpretation of high/low bit is not same as major/minor version...
    Serial.print("Library version: ");
    Serial.print(registers[FT_TOUCH_LIB_VERSION_H]);
    Serial.print(".");
    Serial.print(registers[FT_TOUCH_LIB_VERSION_L]);
    Serial.println(".");

}

void TftTouch::reset() const
{
    digitalWrite(reset_pin, LOW);
    delay(100);
    digitalWrite(reset_pin, HIGH);
}

void TftTouch::read_num_touch_points(uint8_t& points)
{
    Wire.beginTransmission(FT_I2C_ADDRESS);
    Wire.write(FT_TOUCH_POINTS);
    Wire.endTransmission(FT_I2C_ADDRESS);
    Wire.requestFrom(FT_I2C_ADDRESS, 1);
    if (Wire.available()) {
        points = Wire.read();
    }
}

void TftTouch::read_touch_registers(uint8_t len)
{
    int array_length = len * NUM_BYTES_PER_INPUT + 1;
    uint8_t raw_data[array_length];
    memset(raw_data, 0, array_length);

    Wire.beginTransmission(FT_I2C_ADDRESS);
    Wire.write(FT_TOUCH1_BEGIN);
    Wire.endTransmission(FT_I2C_ADDRESS);
    Wire.requestFrom(FT_I2C_ADDRESS, array_length);
    int idx = 0;
    //Serial.println("[");
    while (Wire.available()) {
        raw_data[idx] = Wire.read();
        //Serial.print("\t"); Serial.println(raw_data[idx]);
        idx++;
    }
    //Serial.println("]");

    for (idx = 0; idx < len; idx++) {
        read_touch(new_touch_data + idx, raw_data, idx);
    }

    print_touch_data(new_touch_data[0]);
}

void TftTouch::read_touch(TsData* data, const uint8_t* raw_data, uint8_t reg)
{
    uint8_t values[USED_BYTES_PER_INPUT];
    uint8_t event;

    for (int i = 0; i < USED_BYTES_PER_INPUT; i++) {
        values[i] = raw_data[(reg * NUM_BYTES_PER_INPUT) + i];
    }

    memcpy(data->raw, values, USED_BYTES_PER_INPUT);
    data->event_flag = ((data->raw[0] & FT_STATUS_MASK) >> 6);
    data->x = word(data->raw[0] & FT_XH_MASK, data->raw[1]);
    data->y = word(data->raw[2] & FT_XH_MASK, data->raw[3]);
    data->weight = data->raw[4];
}

void TftTouch::print_touch_data(TsData data)
{
    char buffer[128];
    Serial.println("[");
#if USE_DEBUG_TS_DATA
    Serial.print("\t0b");
    Serial.print(data.raw[0], BIN);
    Serial.print("\t0b");
    Serial.println(data.raw[1], BIN);
    Serial.print("\t0b");
    Serial.print(data.raw[2], BIN);
    Serial.print("\t\t0b");
    Serial.println(data.raw[3], BIN);
#endif
    sprintf(buffer, "\tX: %d\t\tY: %d", data.x, data.y);
    Serial.println(buffer);
    sprintf(buffer, "\tWeight: %d\tEvent: %x\tPoints: %d", data.weight, data.event_flag, data.num_points);
    Serial.println(buffer);
    Serial.println("]\n");
}
#pragma once

#include <RadioLib.h>
#include "LM_Module.h"
#include "driver/spi_master.h"
#include "EspHal.h"

#ifndef LORA_CONFIG_H // Prevent multiple definitions of the struct
#define LORA_CONFIG_H
struct LoraConfig {
    uint8_t spiMosi; // Pin MOSI
    uint8_t spiMiso; // Pin MISO
    uint8_t spiSclk; // Pin SCLK
    uint8_t loraCs;  // Pin CS
    uint8_t loraIrq; // Pin IRQ
    uint8_t loraRst; // Pin Reset
};
#endif
class LM_SX1276 : public LM_Module {
public:
    LM_SX1276(spi_host_device_t spi_host, const LoraConfig& config);
    LM_SX1276(EspHal* hal, uint8_t loraCs, uint8_t loraIrq, uint8_t loraRst);

    int16_t begin(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord,
    int8_t power, int16_t preambleLength) override;

    int16_t receive(uint8_t* data, size_t len) override;
    int16_t startReceive() override;
    int16_t scanChannel() override;
    int16_t startChannelScan() override;
    int16_t standby() override;
    void reset() override;
    int16_t setCRC(bool crc) override;
    size_t getPacketLength() override;
    float getRSSI() override;
    float getSNR() override;
    int16_t readData(uint8_t* buffer, size_t numBytes) override;
    int16_t transmit(uint8_t* buffer, size_t length) override;
    uint32_t getTimeOnAir(size_t length) override;

    void setDioActionForReceiving(void (*action)()) override;
    void setDioActionForReceivingTimeout(void (*action)()) override;
    void setDioActionForScanning(void (*action)()) override;
    void setDioActionForScanningTimeout(void (*action)()) override;
    void clearDioActions() override;

    int16_t setFrequency(float freq) override;
    int16_t setBandwidth(float bw) override;
    int16_t setSpreadingFactor(uint8_t sf) override;
    int16_t setCodingRate(uint8_t cr) override;
    int16_t setSyncWord(uint8_t syncWord) override;
    int16_t setOutputPower(int8_t power) override;
    int16_t setPreambleLength(int16_t preambleLength) override;
    int16_t setGain(uint8_t gain) override;
    int16_t setOutputPower(int8_t power, int8_t useRfo) override;
   SX1276* module;
private:
 
    spi_device_handle_t spi_handle;
    uint8_t loraCs;
    uint8_t loraIrq;
    uint8_t loraRst;
    uint8_t loraIo1;
};
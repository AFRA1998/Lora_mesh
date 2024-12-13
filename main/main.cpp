#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "LoraMesher.h"

// Define LED pin and states
#define BOARD_LED   GPIO_NUM_4
#define LED_ON      0
#define LED_OFF     1

static const char *TAG = "LoRaMesher_APP";

// LoRaMesher instance
LoraMesher& radio = LoraMesher::getInstance();

uint32_t dataCounter = 0;
struct dataPacket {
    uint32_t counter = 0;
};

dataPacket* helloPacket = new dataPacket;

// Function to flash the LED
void led_Flash(uint16_t flashes, uint16_t delaymS) {
    for (uint16_t i = 0; i < flashes; i++) {
        gpio_set_level(BOARD_LED, LED_ON);
        vTaskDelay(pdMS_TO_TICKS(delaymS));
        gpio_set_level(BOARD_LED, LED_OFF);
        vTaskDelay(pdMS_TO_TICKS(delaymS));
    }
}

// Print packet data
void printPacket(dataPacket data) {
    ESP_LOGI(TAG, "Hello Counter received nº %lu", (unsigned long)data.counter);
}

// Process received packets
void processReceivedPackets(void *param) {
    for (;;) {
        ulTaskNotifyTake(pdPASS, portMAX_DELAY); // Wait for notification from LoRaMesher
        led_Flash(1, 100);

        while (radio.getReceivedQueueSize() > 0) {
    ESP_LOGI(TAG, "Queue receiveUserData size: %d", radio.getReceivedQueueSize());
    AppPacket<dataPacket>* packet = radio.getNextAppPacket<dataPacket>();
    if (packet) { // Vérifie seulement si le paquet est valide
        dataPacket* dPacket = packet->payload;
        size_t payloadLength = packet->getPayloadLength();

        for (size_t i = 0; i < payloadLength; i++) {
            printPacket(dPacket[i]);
        }

        radio.deletePacket(packet);
    } else {
        ESP_LOGW(TAG, "Failed to retrieve or process packet");
    }
}

    }
}

// Send Hello Packets Task
void sendHelloPackets(void *param) {
    for (;;) {
        ESP_LOGI(TAG, "Sending Hello packet nº %lu", (unsigned long)dataCounter);
        helloPacket->counter = dataCounter++;
        radio.createPacketAndSend(BROADCAST_ADDR, helloPacket, 1);

        // Send packets every 20 seconds
        vTaskDelay(pdMS_TO_TICKS(20000));
    }
}

// Task handles
TaskHandle_t receiveLoRaMessage_Handle = NULL;
TaskHandle_t sendHelloPackets_Handle = NULL;

// Create receive task
void createReceiveMessages() {
    int res = xTaskCreate(
        processReceivedPackets,
        "Receive App Task",
        4096,
        NULL,
        2, // Priority 2
        &receiveLoRaMessage_Handle);
    if (res != pdPASS) {
        ESP_LOGE(TAG, "Error creating Receive App Task: %d", res);
    }
}

// Create send task
void createSendMessages() {
    int res = xTaskCreate(
        sendHelloPackets,
        "Send Hello Packets",
        4096,
        NULL,
        1, // Priority 1
        &sendHelloPackets_Handle);
    if (res != pdPASS) {
        ESP_LOGE(TAG, "Error creating Send Hello Packets Task: %d", res);
    }
}

// Initialize LoRaMesher
void setupLoraMesher() {
    LoraMesher::LoraMesherConfig config;
    config.loraCs = GPIO_NUM_18;
    config.loraRst = GPIO_NUM_14;
    config.loraIrq = GPIO_NUM_26;
    config.loraIo1 = GPIO_NUM_33;
    config.module = LoraMesher::LoraModules::SX1276_MOD;

    radio.begin(config);
    createReceiveMessages();
    createSendMessages();
    radio.setReceiveAppDataTaskHandle(receiveLoRaMessage_Handle);
    radio.start();
    ESP_LOGI(TAG, "LoRa initialized");
}

// Main application entry
extern "C" void app_main() {
    ESP_LOGI(TAG, "Initializing Board");

    // Initialize LED pin
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << BOARD_LED);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    led_Flash(2, 125); // Flash LED to indicate initialization
    setupLoraMesher(); // Initialize LoRaMesher

    while (1) {
        // Main task loop can handle other functionalities if needed
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

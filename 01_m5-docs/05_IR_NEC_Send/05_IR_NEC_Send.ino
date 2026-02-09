#include "M5Unified.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_encoder.h"

// https://docs.m5stack.com/en/arduino/m5sticks3/ir_nec

#define IR_SEND_PIN 46 // GPIO pin connected to IR LED transmitter

// NEC protocol parameters
uint16_t address = 0x0000; // NEC address (8-bit or 16-bit)
uint8_t command = 0x55;    // NEC command byte
uint8_t repeats = 0;       // Number of repeat frames (0 = no repeat)

rmt_channel_handle_t tx_chan = NULL;
rmt_encoder_handle_t copy_encoder = NULL;

// NEC protocol timing constants (microseconds)
#define NEC_HEADER_MARK 9000
#define NEC_HEADER_SPACE 4500
#define NEC_BIT_MARK 560
#define NEC_BIT_0_SPACE 560
#define NEC_BIT_1_SPACE 1690
#define NEC_REPEAT_MARK 9000
#define NEC_REPEAT_SPACE 2250

// IR carrier configuration
#define IR_CARRIER_FREQ_HZ 38000
#define IR_DUTY_CYCLE 0.33

// Function prototypes
void setup_rmt_tx();
bool sendNEC(uint16_t address, uint8_t command, uint8_t repeats);
void encodeNEC(uint32_t raw_data, rmt_symbol_word_t *symbols, size_t *symbol_count);
uint32_t NECRaw(uint16_t address, uint8_t command);

void setup()
{
    M5.begin();
    Serial.begin(115200);

    // Display initialization
    M5.Display.setRotation(3);
    M5.Display.setFont(&fonts::FreeMonoBold9pt7b); // Fix: setTextFont -> setFont
    M5.Display.clear();
    M5.Display.setCursor(0, 0);
    M5.Display.printf("StickS3 IR example");

    Serial.println("StickS3 IR example");

    // Initialize RMT TX channel
    setup_rmt_tx();

    Serial.printf("IR Send Pin: %d\n", IR_SEND_PIN);

    // Enable external power output for IR LED module
    M5.Power.setExtOutput(true);
    delay(100);
}

void loop()
{
    // Build 32-bit NEC frame data
    uint32_t raw = NECRaw(address, command);

    Serial.printf("Send NEC: addr=0x%04X, cmd=0x%02X, raw=0x%08X\n", address, command, raw);

    // -------- Send NEC frame --------
    sendNEC(address, command, repeats);

    M5.Display.fillRect(0, 30, 240, 105, TFT_BLACK);
    M5.Display.setCursor(0, 30);
    M5.Display.printf("Send NEC:\n");
    M5.Display.printf(" addr=0x%04X\n", address);
    M5.Display.printf(" cmd =0x%02X\n", command);
    M5.Display.printf(" raw =0x%08X\n", raw);

    address += 0x0001;
    command += 0x01;
    repeats = 0;

    delay(2000);
}

// Initialize RMT TX channel
void setup_rmt_tx()
{
    // Configure RMT TX channel
    rmt_tx_channel_config_t tx_chan_config = {
        .gpio_num = (gpio_num_t)IR_SEND_PIN,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1 us per tick
        .mem_block_symbols = 64,
        .trans_queue_depth = 4,
        .flags = {
            .invert_out = false,
            .with_dma = false,
        }};
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));

    // Configure 38kHz carrier wave for IR transmission
    rmt_carrier_config_t carrier_cfg = {
        .frequency_hz = IR_CARRIER_FREQ_HZ,
        .duty_cycle = IR_DUTY_CYCLE,
        .flags = {
            .polarity_active_low = false,
        }};
    ESP_ERROR_CHECK(rmt_apply_carrier(tx_chan, &carrier_cfg));

    // Create copy encoder for pre-encoded symbols
    rmt_copy_encoder_config_t encoder_config = {};
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&encoder_config, &copy_encoder));

    // Enable TX channel
    ESP_ERROR_CHECK(rmt_enable(tx_chan));
}

/*
 * Send NEC IR frame via RMT.
 *
 * @param address NEC address (8-bit or 16-bit)
 * @param command NEC command byte
 * @param repeats Number of repeat frames to send
 *
 * @return true if transmission successful
 */
bool sendNEC(uint16_t address, uint8_t command, uint8_t repeats)
{
    // Build 32-bit NEC raw data
    uint32_t raw = NECRaw(address, command);

    // Buffer for RMT symbols
    rmt_symbol_word_t symbols[68]; // Header + 32 bits + ending mark
    size_t symbol_count = 0;

    encodeNEC(raw, symbols, &symbol_count);

    // RMT transmit configuration
    rmt_transmit_config_t tx_config = {
        .loop_count = 0,
        .flags = {
            .eot_level = 0,
        }};

    // Transmit the frame
    esp_err_t ret = rmt_transmit(tx_chan, copy_encoder, symbols,
                                 symbol_count * sizeof(rmt_symbol_word_t),
                                 &tx_config);

    if (ret == ESP_OK)
    {
        // Wait for transmission completion
        ret = rmt_tx_wait_all_done(tx_chan, 1000);
    }

    // Send repeat frames if requested
    for (int i = 0; i < repeats; i++)
    {
        delay(108); // NEC repeat frame interval

        // TODO: Implement repeat frame
        // Repeat frame: 9ms mark + 2.25ms space + 560us mark
    }

    return (ret == ESP_OK);
}

/*
 * Encode NEC protocol data into RMT symbols.
 *
 * @param raw_data     32-bit NEC frame data
 * @param symbols      Output buffer for RMT symbols
 * @param symbol_count Number of symbols generated
 */
void encodeNEC(uint32_t raw_data, rmt_symbol_word_t *symbols, size_t *symbol_count)
{
    size_t idx = 0;

    // NEC header: ~9 ms mark + ~4.5 ms space
    symbols[idx].duration0 = NEC_HEADER_MARK;
    symbols[idx].level0 = 1;
    symbols[idx].duration1 = NEC_HEADER_SPACE;
    symbols[idx].level1 = 0;
    idx++;

    // Encode 32 data bits (LSB first)
    for (int i = 0; i < 32; i++)
    {
        // Mark duration: always 560 us
        symbols[idx].duration0 = NEC_BIT_MARK;
        symbols[idx].level0 = 1;

        // Space duration distinguishes logic 0 and logic 1
        if (raw_data & (1UL << i))
        {
            symbols[idx].duration1 = NEC_BIT_1_SPACE; // Logic 1: 1690 us
        }
        else
        {
            symbols[idx].duration1 = NEC_BIT_0_SPACE; // Logic 0: 560 us
        }
        symbols[idx].level1 = 0;
        idx++;
    }

    // Ending mark: 560 us
    symbols[idx].duration0 = NEC_BIT_MARK;
    symbols[idx].level0 = 1;
    symbols[idx].duration1 = 0;
    symbols[idx].level1 = 0;
    idx++;

    *symbol_count = idx;
}

/*
 * Build 32-bit NEC raw data from address and command.
 *
 * NEC frame format (LSB first):
 *   bit  0-15 : Address field (8-bit + inverse, or full 16-bit)
 *   bit 16-23 : Command byte
 *   bit 24-31 : Inverse of command byte
 *
 * @param address NEC address (8-bit with auto-inverse, or 16-bit extended)
 * @param command NEC command byte
 *
 * @return 32-bit NEC raw data ready for encoding
 */
uint32_t NECRaw(uint16_t address, uint8_t command)
{
    uint16_t nec_addr;

    // Standard NEC: 8-bit address + inverse byte
    if (address <= 0x00FF)
    {
        uint8_t addr8 = address & 0xFF;
        nec_addr = ((uint16_t)(~addr8) << 8) | addr8;
    }
    // Extended NEC: full 16-bit address
    else
    {
        nec_addr = address;
    }

    // Assemble 32-bit NEC frame
    uint32_t raw = 0;
    raw |= (uint32_t)nec_addr;         // Address field
    raw |= (uint32_t)command << 16;    // Command byte
    raw |= (uint32_t)(~command) << 24; // Inverted command byte

    return raw;
}
#include "M5Unified.h"
#include "driver/rmt_rx.h"

// https://docs.m5stack.com/en/arduino/m5sticks3/ir_nec

#define IR_RECEIVE_PIN 42

rmt_channel_handle_t rx_chan = NULL;

bool decodeNEC(rmt_symbol_word_t *rx_raw_symbols, uint32_t *out_raw, bool *out_repeat);

// Initialize RMT RX channel
void setup_rmt_rx()
{
    rmt_rx_channel_config_t rx_chan_config = {
        .gpio_num = (gpio_num_t)IR_RECEIVE_PIN,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1 us per tick
        .mem_block_symbols = 128,
    };
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_chan_config, &rx_chan));
    ESP_ERROR_CHECK(rmt_enable(rx_chan));
}

void setup()
{
    auto cfg = M5.config();
    cfg.internal_spk = false; // Disable speaker amp to avoid IR RX interference
    M5.begin(cfg);

    Serial.begin(115200);

    // Display initialization
    M5.Display.setRotation(3);
    M5.Display.setFont(&fonts::FreeMonoBold9pt7b);
    M5.Display.clear();
    M5.Display.println("StickS3 IR example");
    M5.Display.setCursor(0, 30);
    M5.Display.println("Waiting for NEC...");

    setup_rmt_rx();

    // Enable external power output for IR receiver module
    M5.Power.setExtOutput(true, m5::ext_none);
}

void loop()
{
    M5.update();

    // Buffer for received RMT symbols
    rmt_symbol_word_t rx_raw_symbols[64];

    // RMT receive configuration
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1000,
        .signal_range_max_ns = 20000000};

    if (rmt_receive(rx_chan, rx_raw_symbols, sizeof(rx_raw_symbols), &receive_config) == ESP_OK)
    {
        delay(100); // Allow DMA buffer to be fully populated

        uint32_t rx_data = 0;
        bool repeat_frame = false;

        // -------- Decode NEC frame --------
        bool valid = decodeNEC(rx_raw_symbols, &rx_data, &repeat_frame);

        if (repeat_frame)
        {
            Serial.println("NEC Repeat Frame");

            M5.Display.fillRect(0, 30, 240, 105, TFT_BLACK);
            M5.Display.setCursor(0, 30);
            M5.Display.setTextColor(YELLOW);
            M5.Display.println("NEC Repeat");
        }
        else if (valid)
        {
            uint16_t rx_addr = rx_data & 0xFFFF;
            uint8_t rx_cmd = (rx_data >> 16) & 0xFF;

            Serial.printf("Received NEC: Addr: 0x%04X, Cmd: 0x%02X, Raw: 0x%08X\n", rx_addr, rx_cmd, rx_data);

            M5.Display.fillRect(0, 30, 240, 105, TFT_BLACK);
            M5.Display.setCursor(0, 30);
            M5.Display.setTextColor(GREEN);
            M5.Display.printf("Received NEC:\n");
            M5.Display.printf("Addr: 0x%04X\n", rx_addr);
            M5.Display.printf("Cmd:  0x%02X\n", rx_cmd);
            M5.Display.printf("Raw:  0x%08X\n", rx_data);
        }
        else
        {
            Serial.println("Signal received, but not a valid NEC frame.");
        }
    }

    delay(10);
    M5.Display.setTextColor(WHITE);
}

/*
 * Decode a NEC IR frame from RMT symbols.
 *
 * @param rx_raw_symbols Pointer to RMT RX symbol buffer
 * @param out_raw        Decoded 32-bit NEC raw data (LSB first)
 * @param out_repeat     Set to true if a NEC repeat frame is detected
 *
 * @return true if a valid NEC data frame is decoded
 */
bool decodeNEC(rmt_symbol_word_t *rx_raw_symbols, uint32_t *out_raw, bool *out_repeat)
{

    *out_raw = 0;
    *out_repeat = false;

    uint32_t header_low = rx_raw_symbols[0].duration0;
    uint32_t header_high = rx_raw_symbols[0].duration1;

    // Standard NEC header: ~9 ms LOW + ~4.5 ms HIGH
    if (header_low > 8000 && header_high > 4000)
    {
        // Valid NEC header, continue decoding
    }
    // NEC repeat frame: ~9 ms LOW + ~2.25 ms HIGH
    else if (header_low > 8000 &&
             header_high > 2000 &&
             header_high < 3000)
    {
        *out_repeat = true;
        return false;
    }
    else
    {
        return false;
    }

    // Decode 32 NEC data bits (LSB first)
    for (int i = 0; i < 32; i++)
    {
        uint32_t mark = rx_raw_symbols[i + 1].duration0;
        uint32_t space = rx_raw_symbols[i + 1].duration1;

        // NEC mark duration should be ~560 us
        if (mark < 300 || mark > 800)
        {
            return false;
        }

        // Space duration distinguishes logic 0 and logic 1
        if (space > 1000)
        {
            *out_raw |= (1UL << i);
        }
    }

    // Verify command byte and its inverse
    uint8_t cmd = (*out_raw >> 16) & 0xFF;
    uint8_t cmd_inv = (*out_raw >> 24) & 0xFF;

    if ((cmd ^ cmd_inv) != 0xFF)
    {
        return false;
    }

    return true;
}
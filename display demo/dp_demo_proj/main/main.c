#include "bsp/esp-bsp.h"
#include "lvgl.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_log.h>

#define ADC_CHANNEL   ADC_CHANNEL_7
#define NUM_BARS      5
#define BAR_W         5
#define BAR_H         10
#define BAR_GAP       3

void app_main(void)
{

    bsp_display_start();
    bsp_display_backlight_on();

    //Create battery indicator objects
    lv_obj_t *bars[NUM_BARS];
    bsp_display_lock(0);
    for (int i = 0; i < NUM_BARS; i++) {
        bars[i] = lv_obj_create(lv_screen_active());
        lv_obj_set_size(bars[i], BAR_W, BAR_H);
        lv_obj_align(bars[i], LV_ALIGN_TOP_MID, ((i - 2) * (BAR_W + BAR_GAP))+100, 0);
        lv_obj_set_style_border_width(bars[i], 0, 0);
        lv_obj_set_style_bg_color(bars[i], lv_palette_darken(LV_PALETTE_GREY, 3), 0);

        //Disable scrolling to hide ugly grey dots
        lv_obj_remove_flag(bars[i], LV_OBJ_FLAG_SCROLLABLE);
    }
    bsp_display_unlock();

    //Adc setup
    adc_oneshot_unit_handle_t adc_handle;

    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &chan_cfg));


    while (1) {

        //"Battery" voltage monitor. Sets amount of battery indicator bars
        int adc_value;
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_value));
        ESP_LOGI("ADC", "%d", adc_value);

        int bars_lit;
        if (adc_value < 819) {
            bars_lit = 0;
        } else if (adc_value < 1638) {
            bars_lit = 1;
        } else if (adc_value < 2457) {
            bars_lit = 2;
        } else if (adc_value < 3276) {
            bars_lit = 3;
        } else if (adc_value < 4095) {
            bars_lit = 4;
        } else {
            bars_lit = 5;
        }
        
        //Color green the appropriate amount of bars on the indicator
        bsp_display_lock(0);

        //Set background balck
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000), 0);

        for (int i = 0; i < NUM_BARS; i++) {
            if (i < bars_lit) {
                lv_obj_set_style_bg_color(bars[i], lv_color_hex(0x00FF00), 0);
            } else {
                lv_obj_set_style_bg_color(bars[i], lv_palette_darken(LV_PALETTE_GREY, 3), 0);
            }
        }
        bsp_display_unlock();

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
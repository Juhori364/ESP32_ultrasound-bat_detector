#include "bsp/esp-bsp.h"
#include "lvgl.h"

void app_main(void)
{
    bsp_display_start();
    bsp_display_backlight_on();

    bsp_display_lock(0);
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello LVGL");
    lv_obj_center(label);
    bsp_display_unlock();
}
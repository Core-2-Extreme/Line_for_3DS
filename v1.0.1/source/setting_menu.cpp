#include <string>
#include <cstring>
#include <unistd.h>
#include <3ds.h>

#include "draw.hpp"
#include "setting_menu.hpp"
#include "share_function.hpp"

bool setting_main_run = false;

std::string setting_string[16];

void Setting_menu_main(void)
{
	osTickCounterUpdate(&share_total_frame_time);

	setting_string[0] = "Screen bridgeness        : " + std::to_string(share_lcd_bridgeness);
	setting_string[1] = "Time to enter sleep mode : " + std::to_string(share_time_to_enter_afk / 10) + "s";
	setting_string[2] = "Scroll speed             : " + std::to_string(share_scroll_speed);

	Draw_set_draw_mode(1);
	Draw_screen_ready_to_draw(0, true, 1);
	Draw_texture(TEXTURE_BACKGROUND, 0, 0.0, 0.0);
	Draw_texture(TEXTURE_WIFI_ICON, share_wifi_signal, 360.0, 0.0);
	Draw_texture(TEXTURE_BATTERY_LEVEL_ICON, share_battery_level / 5, 330.0, 0.0);
	if (share_battery_charge)
		Draw_texture(TEXTURE_BATTERY_CHARGE_ICON, 0, 310.0, 0.0);
	Draw(share_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(share_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);

	Draw_screen_ready_to_draw(1, true, 1);

	Draw(setting_string[0], 0.0, 25.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	Draw("«   ª", 230.0, 25.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	Draw(setting_string[1], 0.0, 40.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	Draw("«   ª", 230.0, 40.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	Draw(setting_string[2], 0.0, 55.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	Draw("«   ª", 230.0, 55.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);

	Draw(share_test_string, 0.0, 100.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);


	Draw_texture(TEXTURE_BACKGROUND, 1, 0.0, 225.0);
	Draw(share_bot_button_string, 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);

	if (share_key_touch_held)
		Draw(share_circle_string, touch_pos.px, touch_pos.py, 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);

	Draw_apply_draw();
	share_fps += 1;
	share_frame_time_point[0] = osTickCounterRead(&share_total_frame_time);

	if (share_key_START_press || (share_key_touch_press && share_touch_pos_x >= 110 && share_touch_pos_x <= 230 && share_touch_pos_y >= 220 && share_touch_pos_y <= 240))
	{
		share_setting_main_run = false;
		share_menu_main_run = true;
	}
}

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

	setting_string[0] = "Language : " + share_setting[1];
	setting_string[1] = "Screen brightness : " + std::to_string(share_lcd_brightness);
	setting_string[2] = "Time to enter sleep mode : " + std::to_string(share_time_to_enter_afk / 10) + "s";
	setting_string[3] = "Screen brightness when sleep : " + std::to_string(share_afk_lcd_brightness);
	setting_string[5] = "Scroll speed : " + std::to_string(share_scroll_speed);
	if (share_allow_send_app_info)
		setting_string[6] = "Allow send app info : allow";
	else
		setting_string[6] = "Allow send app info : deny";

	Draw_set_draw_mode(1);
	Draw_screen_ready_to_draw(0, true, 1);
	Draw_texture(Background_image, 0, 0.0, 0.0, 400.0, 15.0);
	Draw_texture(Wifi_icon_image, share_wifi_signal, 360.0, 0.0, 15.0, 15.0);
	Draw_texture(Battery_level_icon_image, share_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
	if (share_battery_charge)
		Draw_texture(Battery_charge_icon_image, 0, 310.0, 0.0, 20.0, 15.0);
	Draw(share_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(share_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);

	Draw_screen_ready_to_draw(1, true, 1);

	Draw(setting_string[0], 0.0, 0.0 + share_setting_menu_y_offset, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	if (share_setting[1] == "en")
		Draw("en", 10.0, 10.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
	else
		Draw("en", 10.0, 10.0 + share_setting_menu_y_offset, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0);

	if (share_setting[1] == "jp")
		Draw("jp", 60.0, 10.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
	else
		Draw("jp", 60.0, 10.0 + share_setting_menu_y_offset, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0);

	Draw(setting_string[1], 0.0, 40.0 + share_setting_menu_y_offset, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	Draw(setting_string[2], 0.0, 80.0 + share_setting_menu_y_offset, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	Draw(setting_string[3], 0.0, 120.0 + share_setting_menu_y_offset, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	Draw(setting_string[5], 0.0, 160.0 + share_setting_menu_y_offset, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	Draw(setting_string[6], 0.0, 200.0 + share_setting_menu_y_offset, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);

	if (!share_allow_send_app_info)
		Draw("deny", 10.0, 210.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
	else
		Draw("deny", 10.0, 210.0 + share_setting_menu_y_offset, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0);

	if (share_allow_send_app_info)
		Draw("allow", 80.0, 210.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
	else
		Draw("allow", 80.0, 210.0 + share_setting_menu_y_offset, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0);

	Draw_texture(Setting_bar_image, 0, (share_lcd_brightness * 2), 55.0 + share_setting_menu_y_offset, 4.0, 20.0);
	Draw_texture(Setting_bar_image, 0, (share_time_to_enter_afk / 10), 95.0 + share_setting_menu_y_offset, 4.0, 20.0);
	Draw_texture(Setting_bar_image, 0, (share_afk_lcd_brightness * 2), 135.0 + share_setting_menu_y_offset, 4.0, 20.0);
	Draw_texture(Setting_bar_image, 0, (share_scroll_speed * 300), 175.0 + share_setting_menu_y_offset, 4.0, 20.0);

	Draw_texture(Background_image, 1, 0.0, 225.0, 320.0, 15.0);
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

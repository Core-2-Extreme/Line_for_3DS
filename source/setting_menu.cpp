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
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	int texture_color;

	osTickCounterUpdate(&share_total_frame_time);

	if (share_setting[1] == "en")
	{
		setting_string[0] = share_setting_menu_message_en[0] + share_setting[1];
		if (share_night_mode)
			setting_string[1] = share_setting_menu_message_en[1];
		else
			setting_string[1] = share_setting_menu_message_en[2];

		if (share_draw_vsync_mode)
			setting_string[2] = share_setting_menu_message_en[3];
		else
			setting_string[2] = share_setting_menu_message_en[4];

		setting_string[3] = share_setting_menu_message_en[5] + std::to_string(share_lcd_brightness);
		setting_string[4] = share_setting_menu_message_en[6] + std::to_string(share_time_to_enter_afk / 10) + share_setting_menu_message_en[7];
		setting_string[5] = share_setting_menu_message_en[8] + std::to_string(share_afk_lcd_brightness);
		setting_string[6] = share_setting_menu_message_en[9] + std::to_string(share_scroll_speed);
		if (share_allow_send_app_info)
			setting_string[7] = share_setting_menu_message_en[10];
		else
			setting_string[7] = share_setting_menu_message_en[11];

		if (share_debug_mode)
			setting_string[8] = share_setting_menu_message_en[12];
		else
			setting_string[8] = share_setting_menu_message_en[13];
	}
	else if (share_setting[1] == "jp")
	{
		setting_string[0] = share_setting_menu_message_jp[0] + share_setting[1];
		if (share_night_mode)
			setting_string[1] = share_setting_menu_message_jp[1];
		else
			setting_string[1] = share_setting_menu_message_jp[2];

		if (share_draw_vsync_mode)
			setting_string[2] = share_setting_menu_message_jp[3];
		else
			setting_string[2] = share_setting_menu_message_jp[4];

		setting_string[3] = share_setting_menu_message_jp[5] + std::to_string(share_lcd_brightness);
		setting_string[4] = share_setting_menu_message_jp[6] + std::to_string(share_time_to_enter_afk / 10) + share_setting_menu_message_jp[7];
		setting_string[5] = share_setting_menu_message_jp[8] + std::to_string(share_afk_lcd_brightness);
		setting_string[6] = share_setting_menu_message_jp[9] + std::to_string(share_scroll_speed);
		if (share_allow_send_app_info)
			setting_string[7] = share_setting_menu_message_jp[10];
		else
			setting_string[7] = share_setting_menu_message_jp[11];

		if (share_debug_mode)
			setting_string[8] = share_setting_menu_message_jp[12];
		else
			setting_string[8] = share_setting_menu_message_jp[13];
	}

	if (share_night_mode)
	{
		text_red = 1.0;
		text_green = 1.0;
		text_blue = 1.0;
		text_alpha = 0.75;
		texture_color = 12;
	}
	else
	{
		text_red = 0.0;
		text_green = 0.0;
		text_blue = 0.0;
		text_alpha = 1.0;
		texture_color = 0;
	}

	Draw_set_draw_mode(share_draw_vsync_mode);
	if (share_night_mode)
		Draw_screen_ready_to_draw(0, true, 1, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 1, 1.0, 1.0, 1.0);

	Draw_texture(Background_image, 0, 0.0, 0.0, 400.0, 15.0);
	Draw_texture(Wifi_icon_image, share_wifi_signal, 360.0, 0.0, 15.0, 15.0);
	Draw_texture(Battery_level_icon_image, share_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
	if (share_battery_charge)
		Draw_texture(Battery_charge_icon_image, 0, 310.0, 0.0, 20.0, 15.0);
	Draw(share_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(share_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);

	if (share_debug_mode)
	{
		Draw_texture(Square_image, 9, 0.0, 50.0, 230.0, 140.0);
		Draw("Key A press : " + std::to_string(share_key_A_press) + " Key A held : " + std::to_string(share_key_A_held), 0.0, 50.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key B press : " + std::to_string(share_key_B_press) + " Key B held : " + std::to_string(share_key_B_held), 0.0, 60.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key X press : " + std::to_string(share_key_X_press) + " Key X held : " + std::to_string(share_key_X_held), 0.0, 70.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key Y press : " + std::to_string(share_key_Y_press) + " Key Y held : " + std::to_string(share_key_Y_held), 0.0, 80.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD DOWN held : " + std::to_string(share_key_CPAD_DOWN_held), 0.0, 90.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD RIGHT held : " + std::to_string(share_key_CPAD_RIGHT_held), 0.0, 100.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD UP held : " + std::to_string(share_key_CPAD_UP_held), 0.0, 110.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD LEFT held : " + std::to_string(share_key_CPAD_LEFT_held), 0.0, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Touch pos x : " + std::to_string(share_touch_pos_x) + " Touch pos y : " + std::to_string(share_touch_pos_y), 0.0, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("X moved value : " + std::to_string(share_touch_pos_x_moved) + " Y moved value : " + std::to_string(share_touch_pos_y_moved), 0.0, 140.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Held time : " + std::to_string(share_held_time), 0.0, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Drawing time(CPU/per frame) : " + std::to_string(C3D_GetProcessingTime()) + "ms", 0.0, 160.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Drawing time(GPU/per frame) : " + std::to_string(C3D_GetDrawingTime()) + "ms", 0.0, 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Free RAM (estimate) " + std::to_string(share_free_ram) + " MB", 0.0f, 180.0f, 0.4f, 0.4, text_red, text_green, text_blue, text_alpha);
	}
	if (share_app_logs_show)
	{
		for (int i = 0; i < 23; i++)
			Draw(share_app_logs[share_app_log_view_num + i], share_app_log_x, 10.0f + (i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
	}

	if (share_night_mode)
		Draw_screen_ready_to_draw(1, true, 1, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(1, true, 1, 1.0, 1.0, 1.0);

	Draw(setting_string[0], 0.0, 0.0 + share_setting_menu_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(setting_string[1], 0.0, 40.0 + share_setting_menu_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(setting_string[2], 0.0, 80.0 + share_setting_menu_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(setting_string[3], 0.0, 120.0 + share_setting_menu_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(setting_string[4], 0.0, 160.0 + share_setting_menu_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(setting_string[5], 0.0, 200.0 + share_setting_menu_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(setting_string[6], 0.0, 240.0 + share_setting_menu_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(setting_string[7], 0.0, 280.0 + share_setting_menu_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(setting_string[8], 0.0, 320.0 + share_setting_menu_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	
	if (share_setting[1] == "en")
	{
		Draw(share_setting_menu_message_en[14], 10.0, 10.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
		Draw(share_setting_menu_message_en[15], 60.0, 10.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
		if (share_night_mode)
		{
			Draw(share_setting_menu_message_en[16], 10.0, 50.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
			Draw(share_setting_menu_message_en[17], 60.0, 50.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
		}
		else
		{
			Draw(share_setting_menu_message_en[16], 10.0, 50.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
			Draw(share_setting_menu_message_en[17], 60.0, 50.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
		}

		if (share_draw_vsync_mode)
		{
			Draw(share_setting_menu_message_en[16], 10.0, 90.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
			Draw(share_setting_menu_message_en[17], 60.0, 90.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
		}
		else
		{
			Draw(share_setting_menu_message_en[16], 10.0, 90.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
			Draw(share_setting_menu_message_en[17], 60.0, 90.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
		}

		if (share_allow_send_app_info)
		{
			Draw(share_setting_menu_message_en[18], 10.0, 290.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
			Draw(share_setting_menu_message_en[19], 80.0, 290.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
		}
		else
		{
			Draw(share_setting_menu_message_en[18], 10.0, 290.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
			Draw(share_setting_menu_message_en[19], 80.0, 290.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
		}

		if (share_debug_mode)
		{
			Draw(share_setting_menu_message_en[16], 10.0, 330.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
			Draw(share_setting_menu_message_en[17], 80.0, 330.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
		}
		else
		{
			Draw(share_setting_menu_message_en[16], 10.0, 330.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
			Draw(share_setting_menu_message_en[17], 80.0, 330.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
		}
	}
	else if (share_setting[1] == "jp")
	{
		Draw(share_setting_menu_message_jp[14], 10.0, 10.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
		Draw(share_setting_menu_message_jp[15], 60.0, 10.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
		if (share_night_mode)
		{
			Draw(share_setting_menu_message_jp[16], 10.0, 50.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
			Draw(share_setting_menu_message_jp[17], 60.0, 50.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
		}
		else
		{
			Draw(share_setting_menu_message_jp[16], 10.0, 50.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
			Draw(share_setting_menu_message_jp[17], 60.0, 50.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
		}

		if (share_draw_vsync_mode)
		{
			Draw(share_setting_menu_message_jp[16], 10.0, 90.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
			Draw(share_setting_menu_message_jp[17], 60.0, 90.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
		}
		else
		{
			Draw(share_setting_menu_message_jp[16], 10.0, 90.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
			Draw(share_setting_menu_message_jp[17], 60.0, 90.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
		}

		if (share_allow_send_app_info)
		{
			Draw(share_setting_menu_message_jp[18], 10.0, 290.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
			Draw(share_setting_menu_message_jp[19], 80.0, 290.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
		}
		else
		{
			Draw(share_setting_menu_message_jp[18], 10.0, 290.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
			Draw(share_setting_menu_message_jp[19], 80.0, 290.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
		}

		if (share_debug_mode)
		{
			Draw(share_setting_menu_message_jp[16], 10.0, 330.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
			Draw(share_setting_menu_message_jp[17], 80.0, 330.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
		}
		else
		{
			Draw(share_setting_menu_message_jp[16], 10.0, 330.0 + share_setting_menu_y_offset, 1.0, 1.0, text_red, text_green, text_blue, text_alpha);
			Draw(share_setting_menu_message_jp[17], 80.0, 330.0 + share_setting_menu_y_offset, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
		}
	}

	Draw_texture(Square_image, texture_color, (share_lcd_brightness * 2), 135.0 + share_setting_menu_y_offset, 4.0, 20.0);
	Draw_texture(Square_image, texture_color, (share_time_to_enter_afk / 10), 175.0 + share_setting_menu_y_offset, 4.0, 20.0);
	Draw_texture(Square_image, texture_color, (share_afk_lcd_brightness * 2), 215.0 + share_setting_menu_y_offset, 4.0, 20.0);
	Draw_texture(Square_image, texture_color, (share_scroll_speed * 300), 255.0 + share_setting_menu_y_offset, 4.0, 20.0);

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

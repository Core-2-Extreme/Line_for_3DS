#include <3ds.h>
#include <string>
#include <unistd.h>

#include "hid.hpp"
#include "speedtest.hpp"
#include "image_viewer.hpp"
#include "google_translation.hpp"
#include "line.hpp"
#include "file.hpp"
#include "setting_menu.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "explorer.hpp"
#include "log.hpp"
#include "camera.hpp"
#include "mic.hpp"
#include "music_player.hpp"
#include "draw.hpp"

bool hid_scan_hid_thread_run = false;
bool hid_key_A_press = false;
bool hid_key_B_press = false;
bool hid_key_X_press = false;
bool hid_key_Y_press = false;
bool hid_key_C_UP_press = false;
bool hid_key_C_RIGHT_press = false;
bool hid_key_C_DOWN_press = false;
bool hid_key_C_LEFT_press = false;
bool hid_key_D_UP_press = false;
bool hid_key_D_RIGHT_press = false;
bool hid_key_D_DOWN_press = false;
bool hid_key_D_LEFT_press = false;
bool hid_key_L_press = false;
bool hid_key_R_press = false;
bool hid_key_ZL_press = false;
bool hid_key_ZR_press = false;
bool hid_key_START_press = false;
bool hid_key_SELECT_press = false;
bool hid_key_CS_UP_press = false;
bool hid_key_CS_DOWN_press = false;
bool hid_key_CS_RIGHT_press = false;
bool hid_key_CS_LEFT_press = false;
bool hid_key_touch_press = false;
bool hid_key_A_held = false;
bool hid_key_B_held = false;
bool hid_key_X_held = false;
bool hid_key_Y_held = false;
bool hid_key_C_UP_held = false;
bool hid_key_C_DOWN_held = false;
bool hid_key_C_RIGHT_held = false;
bool hid_key_C_LEFT_held = false;
bool hid_key_D_UP_held = false;
bool hid_key_D_DOWN_held = false;
bool hid_key_D_RIGHT_held = false;
bool hid_key_D_LEFT_held = false;
bool hid_key_L_held = false;
bool hid_key_R_held = false;
bool hid_key_ZL_held = false;
bool hid_key_ZR_held = false;
bool hid_key_START_held = false;
bool hid_key_SELECT_held = false;
bool hid_key_CS_UP_held = false;
bool hid_key_CS_DOWN_held = false;
bool hid_key_CS_RIGHT_held = false;
bool hid_key_CS_LEFT_held = false;
bool hid_key_touch_held = false;
bool hid_disabled = false;
int hid_touch_pos_x;
int hid_touch_pos_x_before;
int hid_touch_pos_x_moved;
int hid_touch_pos_y;
int hid_touch_pos_y_before;
int hid_touch_pos_y_moved;
int hid_held_time = 0;
double hid_touch_pos_x_move_left;
double hid_touch_pos_y_move_left;
std::string hid_scan_hid_thread_string = "Hid/Scan hid thread";
Thread hid_scan_hid_thread;

void Hid_init(void)
{
	hid_scan_hid_thread_run = true;
	hid_scan_hid_thread = threadCreate(Hid_scan_hid_thread, (void*)(""), STACKSIZE, PRIORITY_REALTIME, -1, false);
}

void Hid_exit(void)
{
	hid_scan_hid_thread_run = false;
	threadJoin(hid_scan_hid_thread, 10000000000);
	threadFree(hid_scan_hid_thread);
}


bool Hid_query_disable_flag(void)
{
	return hid_disabled;
}

bool Hid_query_key_press_state(int key_num)
{
	if (key_num == KEY_P_A)
		return hid_key_A_press;
	else if (key_num == KEY_P_B)
		return hid_key_B_press;
	else if (key_num == KEY_P_X)
		return hid_key_X_press;
	else if (key_num == KEY_P_Y)
		return hid_key_Y_press;
	else if (key_num == KEY_P_C_UP)
		return hid_key_C_UP_press;
	else if (key_num == KEY_P_C_RIGHT)
		return hid_key_C_RIGHT_press;
	else if (key_num == KEY_P_C_LEFT)
		return hid_key_C_LEFT_press;
	else if (key_num == KEY_P_C_DOWN)
		return hid_key_C_DOWN_press;
	else if (key_num == KEY_P_D_UP)
		return hid_key_D_UP_press;
	else if (key_num == KEY_P_D_RIGHT)
		return hid_key_D_RIGHT_press;
	else if (key_num == KEY_P_D_LEFT)
		return hid_key_D_LEFT_press;
	else if (key_num == KEY_P_D_DOWN)
		return hid_key_D_DOWN_press;
	else if (key_num == KEY_P_L)
		return hid_key_L_press;
	else if (key_num == KEY_P_R)
		return hid_key_R_press;
	else if (key_num == KEY_P_ZL)
		return hid_key_ZL_press;
	else if (key_num == KEY_P_ZR)
		return hid_key_ZR_press;
	else if (key_num == KEY_P_START)
		return hid_key_START_press;
	else if (key_num == KEY_P_SELECT)
		return hid_key_SELECT_press;
	else if (key_num == KEY_P_CS_UP)
		return hid_key_CS_UP_press;
	else if (key_num == KEY_P_CS_RIGHT)
		return hid_key_CS_RIGHT_press;
	else if (key_num == KEY_P_CS_LEFT)
		return hid_key_CS_LEFT_press;
	else if (key_num == KEY_P_CS_DOWN)
		return hid_key_CS_DOWN_press;
	else if (key_num == KEY_P_TOUCH)
		return hid_key_touch_press;
	else
		return false;
}

bool Hid_query_key_held_state(int key_num)
{
	if (key_num == KEY_H_A)
		return hid_key_A_held;
	else if (key_num == KEY_H_B)
		return hid_key_B_held;
	else if (key_num == KEY_H_X)
		return hid_key_X_held;
	else if (key_num == KEY_H_Y)
		return hid_key_Y_held;
	else if (key_num == KEY_H_C_UP)
		return hid_key_C_UP_held;
	else if (key_num == KEY_H_C_RIGHT)
		return hid_key_C_RIGHT_held;
	else if (key_num == KEY_H_C_LEFT)
		return hid_key_C_LEFT_held;
	else if (key_num == KEY_H_C_DOWN)
		return hid_key_C_DOWN_held;
	else if (key_num == KEY_H_D_UP)
		return hid_key_D_UP_held;
	else if (key_num == KEY_H_D_RIGHT)
		return hid_key_D_RIGHT_held;
	else if (key_num == KEY_H_D_LEFT)
		return hid_key_D_LEFT_held;
	else if (key_num == KEY_H_D_DOWN)
		return hid_key_D_DOWN_held;
	else if (key_num == KEY_H_L)
		return hid_key_L_held;
	else if (key_num == KEY_H_R)
		return hid_key_R_held;
	else if (key_num == KEY_H_ZL)
		return hid_key_ZL_held;
	else if (key_num == KEY_H_ZR)
		return hid_key_ZR_held;
	else if (key_num == KEY_H_START)
		return hid_key_START_held;
	else if (key_num == KEY_H_SELECT)
		return hid_key_SELECT_held;
	else if (key_num == KEY_H_CS_UP)
		return hid_key_CS_UP_held;
	else if (key_num == KEY_H_CS_RIGHT)
		return hid_key_CS_RIGHT_held;
	else if (key_num == KEY_H_CS_LEFT)
		return hid_key_CS_LEFT_held;
	else if (key_num == KEY_H_CS_DOWN)
		return hid_key_CS_DOWN_held;
	else if (key_num == KEY_H_TOUCH)
		return hid_key_touch_held;
	else
		return false;
}

int Hid_query_touch_pos(bool x)
{
	if (x)
		return hid_touch_pos_x;
	else
		return hid_touch_pos_y;
}

void Hid_key_flag_reset(void)
{
	hid_key_A_press = false;
	hid_key_B_press = false;
	hid_key_X_press = false;
	hid_key_Y_press = false;
	hid_key_C_UP_press = false;
	hid_key_C_DOWN_press = false;
	hid_key_C_RIGHT_press = false;
	hid_key_C_LEFT_press = false;
	hid_key_D_UP_press = false;
	hid_key_D_DOWN_press = false;
	hid_key_D_RIGHT_press = false;
	hid_key_D_LEFT_press = false;
	hid_key_L_press = false;
	hid_key_R_press = false;
	hid_key_ZL_press = false;
	hid_key_ZR_press = false;
	hid_key_START_press = false;
	hid_key_SELECT_press = false;
	hid_key_CS_UP_press = false;
	hid_key_CS_DOWN_press = false;
	hid_key_CS_RIGHT_press = false;
	hid_key_CS_LEFT_press = false;
	hid_key_touch_press = false;
	hid_key_A_held = false;
	hid_key_B_held = false;
	hid_key_X_held = false;
	hid_key_Y_held = false;
	hid_key_C_UP_held = false;
	hid_key_C_DOWN_held = false;
	hid_key_C_RIGHT_held = false;
	hid_key_C_LEFT_held = false;
	hid_key_D_UP_held = false;
	hid_key_D_DOWN_held = false;
	hid_key_D_RIGHT_held = false;
	hid_key_D_LEFT_held = false;
	hid_key_L_held = false;
	hid_key_R_held = false;
	hid_key_ZL_held = false;
	hid_key_ZR_held = false;
	hid_key_START_held = false;
	hid_key_SELECT_held = false;
	hid_key_CS_UP_held = false;
	hid_key_CS_DOWN_held = false;
	hid_key_CS_RIGHT_held = false;
	hid_key_CS_LEFT_held = false;
	hid_key_touch_held = false;
	hid_touch_pos_x = 0;
	hid_touch_pos_y = 0;
}

void Hid_set_disable_flag(bool flag)
{
	hid_disabled = flag;
}

void Hid_scan_hid_thread(void* arg)
{
	Log_log_save(hid_scan_hid_thread_string, "Thread started.", 1234567890, false);

	int log_num;
	int load_font_num = 0;
	int pos_x = 0;
	int pos_y = 0;
	int cool_time = 0;
	double saved_y[2] = { 0.0, 0.0, };
	double scroll_speed;
	double sem_y;
	double line_x;
	double line_y;
	double line_interval;
	double line_size;
	u32 kDown;
	u32 kHeld;
	size_t cut_pos[2];
	size_t type_pos[2];
	bool scroll_bar_selected = false;
	bool bar_selected[9] = { false, false, false, false, false, false, false, false, false, };
	bool button_selected[4] = { false, false, false, false, };
	bool scroll_mode = false;
	bool do_not_scroll_mode = false;
	bool line_init = false;
	bool gtr_init = false;
	bool spt_init = false;
	bool imv_init = false;
	bool cam_init = false;
	bool mic_init = false;
	bool mup_init = false;
	bool line_run = false;
	bool gtr_run = false;
	bool spt_run = false;
	bool imv_run = false;
	bool cam_run = false;
	bool mic_run = false;
	bool mup_run = false;
	bool sem_run = false;
	std::string cache_string;
	std::string content;
	touchPosition touch_pos;
	circlePosition circle_pos;
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	Result_with_string result;

	while (hid_scan_hid_thread_run)
	{
		Hid_key_flag_reset();

		hidScanInput();
		hidTouchRead(&touch_pos);
		hidCircleRead(&circle_pos);
		kHeld = hidKeysHeld();
		kDown = hidKeysDown();

		scroll_speed = Sem_query_settings_d(SEM_SCROLL_SPEED);

		if (kDown & KEY_A)
			hid_key_A_press = true;
		if (kDown & KEY_B)
			hid_key_B_press = true;
		if (kDown & KEY_X)
			hid_key_X_press = true;
		if (kDown & KEY_Y)
			hid_key_Y_press = true;
		if (kDown & KEY_DUP)
			hid_key_D_UP_press = true;
		if (kDown & KEY_DDOWN)
			hid_key_D_DOWN_press = true;
		if (kDown & KEY_DRIGHT)
			hid_key_D_RIGHT_press = true;
		if (kDown & KEY_DLEFT)
			hid_key_D_LEFT_press = true;
		if (kDown & KEY_CPAD_UP)
			hid_key_C_UP_press = true;
		if (kDown & KEY_CPAD_DOWN)
			hid_key_C_DOWN_press = true;
		if (kDown & KEY_CPAD_RIGHT)
			hid_key_C_RIGHT_press = true;
		if (kDown & KEY_CPAD_LEFT)
			hid_key_C_LEFT_press = true;
		if (kDown & KEY_SELECT)
			hid_key_SELECT_press = true;
		if (kDown & KEY_START)
			hid_key_START_press = true;
		if (kDown & KEY_L)
			hid_key_L_press = true;
		if (kDown & KEY_R)
			hid_key_R_press = true;
		if (kDown & KEY_ZL)
			hid_key_ZL_press = true;
		if (kDown & KEY_ZR)
			hid_key_ZR_press = true;
		if (kHeld & KEY_DRIGHT)
			hid_key_D_RIGHT_held = true;
		if (kHeld & KEY_DLEFT)
			hid_key_D_LEFT_held = true;
		if (kHeld & KEY_DDOWN)
			hid_key_D_DOWN_held = true;
		if (kHeld & KEY_DUP)
			hid_key_D_UP_held = true;
		if (kHeld & KEY_A)
			hid_key_A_held = true;
		if (kHeld & KEY_B)
			hid_key_B_held = true;
		if (kHeld & KEY_Y)
			hid_key_Y_held = true;
		if (kHeld & KEY_X)
			hid_key_X_held = true;
		if (kHeld & KEY_CPAD_UP)
			hid_key_C_UP_held = true;
		if (kHeld & KEY_CPAD_DOWN)
			hid_key_C_DOWN_held = true;
		if (kHeld & KEY_CPAD_RIGHT)
			hid_key_C_RIGHT_held = true;
		if (kHeld & KEY_CPAD_LEFT)
			hid_key_C_LEFT_held = true;
		if (kHeld & KEY_L)
			hid_key_L_held = true;
		if (kHeld & KEY_R)
			hid_key_R_held = true;
		if (kDown & KEY_ZL)
			hid_key_ZL_held = true;
		if (kDown & KEY_ZR)
			hid_key_ZR_held = true;
		if (kDown & KEY_TOUCH || kHeld & KEY_TOUCH)
		{
			if (kDown & KEY_TOUCH)
			{
				hid_key_touch_press = true;
				hid_touch_pos_x_before = touch_pos.px;
				hid_touch_pos_y_before = touch_pos.py;
				hid_touch_pos_x = touch_pos.px;
				hid_touch_pos_y = touch_pos.py;
			}
			if (kHeld & KEY_TOUCH)
			{
				hid_key_touch_held = true;
				hid_touch_pos_x = touch_pos.px;
				hid_touch_pos_y = touch_pos.py;
				hid_touch_pos_x_moved = hid_touch_pos_x_before - hid_touch_pos_x;
				hid_touch_pos_y_moved = hid_touch_pos_y_before - hid_touch_pos_y;
				hid_touch_pos_x_before = touch_pos.px;
				hid_touch_pos_y_before = touch_pos.py;
			}
		}
		else
		{
			hid_touch_pos_x_moved = 0;
			hid_touch_pos_y_moved = 0;
			hid_touch_pos_x_before = 0;
			hid_touch_pos_y_before = 0;
		}

		line_init = Line_query_init_flag();
		gtr_init = Gtr_query_init_flag();
		spt_init = Spt_query_init_flag();
		imv_init = Imv_query_init_flag();
		cam_init = Cam_query_init_flag();
		mic_init = Mic_query_init_flag();
		mup_init = Mup_query_init_flag();
		line_run = Line_query_running_flag();
		gtr_run = Gtr_query_running_flag();
		spt_run = Spt_query_running_flag();
		imv_run = Imv_query_running_flag();
		cam_run = Cam_query_running_flag();
		mic_run = Mic_query_running_flag();
		mup_run = Mup_query_running_flag();
		sem_run = Sem_query_running_flag();

		if (hid_key_X_held && hid_key_Y_press)
			Sem_set_settings(SEM_SYSTEM_SETTING_MENU_SHOW, !Sem_query_settings(SEM_SYSTEM_SETTING_MENU_SHOW));

		if (Sem_query_settings(SEM_SYSTEM_SETTING_MENU_SHOW))
		{
			if (hid_key_touch_held)
			{
				if (hid_touch_pos_x > 0 && hid_touch_pos_x < 300 && hid_touch_pos_y > 0 && hid_touch_pos_y < 30)
				{
					Sem_set_settings_i(SEM_LCD_BRIGHTNESS, ((hid_touch_pos_x + 20) / 2));
					Menu_set_operation_flag(MENU_CHANGE_BRIGHTNESS_REQUEST, true);
				}
			}
			if (hid_key_touch_press)
			{
				if (hid_touch_pos_x > 300 && hid_touch_pos_x < 320 && hid_touch_pos_y > 0 && hid_touch_pos_y < 30)
						Sem_set_operation_flag(SEM_CHANGE_WIFI_STATE_REQUEST, true);
			}
		}

		if(Log_query_log_show_flag() && !hid_disabled)
		{
			if (hid_key_C_UP_held)
			{
					if ((Log_query_y() - 1) > 0)
						Log_set_y(Log_query_y() - 1);
			}
			if (hid_key_C_DOWN_held)
			{
					if ((Log_query_y() + 1) < 512)
						Log_set_y(Log_query_y() + 1);
			}
			if (hid_key_C_LEFT_held && !hid_disabled)
			{
					if ((Log_query_x() + 5.0) < 0.0)
						Log_set_x(Log_query_x() + 5.0);
					else
						Log_set_x(0.0);
			}
			if (hid_key_C_RIGHT_held && !hid_disabled)
			{
					if ((Log_query_x() - 5.0) > -1000.0)
						Log_set_x(Log_query_x() - 5.0);
					else
						Log_set_x(-1000.0);
			}
		}

		if (!hid_key_touch_press && !hid_key_touch_held)
		{
			for (int i = 0; i < 4; i++)
				button_selected[i] = false;

			for (int i = 0; i < 9; i++)
				bar_selected[i] = false;
		}

		if (Err_query_error_show_flag())
		{
			if (hid_key_touch_press && hid_key_touch_press && hid_touch_pos_x >= 150 && hid_touch_pos_x <= 170 && hid_touch_pos_y >= 150 && hid_touch_pos_y < 170)
				Err_set_error_show_flag(false);
		}
		else if ((Mup_query_operation_flag(MUP_SELECT_FILE_REQUEST) || Imv_query_operation_flag(IMV_SELECT_FILE_REQUEST) || Line_query_operation_flag(LINE_SELECT_FILE_REQUEST)) && !hid_disabled)
		{
			if (hid_key_Y_press)
			{
				if(line_run)
					Line_set_operation_flag(LINE_SELECT_FILE_REQUEST, false);
				else if(imv_run)
					Imv_set_operation_flag(IMV_SELECT_FILE_REQUEST, false);
				else if (mup_run)
					Mup_set_operation_flag(MUP_SELECT_FILE_REQUEST, false);
			}
			else if (!(Expl_query_operation_flag(EXPL_READ_DIR_REQUEST)))
			{
				for (int i = 0; i < 16; i++)
				{
					if (hid_key_A_press || (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 20 + (i * 10) && hid_touch_pos_y <= 30 + (i * 10)))
					{
						if (hid_key_A_press || i == (int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM))
						{
							if (((int)Expl_query_view_offset_y() + (int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM)) == 0 && !(Expl_query_current_patch() == "/"))
							{
								Expl_set_current_patch(Expl_query_current_patch().substr(0, Expl_query_current_patch().length() - 1));
								cut_pos[0] = Expl_query_current_patch().find_last_of("/");
								if (!(cut_pos[0] == std::string::npos))
									Expl_set_current_patch(Expl_query_current_patch().substr(0, cut_pos[0] + 1));

								Expl_set_view_offset_y(0.0);
								Expl_set_selected_num(EXPL_SELECTED_FILE_NUM, 0.0);
								Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
							}
							else if (Expl_query_type((int)Expl_query_view_offset_y() + (int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM)) == "dir")
							{
								Expl_set_current_patch(Expl_query_current_patch() + Expl_query_file_name((int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + (int)Expl_query_view_offset_y()) + "/");
								Expl_set_view_offset_y(0.0);
								Expl_set_selected_num(EXPL_SELECTED_FILE_NUM, 0.0);
								Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
							}
							else
							{
								if (line_run)
								{
									Line_set_operation_flag(LINE_SELECT_FILE_REQUEST, false);
									Line_set_operation_flag(LINE_SEND_CONTENT_CHECK_REQUEST, true);
								}
								else if (imv_run)
								{
									Imv_set_operation_flag(IMV_SELECT_FILE_REQUEST, false);
									Imv_set_load_dir_name(Expl_query_current_patch());
									Imv_set_load_file_name(Expl_query_file_name((int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + (int)Expl_query_view_offset_y()));
									Imv_set_operation_flag(IMV_IMG_LOAD_AND_PARSE_REQUEST, true);
								}
								else if (mup_run)
								{
									Mup_set_operation_flag(MUP_SELECT_FILE_REQUEST, false);
									Mup_set_load_dir_name(Expl_query_current_patch());
									Mup_set_load_file_name(Expl_query_file_name((int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + (int)Expl_query_view_offset_y()));
									Mup_set_operation_flag(MUP_PLAY_MUSIC_REQUEST, true);
									Mup_set_operation_flag(MUP_CHANGE_MUSIC_REQUEST, true);
								}
							}
							break;
						}
						else
						{
							if (Expl_query_num_of_file() >= (i + (int)Expl_query_view_offset_y()))
								Expl_set_selected_num(EXPL_SELECTED_FILE_NUM, i);
						}
					}
				}
				if (hid_key_B_press)
				{
					if (!(Expl_query_current_patch() == "/"))
					{
						Expl_set_current_patch(Expl_query_current_patch().substr(0, Expl_query_current_patch().length() - 1));
						cut_pos[0] = Expl_query_current_patch().find_last_of("/");
						if (!(cut_pos[0] == std::string::npos))
							Expl_set_current_patch(Expl_query_current_patch().substr(0, cut_pos[0] + 1));

						Expl_set_view_offset_y(0.0);
						Expl_set_selected_num(EXPL_SELECTED_FILE_NUM, 0.0);
						Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
					}
				}
				else if (hid_key_D_DOWN_press || hid_key_D_DOWN_held || hid_key_D_RIGHT_press || hid_key_D_RIGHT_held)
				{
					if ((Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + 1.0) < 16.0 && (Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + 1.0) < Expl_query_num_of_file())
					{
						if (hid_key_D_DOWN_press || hid_key_D_DOWN_held)
							Expl_set_selected_num(EXPL_SELECTED_FILE_NUM, Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + 0.125);
						else if (hid_key_D_RIGHT_press || hid_key_D_RIGHT_held)
							Expl_set_selected_num(EXPL_SELECTED_FILE_NUM, Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + 1.0);
					}
					else if ((Expl_query_view_offset_y() + Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + 1) < Expl_query_num_of_file())
					{
						if (hid_key_D_DOWN_press || hid_key_D_DOWN_held)
							Expl_set_view_offset_y(Expl_query_view_offset_y() + 0.125);
						else if (hid_key_D_RIGHT_press || hid_key_D_RIGHT_held)
							Expl_set_view_offset_y(Expl_query_view_offset_y() + 1.0);
					}
				}
				else if (hid_key_D_UP_press || hid_key_D_UP_held || hid_key_D_LEFT_press || hid_key_D_LEFT_held)
				{
					if ((Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) - 1.0) > -1.0)
					{
						if (hid_key_D_UP_press || hid_key_D_UP_held)
							Expl_set_selected_num(EXPL_SELECTED_FILE_NUM, (Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) - 0.125));
						else if (hid_key_D_LEFT_press || hid_key_D_LEFT_held)
							Expl_set_selected_num(EXPL_SELECTED_FILE_NUM, (Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) - 1.0));
					}
					else if ((Expl_query_view_offset_y() - 1) > -1)
					{
						if (hid_key_D_UP_press || hid_key_D_UP_held)
							Expl_set_view_offset_y(Expl_query_view_offset_y() - 0.125);
						else if (hid_key_D_LEFT_press || hid_key_D_LEFT_held)
							Expl_set_view_offset_y(Expl_query_view_offset_y() - 1.0);
					}
				}
			}
		}
		else if (Menu_query_running_flag() && !hid_disabled)
		{
			if (hid_key_SELECT_press)
				Log_set_log_show_flag(!Log_query_log_show_flag());
			else if (hid_key_START_press || (hid_key_touch_press && hid_touch_pos_x >= 0 && hid_touch_pos_x <= 320 && hid_touch_pos_y >= 220 && hid_touch_pos_y <= 240))
				Menu_set_operation_flag(MENU_CHECK_EXIT_REQUEST, true);
			else if (hid_key_touch_press)
			{
				if (hid_touch_pos_x >= 45 && hid_touch_pos_x <= 59 && hid_touch_pos_y >= 0 && hid_touch_pos_y <= 14 && line_init)
					Menu_set_operation_flag(MENU_DESTROY_LINE_REQUEST, true);
				else if (hid_touch_pos_x >= 0 && hid_touch_pos_x <= 59 && hid_touch_pos_y >= 0 && hid_touch_pos_y <= 59)
					Menu_set_operation_flag(MENU_JUMP_TO_LINE_REQUEST, true);
				else if (hid_touch_pos_x >= 125 && hid_touch_pos_x <= 139 && hid_touch_pos_y >= 0 && hid_touch_pos_y <= 14 && gtr_init)
					Menu_set_operation_flag(MENU_DESTROY_GTR_REQUEST, true);
				else if (hid_touch_pos_x >= 80 && hid_touch_pos_x <= 139 && hid_touch_pos_y >= 0 && hid_touch_pos_y <= 59)
					Menu_set_operation_flag(MENU_JUMP_TO_GTR_REQUEST, true);
				else if (hid_touch_pos_x >= 205 && hid_touch_pos_x <= 219 && hid_touch_pos_y >= 0 && hid_touch_pos_y <= 14 && spt_init)
					Menu_set_operation_flag(MENU_DESTROY_SPT_REQUEST, true);
				else if (hid_touch_pos_x >= 160 && hid_touch_pos_x <= 219 && hid_touch_pos_y >= 0 && hid_touch_pos_y <= 59)
					Menu_set_operation_flag(MENU_JUMP_TO_SPT_REQUEST, true);
				else if (hid_touch_pos_x >= 285 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 0 && hid_touch_pos_y <= 14 && imv_init)
					Menu_set_operation_flag(MENU_DESTROY_IMV_REQUEST, true);
				else if (hid_touch_pos_x >= 240 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 0 && hid_touch_pos_y <= 59)
					Menu_set_operation_flag(MENU_JUMP_TO_IMV_REQUEST, true);
				else if (hid_touch_pos_x >= 260 && hid_touch_pos_x <= 319 && hid_touch_pos_y >= 180 && hid_touch_pos_y <= 239)
					Menu_set_operation_flag(MENU_JUMP_TO_SEM_REQUEST, true);
				else if (hid_touch_pos_x >= 45 && hid_touch_pos_x <= 59 && hid_touch_pos_y >= 80 && hid_touch_pos_y <= 94 && cam_init)
					Menu_set_operation_flag(MENU_DESTROY_CAM_REQUEST, true);
				else if (hid_touch_pos_x >= 0 && hid_touch_pos_x <= 59 && hid_touch_pos_y >= 80 && hid_touch_pos_y <= 139)
					Menu_set_operation_flag(MENU_JUMP_TO_CAM_REQUEST, true);
				else if (hid_touch_pos_x >= 125 && hid_touch_pos_x <= 139 && hid_touch_pos_y >= 80 && hid_touch_pos_y <= 94 && mic_init)
					Menu_set_operation_flag(MENU_DESTROY_MIC_REQUEST, true);
				else if (hid_touch_pos_x >= 80 && hid_touch_pos_x <= 139 && hid_touch_pos_y >= 80 && hid_touch_pos_y <= 139)
					Menu_set_operation_flag(MENU_JUMP_TO_MIC_REQUEST, true);
				else if (hid_touch_pos_x >= 205 && hid_touch_pos_x <= 219 && hid_touch_pos_y >= 80 && hid_touch_pos_y <= 94 && mup_init)
					Menu_set_operation_flag(MENU_DESTROY_MUP_REQUEST, true);
				else if (hid_touch_pos_x >= 160 && hid_touch_pos_x <= 219 && hid_touch_pos_y >= 80 && hid_touch_pos_y <= 139)
					Menu_set_operation_flag(MENU_JUMP_TO_MUP_REQUEST, true);
			}
		}
		else if (sem_run && !hid_disabled)
		{
			sem_y = Sem_query_y_offset();
			if (hid_key_START_press || (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 230 && hid_touch_pos_y >= 220 && hid_touch_pos_y <= 240))
				Sem_suspend();

			if (Sem_query_operation_flag(SEM_SHOW_PATCH_NOTE_REQUEST))
			{
				if (hid_key_B_press || (hid_key_touch_press && hid_touch_pos_x >= 160 && hid_touch_pos_x <= 304 && hid_touch_pos_y >= 200 && hid_touch_pos_y < 215))
					Sem_set_operation_flag(SEM_SHOW_PATCH_NOTE_REQUEST, false);
				if (hid_key_A_press || (hid_key_touch_press && hid_touch_pos_x >= 15 && hid_touch_pos_x <= 159 && hid_touch_pos_y >= 200 && hid_touch_pos_y < 215))
				{
					Sem_set_operation_flag(SEM_SHOW_PATCH_NOTE_REQUEST, false);
					Sem_set_operation_flag(SEM_SELECT_VER_REQUEST, true);
				}
			}
			else if (Sem_query_operation_flag(SEM_SELECT_VER_REQUEST))
			{
				if (!Sem_query_operation_flag(SEM_DL_FILE_REQUEST))
				{
					if (hid_key_B_press || (hid_key_touch_press && hid_touch_pos_x >= 15 && hid_touch_pos_x <= 159 && hid_touch_pos_y >= 200 && hid_touch_pos_y < 215))
					{
						Sem_set_operation_flag(SEM_SHOW_PATCH_NOTE_REQUEST, true);
						Sem_set_operation_flag(SEM_SELECT_VER_REQUEST, false);
					}
					else if ((hid_key_X_press || (hid_key_touch_press && hid_touch_pos_x >= 160 && hid_touch_pos_x <= 304 && hid_touch_pos_y >= 200 && hid_touch_pos_y < 215)) && Sem_query_available_edtion(Sem_query_selected_num(SEM_SELECTED_EDITION_NUM)))
						Sem_set_operation_flag(SEM_DL_FILE_REQUEST, true);

					for (int i = 0; i < 8; i++)
					{
						if (hid_key_touch_press && hid_touch_pos_x >= 17 && hid_touch_pos_x <= 250 && hid_touch_pos_y >= 15 + (i * 10) && hid_touch_pos_y <= 24 + (i * 10))
						{
							Sem_set_selected_num(SEM_SELECTED_EDITION_NUM, i);
							break;
						}
					}
				}
			}
			else
			{
				if (hid_key_touch_press || hid_key_touch_held)
				{
					hid_touch_pos_x_move_left = 0;
					hid_touch_pos_y_move_left = 0;
				}
				else
				{
					scroll_mode = false;
					do_not_scroll_mode = false;
					scroll_bar_selected = false;
					hid_touch_pos_x_move_left -= (hid_touch_pos_x_move_left * 0.025);
					hid_touch_pos_y_move_left -= (hid_touch_pos_y_move_left * 0.025);
					if (hid_touch_pos_x_move_left < 0.5 && hid_touch_pos_x_move_left > -0.5)
						hid_touch_pos_x_move_left = 0;
					if (hid_touch_pos_y_move_left < 0.5 && hid_touch_pos_y_move_left > -0.5)
						hid_touch_pos_y_move_left = 0;
				}

				if (Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) >= 1 && Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) <= 8)
				{
					if (hid_key_touch_press && hid_touch_pos_x >= 0 && hid_touch_pos_x <= 40 && hid_touch_pos_y >= 0 + sem_y && hid_touch_pos_y <= 24 + sem_y)
					{
						Sem_set_y_offset(0.0);
						Sem_set_y_max(0.0);
						Sem_set_selected_num(SEM_SELECTED_MENU_MODE_NUM, 0);
						continue;
					}
				}

				if((Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) == 5 && Sem_query_font_flag(SEM_USE_EXTERNAL_FONT)) || Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) == 6)//Scroll bar
				{
					if (hid_key_C_DOWN_held || hid_key_C_UP_held)
						Sem_set_y_offset(sem_y + ((double)circle_pos.dy * scroll_speed * 0.0625));

					if (hid_key_touch_held && scroll_bar_selected)
						Sem_set_y_offset(Sem_query_y_max() * ((hid_touch_pos_y - 15.0) / 195.0));
					else if (hid_key_touch_held && scroll_mode && !do_not_scroll_mode)
					{
						hid_touch_pos_x_move_left += hid_touch_pos_x_moved;
						hid_touch_pos_y_move_left += hid_touch_pos_y_moved;
					}

					if (hid_key_touch_press && hid_touch_pos_x >= 305 && hid_touch_pos_x <= 320 && hid_touch_pos_y >= 15)
						scroll_bar_selected = true;

					sem_y = Sem_query_y_offset();
					Sem_set_y_offset((sem_y - (hid_touch_pos_y_move_left * scroll_speed)));
					sem_y = Sem_query_y_offset();
				}

				if (Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) == 0)
				{
					for (int i = 0; i < 8; i++)
					{
						if (hid_key_touch_press && hid_touch_pos_x >= 0 && hid_touch_pos_x <= 240 && hid_touch_pos_y >= 0 + (i * 25) && hid_touch_pos_y <= 19 + (i * 25))
						{
							Sem_set_y_offset(0.0);
							Sem_set_selected_num(SEM_SELECTED_MENU_MODE_NUM, i + 1);
							if(i + 1 == 6)
								Sem_set_y_max(-200.0);
							else if (i + 1 == 5 && Sem_query_font_flag(SEM_USE_EXTERNAL_FONT))
								Sem_set_y_max(-850.0);

							break;
						}
					}
				}
				else if (Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) == 1)//Check for updates
				{
					if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 239 && hid_touch_pos_y >= 25 && hid_touch_pos_y <= 44)
					{
						Sem_set_operation_flag(SEM_CHECK_UPDATE_REQUEST, true);
						Sem_set_operation_flag(SEM_SHOW_PATCH_NOTE_REQUEST, true);
					}
				}
				else if (Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) == 2 && !Sem_query_operation_flag(SEM_RELOAD_MSG_REQUEST))//Language
				{
					if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 249 && hid_touch_pos_y >= 25 && hid_touch_pos_y <= 44)
					{
						Sem_set_lang("en");
						Sem_set_operation_flag(SEM_RELOAD_MSG_REQUEST, true);
					}
					else if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 249 && hid_touch_pos_y >= 50 && hid_touch_pos_y <= 69)
					{
						Sem_set_lang("jp");
						Sem_set_operation_flag(SEM_RELOAD_MSG_REQUEST, true);
					}
				}
				else if (Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) == 3)//LCD
				{
					if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 99 && hid_touch_pos_y >= 40 && hid_touch_pos_y <= 59)
					{
						C2D_PlainImageTint(&texture_tint, C2D_Color32f(1.0, 1.0, 1.0, 0.75), true);
						Sem_set_settings(SEM_NIGHT_MODE, true);
					}
					else if (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 199 && hid_touch_pos_y >= 40 && hid_touch_pos_y <= 59)
					{
						C2D_PlainImageTint(&texture_tint, C2D_Color32f(0.0, 0.0, 0.0, 1.0), true);
						Sem_set_settings(SEM_NIGHT_MODE, false);
					}
					else if (hid_key_touch_press && hid_touch_pos_x >= 210 && hid_touch_pos_x <= 249 && hid_touch_pos_y >= 40 && hid_touch_pos_y <= 59)
						Sem_set_settings(SEM_FLASH_MODE, !Sem_query_settings(SEM_FLASH_MODE));
					else if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 99 && hid_touch_pos_y >= 80 && hid_touch_pos_y <= 99)
						Sem_set_settings(SEM_VSYNC_MODE, true);
					else if (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 199 && hid_touch_pos_y >= 80 && hid_touch_pos_y <= 99)
						Sem_set_settings(SEM_VSYNC_MODE, false);
					else if (hid_key_touch_held && bar_selected[0] && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 309)
					{
						Sem_set_settings_i(SEM_LCD_BRIGHTNESS, ((hid_touch_pos_x / 2) + 10));
						Menu_set_operation_flag(MENU_CHANGE_BRIGHTNESS_REQUEST, true);
					}
					else if (hid_key_touch_held && bar_selected[1] && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 309)
						Sem_set_settings_i(SEM_TIME_TO_TURN_OFF_LCD, (hid_touch_pos_x * 10));
					else if (hid_key_touch_held && bar_selected[2] && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 309)
						Sem_set_settings_i(SEM_LCD_BRIGHTNESS_BEFORE_TURN_OFF, ((hid_touch_pos_x / 2) + 10));
					else if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 120 && hid_touch_pos_y <= 139)
						bar_selected[0] = true;
					else if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 160 && hid_touch_pos_y <= 179)
						bar_selected[1] = true;
					else if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 200 && hid_touch_pos_y <= 219)
						bar_selected[2] = true;
				}
				else if (Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) == 4)//Scroll speed
				{
					if (hid_key_touch_held && bar_selected[3] && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 309)
						Sem_set_settings_d(SEM_SCROLL_SPEED, ((double)hid_touch_pos_x / 300));
					else if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 35 && hid_touch_pos_y <= 54)
						bar_selected[3] = true;
				}
				else if (Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) == 5)//Font
				{
					scroll_mode = true;
					if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 40 + sem_y && hid_touch_pos_y <= 59 + sem_y)
					{
						do_not_scroll_mode = true;
						Sem_set_font_flag(SEM_USE_DEFAULT_FONT, false);
						Sem_set_font_flag(SEM_USE_SYSTEM_SPEIFIC_FONT, false);
						Sem_set_font_flag(SEM_USE_EXTERNAL_FONT, false);
						if (hid_touch_pos_x >= 10 && hid_touch_pos_x <= 99)
						{
							Sem_set_font_flag(SEM_USE_DEFAULT_FONT, true);
							Sem_set_y_max(0.0);
						}
						else if (hid_touch_pos_x >= 110 && hid_touch_pos_x <= 199)
						{
							Sem_set_font_flag(SEM_USE_EXTERNAL_FONT, true);
							Sem_set_y_max(-850.0);
						}
						else if (hid_touch_pos_x >= 210 && hid_touch_pos_x <= 299)
						{
							Sem_set_font_flag(SEM_USE_SYSTEM_SPEIFIC_FONT, true);
							Sem_set_y_max(0.0);
						}
					}

					if (Sem_query_font_flag(SEM_USE_SYSTEM_SPEIFIC_FONT))
					{
						if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 304 && hid_touch_pos_y >= 80 + sem_y && hid_touch_pos_y <= 99 + sem_y && !(Sem_query_operation_flag(SEM_LOAD_SYSTEM_FONT_REQUEST)))
						{
							for (int i = 0; i < 4; i++)
							{
								if (hid_touch_pos_x >= 10 + (i * 75) && hid_touch_pos_x <= 79 + (i * 75))
								{
									Sem_set_selected_num(SEM_SELECTED_LANG_NUM, i);
									break;
								}
							}
							Sem_set_operation_flag(SEM_LOAD_SYSTEM_FONT_REQUEST, true);
						}
					}
					else if (Sem_query_font_flag(SEM_USE_EXTERNAL_FONT))
					{
						if (hid_key_touch_press && hid_touch_pos_y <= 219 && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 209 && hid_touch_pos_y >= 100 + sem_y && hid_touch_pos_y <= 1019 + sem_y && !Sem_query_operation_flag(SEM_LOAD_EXTERNAL_FONT_REQUEST) && !Sem_query_operation_flag(SEM_UNLOAD_EXTERNAL_FONT_REQUEST))
						{
							do_not_scroll_mode = true;
							load_font_num = -1;
							for (int i = 0; i < 46; i++)
							{
								if (hid_touch_pos_y >= 100 + sem_y + (i * 20) && hid_touch_pos_y <= 119 + sem_y + (i * 20))
									load_font_num = i;
							}

							if (load_font_num != -1)
							{
								if (Sem_query_loaded_external_font_flag(load_font_num))
								{
									Sem_set_load_external_font_request(load_font_num, false);
									Sem_set_operation_flag(SEM_UNLOAD_EXTERNAL_FONT_REQUEST, true);
								}
								else
								{
									Sem_set_load_external_font_request(load_font_num, true);
									Sem_set_operation_flag(SEM_LOAD_EXTERNAL_FONT_REQUEST, true);
								}
							}
						}
						else if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 109 && hid_touch_pos_y >= 80 + sem_y && hid_touch_pos_y <= 99 + sem_y && !Sem_query_operation_flag(SEM_LOAD_EXTERNAL_FONT_REQUEST) && !Sem_query_operation_flag(SEM_UNLOAD_EXTERNAL_FONT_REQUEST))
						{
							do_not_scroll_mode = true;
								for (int i = 0; i < 46; i++)
									Sem_set_load_external_font_request(i, true);

								Sem_set_operation_flag(SEM_LOAD_EXTERNAL_FONT_REQUEST, true);
						}
						else if (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 209 && hid_touch_pos_y >= 80 + sem_y && hid_touch_pos_y <= 99 + sem_y && !Sem_query_operation_flag(SEM_LOAD_EXTERNAL_FONT_REQUEST) && !Sem_query_operation_flag(SEM_UNLOAD_EXTERNAL_FONT_REQUEST))
						{
							do_not_scroll_mode = true;
							for (int i = 0; i < 46; i++)
								Sem_set_load_external_font_request(i, false);

							Sem_set_operation_flag(SEM_UNLOAD_EXTERNAL_FONT_REQUEST, true);
						}
					}
				}
				else if (Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) == 6)//Memory
				{
					scroll_mode = true;
					for (int i = 0; i < 9; i++)
					{
						if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 40 + (i * 40) + sem_y && hid_touch_pos_y <= 59 + (i * 40) + sem_y)
						{
							bar_selected[i] = true;
							do_not_scroll_mode = true;
							break;
						}
					}

					for (int i = 0; i < 40; i++)
					{
						if (!line_init && hid_key_touch_held && bar_selected[0] && hid_touch_pos_x >= 10 + (i * 7.5) && hid_touch_pos_x <= 17 + (i * 7.5))
							Line_set_buffer_size(LINE_HTTPC_BUFFER, (i + 1) * 0x40000);
					}
					for (int i = 0; i < 20; i++)
					{
						if (!line_init && hid_key_touch_held && bar_selected[1] && hid_touch_pos_x >= 10 + (i * 15) && hid_touch_pos_x <= 24 + (i * 15))
							Line_set_buffer_size(LINE_FS_BUFFER, (i + 1) * 0x40000);
					}
					for (int i = 0; i < 19; i++)
					{
						if (!line_init && hid_key_touch_held && bar_selected[2] && hid_touch_pos_x >= 10 + (i * 15.7) && hid_touch_pos_x <= 25 + (i * 15.7))
							Line_set_buffer_size(LINE_SEND_FS_CACHE_BUFFER, (i + 1) * 0x40000);
					}
					for (int i = 0; i < 80; i++)
					{
						if (!line_init && hid_key_touch_held && bar_selected[3] && hid_touch_pos_x >= 10 + (i * 3.75) && hid_touch_pos_x <= 13 + (i * 3.75))
							Line_set_buffer_size(LINE_SEND_FS_BUFFER, (i + 1) * 0x40000);
					}
					for (int i = 0; i < 28; i++)
					{
						if (!spt_init && hid_key_touch_held && bar_selected[4] && hid_touch_pos_x >= 10 + (i * 10.7) && hid_touch_pos_x <= 20 + (i * 10.7))
							Spt_set_buffer_size(SPT_HTTPC_BUFFER, (i + 1) * 0x40000);
					}
					for (int i = 0; i < 20; i++)
					{
						if (!imv_init && hid_key_touch_held && bar_selected[5] && hid_touch_pos_x >= 10 + (i * 15) && hid_touch_pos_x <= 24 + (i * 15))
							Imv_set_buffer_size(IMV_HTTPC_BUFFER, (i + 1) * 0x40000);
					}
					for (int i = 0; i < 20; i++)
					{
						if (!imv_init && hid_key_touch_held && bar_selected[6] && hid_touch_pos_x >= 10 + (i * 15) && hid_touch_pos_x <= 24 + (i * 15))
							Imv_set_buffer_size(IMV_FS_BUFFER, (i + 1) * 0x40000);
					}
					for (int i = 0; i < 40; i++)
					{
						if (!mup_init && hid_key_touch_held && bar_selected[7] && hid_touch_pos_x >= 10 + (i * 7.5) && hid_touch_pos_x <= 17 + (i * 7.5))
							Mup_set_buffer_size(MUP_FS_OUT_BUFFER, (i + 1) * 0x40000);
					}
					for (int i = 0; i < 8; i++)
					{
						if (!mup_init && hid_key_touch_held && bar_selected[8] && hid_touch_pos_x >= 10 + (i * 37.5) && hid_touch_pos_x <= 47 + (i * 37.5))
							Mup_set_buffer_size(MUP_FS_IN_BUFFER, (i + 1) * 0x40000);
					}
				}
				else if (Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) == 7)//Advanced settings
				{
					if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 99 && hid_touch_pos_y >= 40 && hid_touch_pos_y <= 59)
						Sem_set_settings(SEM_ALLOW_SEND_APP_INFO, true);
					else if (hid_key_touch_press && hid_touch_pos_x >= 100 && hid_touch_pos_x <= 199 && hid_touch_pos_y >= 40 && hid_touch_pos_y <= 59)
						Sem_set_settings(SEM_ALLOW_SEND_APP_INFO, false);
					else if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 99 && hid_touch_pos_y >= 80 && hid_touch_pos_y <= 99)
						Sem_set_settings(SEM_DEBUG_MODE, true);
					else if (hid_key_touch_press && hid_touch_pos_x >= 100 && hid_touch_pos_x <= 199 && hid_touch_pos_y >= 80 && hid_touch_pos_y <= 99)
						Sem_set_settings(SEM_DEBUG_MODE, false);
					else if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 199 && hid_touch_pos_y >= 120 && hid_touch_pos_y <= 139)
						Sem_set_operation_flag(SEM_DELETE_LINE_IMG_CACHE_REQUEST, true);
				}
				else if (Sem_query_selected_num(SEM_SELECTED_MENU_MODE_NUM) == 8)//Battery
				{
					if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 99 && hid_touch_pos_y >= 40 && hid_touch_pos_y <= 59)
						Sem_set_settings(SEM_ECO_MODE, true);
					else if (hid_key_touch_press && hid_touch_pos_x >= 100 && hid_touch_pos_x <= 199 && hid_touch_pos_y >= 40 && hid_touch_pos_y <= 59)
						Sem_set_settings(SEM_ECO_MODE, false);
				}

				if (sem_y >= 0)
					Sem_set_y_offset(0);
				else if (sem_y <= Sem_query_y_max())
					Sem_set_y_offset(Sem_query_y_max());
			}
		}
		else if (line_run && !hid_disabled)
		{
			line_x = Line_query_x_y_size_interval(LINE_TEXT_X);
			line_y = Line_query_x_y_size_interval(LINE_TEXT_Y);
			line_interval = Line_query_x_y_size_interval(LINE_TEXT_INTERVAL);
			line_size = Line_query_x_y_size_interval(LINE_TEXT_SIZE);

			if (hid_key_START_press || (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 230 && hid_touch_pos_y >= 220 && hid_touch_pos_y <= 240))
				Line_suspend();

			if (hid_key_touch_press || hid_key_touch_held)
			{
				hid_touch_pos_x_move_left = 0;
				hid_touch_pos_y_move_left = 0;
			}
			else
			{
				scroll_mode = false;
				scroll_bar_selected = false;
				hid_touch_pos_x_move_left -= (hid_touch_pos_x_move_left * 0.025);
				hid_touch_pos_y_move_left -= (hid_touch_pos_y_move_left * 0.025);
				if (hid_touch_pos_x_move_left < 0.5 && hid_touch_pos_x_move_left > -0.5)
					hid_touch_pos_x_move_left = 0;
				if (hid_touch_pos_y_move_left < 0.5 && hid_touch_pos_y_move_left > -0.5)
					hid_touch_pos_y_move_left = 0;
			}

			if (Line_query_operation_flag(LINE_SEND_MSG_CHECK_REQUEST) || Line_query_operation_flag(LINE_SEND_STICKER_CHECK_REQUEST) || Line_query_operation_flag(LINE_SEND_CONTENT_CHECK_REQUEST) || Line_query_operation_flag(LINE_DELETE_ID_CHECK_REQUEST))
			{
				if (hid_key_touch_press && hid_touch_pos_x >= 305 && hid_touch_pos_x <= 320 && hid_touch_pos_y >= 15 && hid_touch_pos_y < 220)
					scroll_bar_selected = true;
				else if (hid_key_touch_press && hid_touch_pos_y <= 109)
					scroll_mode = true;
				else if (hid_key_A_press || (hid_key_touch_press && hid_touch_pos_x >= 30 && hid_touch_pos_x <= 99 && hid_touch_pos_y >= 200 && hid_touch_pos_y <= 214))
				{
					if (Line_query_operation_flag(LINE_SEND_MSG_CHECK_REQUEST))
					{
						Line_set_operation_flag(LINE_SEND_MSG_REQUEST, true);
						Line_set_operation_flag(LINE_SEND_MSG_CHECK_REQUEST, false);
					}
					else if (Line_query_operation_flag(LINE_SEND_STICKER_CHECK_REQUEST))
					{
						Line_set_operation_flag(LINE_SEND_STICKER_REQUEST, true);
						Line_set_operation_flag(LINE_SEND_STICKER_CHECK_REQUEST, false);
					}
					else if (Line_query_operation_flag(LINE_SEND_CONTENT_CHECK_REQUEST))
					{
						Line_set_send_dir_name(Expl_query_current_patch());
						Line_set_send_file_name(Expl_query_file_name((int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + (int)Expl_query_view_offset_y()));
						Line_set_operation_flag(LINE_SEND_CONTENT_REQUEST, true);
						Line_set_operation_flag(LINE_SEND_CONTENT_CHECK_REQUEST, false);
					}
					else if (Line_query_operation_flag(LINE_DELETE_ID_CHECK_REQUEST) && !Line_query_operation_flag(LINE_DL_LOG_REQUEST) && !Line_query_operation_flag(LINE_PARSE_LOG_REQUEST) && !Line_query_setting(LINE_AUTO_UPDATE) && !Line_query_setting(LINE_LOAD_LOG_REQUEST))
					{
						saved_y[1] = line_y;
						Line_set_x_y_size_interval(LINE_TEXT_Y, saved_y[0]);
						Line_set_operation_flag(LINE_DELETE_ID_REQUEST, true);
						Line_set_operation_flag(LINE_DELETE_ID_CHECK_REQUEST, false);
						Line_reset_msg();
					}
				}
				else if (hid_key_B_press || (hid_key_touch_press && hid_touch_pos_x >= 120 && hid_touch_pos_x <= 189 && hid_touch_pos_y >= 200 && hid_touch_pos_y <= 214))
				{
					if (Line_query_operation_flag(LINE_SEND_MSG_CHECK_REQUEST))
						Line_set_operation_flag(LINE_SEND_MSG_CHECK_REQUEST, false);
					else if (Line_query_operation_flag(LINE_SEND_STICKER_CHECK_REQUEST))
						Line_set_operation_flag(LINE_SEND_STICKER_CHECK_REQUEST, false);
					else if (Line_query_operation_flag(LINE_SEND_CONTENT_CHECK_REQUEST))
						Line_set_operation_flag(LINE_SEND_CONTENT_CHECK_REQUEST, false);
					else if(Line_query_operation_flag(LINE_DELETE_ID_CHECK_REQUEST))
						Line_set_operation_flag(LINE_DELETE_ID_CHECK_REQUEST, false);
				}
				else if (hid_key_X_press || (hid_key_touch_press && hid_touch_pos_x >= 210 && hid_touch_pos_x <= 279 && hid_touch_pos_y >= 200 && hid_touch_pos_y <= 214))
				{
					if (Line_query_operation_flag(LINE_SEND_CONTENT_CHECK_REQUEST))
					{
						Imv_set_load_dir_name(Expl_query_current_patch());
						Imv_set_load_file_name(Expl_query_file_name((int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + (int)Expl_query_view_offset_y()));
						Imv_set_operation_flag(IMV_IMG_LOAD_AND_PARSE_REQUEST, true);
						Line_suspend();
						Menu_set_operation_flag(MENU_JUMP_TO_IMV_REQUEST, true);
					}
					else if (Line_query_operation_flag(LINE_SEND_MSG_CHECK_REQUEST))
					{
						Line_set_operation_flag(LINE_TYPE_MSG_REQUEST, true);
						Line_set_operation_flag(LINE_SEND_MSG_CHECK_REQUEST, false);
					}
				}
			}
			else if (Line_query_operation_flag(LINE_SELECT_STICKER_REQUEST))
			{
				if (hid_key_B_press || (hid_key_touch_press && hid_touch_pos_x >= 290 && hid_touch_pos_x <= 309 && hid_touch_pos_y > 120 && hid_touch_pos_y < 139))
					Line_set_operation_flag(LINE_SELECT_STICKER_REQUEST, false);

				for (int i = 0; i < 10; i++)
				{
					if (hid_key_touch_press && hid_touch_pos_x >= 10 + (i * 30) && hid_touch_pos_x <= 39 + (i * 30) && hid_touch_pos_y > 140 && hid_touch_pos_y < 149)
						Line_set_selected_num(LINE_SELECTED_STICKER_TAB_NUM, i);
				}

				for (int i = 0; i < 7; i++)
				{
					if (hid_key_touch_press && hid_touch_pos_x >= 20 + (i * 50) && hid_touch_pos_x <= 49 + (i * 50) && hid_touch_pos_y > 150 && hid_touch_pos_y < 179)
					{
						Line_set_selected_num(LINE_SELECTED_STICKER_NUM, (Line_query_selected_num(LINE_SELECTED_STICKER_TAB_NUM) * 12) + i + 1);
						Line_set_operation_flag(LINE_SEND_STICKER_CHECK_REQUEST, true);
						Line_set_operation_flag(LINE_SELECT_STICKER_REQUEST, false);
					}
					else if (hid_key_touch_press && hid_touch_pos_x >= 20 + (i * 50) && hid_touch_pos_x <= 49 + (i * 50) && hid_touch_pos_y > 190 && hid_touch_pos_y < 219)
					{

						Line_set_selected_num(LINE_SELECTED_STICKER_NUM, (Line_query_selected_num(LINE_SELECTED_STICKER_TAB_NUM) * 12) + i + 7);
						Line_set_operation_flag(LINE_SEND_STICKER_CHECK_REQUEST, true);
						Line_set_operation_flag(LINE_SELECT_STICKER_REQUEST, false);
					}
				}
			}
			else if (Line_query_operation_flag(LINE_SELECT_CHAT_ROOM_REQUEST))
			{
				if (!Line_query_operation_flag(LINE_SOLVE_SHORT_URL_REQUEST))
				{
					if(!Line_query_operation_flag(LINE_DL_LOG_NO_PARSE_REQUEST) && !Line_query_operation_flag(LINE_DL_ALL_LOG_NO_PARSE_REQUEST))
					{
						for (int i = 0; i < 128; i++)
						{
							if(hid_touch_pos_y <= 164 && hid_touch_pos_y >= line_y + (i * 35) && hid_touch_pos_y <= 29 + line_y + (i * 35))
							{
								if (hid_key_touch_press && hid_touch_pos_x >= 0 && hid_touch_pos_x <= 229)
								{
									Line_set_selected_num(LINE_SELECTED_ROOM_NUM, i);
									Line_set_operation_flag(LINE_SELECT_CHAT_ROOM_REQUEST, false);
									saved_y[0] = line_y;
									Line_set_x_y_size_interval(LINE_TEXT_Y, saved_y[1]);
									Line_set_operation_flag(LINE_LOAD_LOG_REQUEST, true);
									break;
								}
								else if (hid_key_touch_press && hid_touch_pos_x >= 230 && hid_touch_pos_x <= 269)
								{
									Line_set_selected_num(LINE_SELECTED_ROOM_NUM, i);
									Line_set_operation_flag(LINE_DL_LOG_NO_PARSE_REQUEST, true);
									break;
								}
							}
						}
					}

					if (hid_key_touch_press && hid_touch_pos_x >= 305 && hid_touch_pos_x <= 320 && hid_touch_pos_y >= 15 && hid_touch_pos_y < 220)
						scroll_bar_selected = true;
					else if (hid_key_touch_press && hid_touch_pos_y <= 169)
						scroll_mode = true;
					else if(!Line_query_operation_flag(LINE_DL_LOG_NO_PARSE_REQUEST) && !Line_query_operation_flag(LINE_DL_ALL_LOG_NO_PARSE_REQUEST))
					{
						//if (hid_key_ZL_press)
						//	Line_set_operation_flag(LINE_DL_ALL_LOG_NO_PARSE_REQUEST, true);
						if (hid_key_Y_press || (hid_key_touch_press && hid_touch_pos_x >= 20 && hid_touch_pos_x <= 149 && hid_touch_pos_y >= 190 && hid_touch_pos_y <= 202))
							Line_set_operation_flag(LINE_TYPE_ID_REQUEST, true);
						else if (((hid_key_L_press && hid_key_R_held) || (hid_key_L_held && hid_key_R_press)) || (hid_key_touch_press && hid_touch_pos_x >= 20 && hid_touch_pos_x <= 149 && hid_touch_pos_y >= 170 && hid_touch_pos_y <= 182))
							Line_set_operation_flag(LINE_TYPE_SHORT_URL_REQUEST, true);
						else if (hid_key_X_press || (hid_key_touch_press && hid_touch_pos_x >= 20 && hid_touch_pos_x <= 149 && hid_touch_pos_y >= 210 && hid_touch_pos_y <= 222))
							Line_set_operation_flag(LINE_TYPE_MAIN_URL_REQUEST, true);
						else if (hid_key_touch_press && hid_touch_pos_x >= 170 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 170 && hid_touch_pos_y <= 182)
							Line_set_setting(LINE_HIDE_ID, !(Line_query_setting(LINE_HIDE_ID)));
						else if (hid_key_A_press || (hid_key_touch_press && hid_touch_pos_x >= 170 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 190 && hid_touch_pos_y <= 202))
							Line_set_operation_flag(LINE_TYPE_APP_PS_REQUEST, true);
						else if (hid_key_B_press || (hid_key_touch_press && hid_touch_pos_x >= 170 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 210 && hid_touch_pos_y <= 222))
							Line_set_operation_flag(LINE_TYPE_SCRIPT_PS_REQUEST, true);
					}
				}
			}
			else
			{
				if (hid_key_touch_press && hid_touch_pos_y >= 0 && hid_touch_pos_y < 140)
				{
					for (int i = 1; i <= 59999; i++)
					{
						if ((line_y + line_interval * i) - 240 >= 125)
							break;
						else if (line_y + line_interval * i <= -1000)
						{
							if ((line_y + line_interval * (i + 100)) <= 10)
								i += 100;
						}
						else if ((line_y + line_interval * i) - 240 <= -60)
						{
						}
						else
						{
							content = Line_query_content_info(i);
							type_pos[0] = content.find("<type>image</type>");
							type_pos[1] = content.find("<type>id</type>");

							//jump to imv
							if (!(type_pos[0] == std::string::npos) && hid_touch_pos_y >= (line_y + line_interval * i) - 240.0
								&& hid_touch_pos_y <= (line_y + line_interval * i) - 220.0
								&& hid_touch_pos_x >= (line_x - 40.0) && hid_touch_pos_x <= (line_x + 460.0))
							{
								result.code = -1;
								cut_pos[0] = content.find("&id=");
								cut_pos[1] = content.find("om/d/");
								if (!(cut_pos[0] == std::string::npos && cut_pos[1] == std::string::npos))
								{
									cache_string = "";
									if (!(cut_pos[0] == std::string::npos))
										cache_string = content.substr(cut_pos[0] + 4);
									else if (!(cut_pos[1] == std::string::npos))
										cache_string = content.substr(cut_pos[1] + 5);

									if (cache_string.length() > 33)
										cache_string = cache_string.substr(0, 33);

									cache_string += ".jpg";
									log_num = Log_log_save(hid_scan_hid_thread_string, "File_check_file_exist()...", 1234567890, false);
									result = File_check_file_exist(cache_string, "/Line/images/", fs_handle, fs_archive);
									Log_log_add(log_num, result.string, result.code, false);
								}

								if (result.code == 0)
								{
									Imv_set_load_dir_name("/Line/images/");
									Imv_set_load_file_name(cache_string);
									Imv_set_operation_flag(IMV_IMG_LOAD_AND_PARSE_REQUEST, true);
								}
								else
								{
									cut_pos[0] = content.find("<url>");
									cut_pos[1] = content.find("</url>");
									if (cut_pos[0] == std::string::npos || cut_pos[1] == std::string::npos)
										Imv_set_url("");
									else
										Imv_set_url(content.substr((cut_pos[0] + 5), cut_pos[1] - (cut_pos[0] + 5)));

									Imv_set_operation_flag(IMV_IMG_DL_AND_PARSE_REQUEST, true);
								}

								Line_suspend();
								Menu_set_operation_flag(MENU_JUMP_TO_IMV_REQUEST, true);
							}
							//add id
							else if (!(type_pos[1] == std::string::npos) && hid_touch_pos_y >= (line_y + line_interval * i) - 240.0
								&& hid_touch_pos_y <= (line_y + line_interval * i) - 220.0
								&& hid_touch_pos_x >= (line_x - 40.0) && hid_touch_pos_x <= (line_x + 460.0) && !Line_query_operation_flag(LINE_DL_LOG_REQUEST)
								&& !Line_query_operation_flag(LINE_PARSE_LOG_REQUEST) && !Line_query_setting(LINE_AUTO_UPDATE) && !Line_query_setting(LINE_LOAD_LOG_REQUEST))
							{
								cut_pos[0] = content.find("<id>");
								cut_pos[1] = content.find("</id>");
								if (cut_pos[0] == std::string::npos || cut_pos[1] == std::string::npos)
									Menu_set_clipboard("");
								else
									Menu_set_clipboard(content.substr((cut_pos[0] + 4), cut_pos[1] - (cut_pos[0] + 4)));

								saved_y[1] = line_y;
								Line_set_x_y_size_interval(LINE_TEXT_Y, saved_y[0]);
								Line_set_operation_flag(LINE_SELECT_CHAT_ROOM_REQUEST, true);
								Line_reset_msg();
								Line_set_operation_flag(LINE_TYPE_ID_REQUEST, true);
							}
						}
					}
				}

				if (hid_key_touch_press && hid_touch_pos_x >= 305 && hid_touch_pos_x <= 320 && hid_touch_pos_y >= 15 && hid_touch_pos_y < 220)
					scroll_bar_selected = true;
				else if (hid_key_touch_press && hid_touch_pos_y <= 134)
					scroll_mode = true;
				else if (hid_key_touch_press && hid_touch_pos_x > 10 && hid_touch_pos_x < 59 && hid_touch_pos_y > 170 && hid_touch_pos_y < 180)
					Line_set_selected_num(LINE_SELECTED_MENU_MODE_NUM, LINE_MENU_SEND);
				else if (hid_key_touch_press && hid_touch_pos_x > 60 && hid_touch_pos_x < 109 && hid_touch_pos_y > 170 && hid_touch_pos_y < 180)
					Line_set_selected_num(LINE_SELECTED_MENU_MODE_NUM, LINE_MENU_RECEIVE);
				else if (hid_key_touch_press && hid_touch_pos_x > 110 && hid_touch_pos_x < 159 && hid_touch_pos_y > 170 && hid_touch_pos_y < 180)
					Line_set_selected_num(LINE_SELECTED_MENU_MODE_NUM, LINE_MENU_COPY);
				else if (hid_key_touch_press && hid_touch_pos_x > 160 && hid_touch_pos_x < 209 && hid_touch_pos_y > 170 && hid_touch_pos_y < 180)
					Line_set_selected_num(LINE_SELECTED_MENU_MODE_NUM, LINE_MENU_SETTINGS);
				else if (hid_key_touch_press && hid_touch_pos_x > 210 && hid_touch_pos_x < 259 && hid_touch_pos_y > 170 && hid_touch_pos_y < 180)
					Line_set_selected_num(LINE_SELECTED_MENU_MODE_NUM, LINE_MENU_SEARCH);
				else if (hid_key_touch_press && hid_touch_pos_x > 260 && hid_touch_pos_x < 309 && hid_touch_pos_y > 170 && hid_touch_pos_y < 180)
					Line_set_selected_num(LINE_SELECTED_MENU_MODE_NUM, LINE_MENU_ADVANCED);
				else if (hid_key_touch_press && hid_touch_pos_x >= 260 && hid_touch_pos_x <= 300 && hid_touch_pos_y >= 135 && hid_touch_pos_y <= 157 && !Line_query_operation_flag(LINE_DL_LOG_REQUEST)
					&& !Line_query_operation_flag(LINE_PARSE_LOG_REQUEST) && !Line_query_setting(LINE_AUTO_UPDATE) && !Line_query_setting(LINE_LOAD_LOG_REQUEST)
				&& !Line_query_operation_flag(LINE_SEND_MSG_REQUEST) && !Line_query_operation_flag(LINE_SEND_STICKER_REQUEST) && !Line_query_operation_flag(LINE_SEND_CONTENT_REQUEST))
				{
					saved_y[1] = line_y;
					Line_set_x_y_size_interval(LINE_TEXT_Y, saved_y[0]);
					Line_set_operation_flag(LINE_SELECT_CHAT_ROOM_REQUEST, true);
					Line_reset_msg();
				}
				else if (Line_query_selected_num(LINE_SELECTED_MENU_MODE_NUM) == LINE_MENU_SEND && Line_query_operation_flag(LINE_SEND_SUCCESS))
				{
					if (hid_key_A_press || (hid_key_touch_press && hid_touch_pos_x >= 20 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 197))
						Line_set_operation_flag(LINE_SEND_SUCCESS, false);
				}
				else if (Line_query_selected_num(LINE_SELECTED_MENU_MODE_NUM) == LINE_MENU_SEND && !Line_query_operation_flag(LINE_SENDING_MSG))
				{
					if (hid_key_A_press || (hid_key_touch_press && hid_touch_pos_x >= 20 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 197))
						Line_set_operation_flag(LINE_TYPE_MSG_REQUEST, true);
					else if (hid_key_Y_press || (hid_key_touch_press && hid_touch_pos_x >= 20 && hid_touch_pos_x <= 149 && hid_touch_pos_y >= 205 && hid_touch_pos_y <= 217))
						Line_set_operation_flag(LINE_SELECT_STICKER_REQUEST, true);
					else if (hid_key_X_press || (hid_key_touch_press && hid_touch_pos_x >= 170 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 205 && hid_touch_pos_y <= 217))
					{
						Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
						Line_set_operation_flag(LINE_SELECT_FILE_REQUEST, true);
					}
				}
				else if (Line_query_selected_num(LINE_SELECTED_MENU_MODE_NUM) == LINE_MENU_RECEIVE)
				{
					if (hid_key_B_press || (hid_key_touch_press && hid_touch_pos_x >= 20 && hid_touch_pos_x <= 149 && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 197))
						Line_set_operation_flag(LINE_DL_LOG_REQUEST, true);
					else if (hid_key_X_press || (hid_key_touch_press && hid_touch_pos_x >= 170 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 197))
						Line_set_setting(LINE_AUTO_UPDATE, !(Line_query_setting(LINE_AUTO_UPDATE)));
					else if (hid_key_touch_press && hid_touch_pos_x >= 100 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 205 && hid_touch_pos_y <= 217 && !Line_query_operation_flag(LINE_DL_LOG_REQUEST) && !Line_query_operation_flag(LINE_AUTO_UPDATE))
						bar_selected[0] = true;
					else if (bar_selected[0] && hid_key_touch_held && !Line_query_operation_flag(LINE_DL_LOG_REQUEST) && !Line_query_operation_flag(LINE_AUTO_UPDATE))
					{
						if(hid_touch_pos_x <= 99)
							pos_x = 100;
						else if(hid_touch_pos_x >= 300)
							pos_x = 299;
						else
							pos_x = hid_touch_pos_x;

						Line_set_selected_num(LINE_SELECTED_NUM_OF_LOGS, (pos_x - 99) * 20);
					}
				}
				else if (Line_query_selected_num(LINE_SELECTED_MENU_MODE_NUM) == LINE_MENU_COPY)
				{
					if ((hid_key_X_press || (hid_key_touch_press && hid_touch_pos_x >= 90 && hid_touch_pos_x <= 149 && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 215)))
						Menu_set_clipboard(Line_query_msg_log(Line_query_selected_num(LINE_SELECTED_HIGHLIGHT_NUM)));
					else if (hid_key_touch_press && hid_touch_pos_x >= 160 && hid_touch_pos_x <= 219 && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 214)
						button_selected[0] = true;
					else if (hid_key_touch_press && hid_touch_pos_x >= 230 && hid_touch_pos_x <= 289 && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 214)
						button_selected[1] = true;

					else if ((hid_key_D_DOWN_held || hid_key_D_UP_held) || (((button_selected[0] && hid_touch_pos_x >= 160 && hid_touch_pos_x <= 219)
						|| (button_selected[1] && hid_touch_pos_x >= 230 && hid_touch_pos_x <= 289)) && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 214))
					{
						if(cool_time > 0)
							cool_time--;
						else if((hid_key_D_DOWN_held || (button_selected[0] && hid_touch_pos_x >= 160 && hid_touch_pos_x <= 219)) && Line_query_max_msg() >= Line_query_selected_num(LINE_SELECTED_HIGHLIGHT_NUM) + 1)
						{
							Line_set_selected_num(LINE_SELECTED_HIGHLIGHT_NUM, Line_query_selected_num(LINE_SELECTED_HIGHLIGHT_NUM) + 1);
							if(hid_held_time > 150)
								cool_time = 1;
							else
								cool_time = 20;
						}
						else if((hid_key_D_UP_held || (button_selected[1] && hid_touch_pos_x >= 230 && hid_touch_pos_x <= 289)) && Line_query_selected_num(LINE_SELECTED_HIGHLIGHT_NUM) - 1 >= 0)
						{
							Line_set_selected_num(LINE_SELECTED_HIGHLIGHT_NUM, Line_query_selected_num(LINE_SELECTED_HIGHLIGHT_NUM) - 1);
							if(hid_held_time > 150)
								cool_time = 1;
							else
								cool_time = 20;
						}

						Line_set_x_y_size_interval(LINE_TEXT_Y, Line_query_msg_pos(Line_query_selected_num(LINE_SELECTED_HIGHLIGHT_NUM)));
					}
				}
				else if (Line_query_selected_num(LINE_SELECTED_MENU_MODE_NUM) == LINE_MENU_SETTINGS)
				{
					if ((hid_key_D_UP_held || (hid_key_touch_held && hid_touch_pos_x > 20 && hid_touch_pos_x < 80 && hid_touch_pos_y > 185 && hid_touch_pos_y < 215 && button_selected[0])) && (line_interval + 0.5) < 250.0)
						Line_set_x_y_size_interval(LINE_TEXT_INTERVAL, (line_interval + 0.5));
					else if ((hid_key_D_DOWN_held || (hid_key_touch_held && hid_touch_pos_x > 90 && hid_touch_pos_x < 150 && hid_touch_pos_y > 185 && hid_touch_pos_y < 215 && button_selected[1])) && (line_interval - 0.5) > 10.0)
						Line_set_x_y_size_interval(LINE_TEXT_INTERVAL, (line_interval - 0.5));
					else if ((hid_key_L_held || (hid_key_touch_held && hid_touch_pos_x > 170 && hid_touch_pos_x < 230 && hid_touch_pos_y > 185 && hid_touch_pos_y < 215 && button_selected[2])) && (line_size - 0.003) > 0.25)
						Line_set_x_y_size_interval(LINE_TEXT_SIZE, (line_size - 0.003));
					else if ((hid_key_R_held || (hid_key_touch_held && hid_touch_pos_x > 240 && hid_touch_pos_x < 300 && hid_touch_pos_y > 185 && hid_touch_pos_y < 215 && button_selected[3])) && (line_size + 0.003) < 3.0)
						Line_set_x_y_size_interval(LINE_TEXT_SIZE, (line_size + 0.003));
					else if (hid_key_touch_press && hid_touch_pos_x > 20 && hid_touch_pos_x < 80 && hid_touch_pos_y > 185 && hid_touch_pos_y < 215)
						button_selected[0] = true;
					else if (hid_key_touch_press && hid_touch_pos_x > 90 && hid_touch_pos_x < 150 && hid_touch_pos_y > 185 && hid_touch_pos_y < 215)
						button_selected[1] = true;
					else if (hid_key_touch_press && hid_touch_pos_x > 170 && hid_touch_pos_x < 230 && hid_touch_pos_y > 185 && hid_touch_pos_y < 215)
						button_selected[2] = true;
					else if (hid_key_touch_press && hid_touch_pos_x > 240 && hid_touch_pos_x < 300 && hid_touch_pos_y > 185 && hid_touch_pos_y < 215)
						button_selected[3] = true;
				}
				else if (Line_query_selected_num(LINE_SELECTED_MENU_MODE_NUM) == LINE_MENU_SEARCH)
				{
					if (hid_key_A_press || (hid_key_touch_press && hid_touch_pos_x >= 90 && hid_touch_pos_x <= 149 && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 214))
						Line_set_operation_flag(LINE_SEARCH_REQUEST, true);
					else if (hid_key_touch_press && hid_touch_pos_x >= 160 && hid_touch_pos_x <= 219 && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 214)
						button_selected[0] = true;
					else if (hid_key_touch_press && hid_touch_pos_x >= 230 && hid_touch_pos_x <= 289 && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 214)
						button_selected[1] = true;

					else if ((hid_key_D_DOWN_held || hid_key_D_UP_held) || (((button_selected[0] && hid_touch_pos_x >= 160 && hid_touch_pos_x <= 219)
						|| (button_selected[1] && hid_touch_pos_x >= 230 && hid_touch_pos_x <= 289)) && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 214))
					{
						if(cool_time > 0)
							cool_time--;
						else if((hid_key_D_DOWN_held || (button_selected[0] && hid_touch_pos_x >= 160 && hid_touch_pos_x <= 219)) && Line_query_max_search_result() >= Line_query_selected_num(LINE_SELECTED_SEARCH_HIGHLIGHT_NUM) + 1)
						{
							Line_set_selected_num(LINE_SELECTED_SEARCH_HIGHLIGHT_NUM, Line_query_selected_num(LINE_SELECTED_SEARCH_HIGHLIGHT_NUM) + 1);
							if(hid_held_time > 150)
								cool_time = 1;
							else
								cool_time = 20;
						}
						else if((hid_key_D_UP_held || (button_selected[1] && hid_touch_pos_x >= 230 && hid_touch_pos_x <= 289)) && Line_query_selected_num(LINE_SELECTED_SEARCH_HIGHLIGHT_NUM) - 1 >= 0)
						{
							Line_set_selected_num(LINE_SELECTED_SEARCH_HIGHLIGHT_NUM, Line_query_selected_num(LINE_SELECTED_SEARCH_HIGHLIGHT_NUM) - 1);
							if(hid_held_time > 150)
								cool_time = 1;
							else
								cool_time = 20;
						}

						Line_set_selected_num(LINE_SELECTED_HIGHLIGHT_NUM, Line_query_search_num(Line_query_selected_num(LINE_SELECTED_SEARCH_HIGHLIGHT_NUM)));
						Line_set_x_y_size_interval(LINE_TEXT_Y, Line_query_msg_pos(Line_query_search_num(Line_query_selected_num(LINE_SELECTED_SEARCH_HIGHLIGHT_NUM))));
					}
				}
				else if (Line_query_selected_num(LINE_SELECTED_MENU_MODE_NUM) == LINE_MENU_ADVANCED)
				{
					if (((hid_key_L_press && hid_key_R_held) || (hid_key_L_held && hid_key_R_press)) || (hid_key_touch_press && hid_touch_pos_x >= 20 && hid_touch_pos_x <= 299 && hid_touch_pos_y >= 185 && hid_touch_pos_y <= 214))
						Line_set_operation_flag(LINE_DELETE_ID_CHECK_REQUEST, true);
				}
			}

			if (hid_key_C_DOWN_held || hid_key_C_UP_held)
			{
				if (hid_held_time > 600)
					Line_set_x_y_size_interval(LINE_TEXT_Y, (line_y + ((double)circle_pos.dy * scroll_speed * 0.5)));
				else if (hid_held_time > 240)
					Line_set_x_y_size_interval(LINE_TEXT_Y, (line_y + ((double)circle_pos.dy * scroll_speed * 0.125)));
				else
					Line_set_x_y_size_interval(LINE_TEXT_Y, (line_y + ((double)circle_pos.dy * scroll_speed * 0.0625)));
			}
			if (hid_key_C_LEFT_held || hid_key_C_RIGHT_held)
			{
				if (hid_held_time > 240)
					Line_set_x_y_size_interval(LINE_TEXT_X, (line_x - ((double)circle_pos.dx * scroll_speed * 0.125)));
				else
					Line_set_x_y_size_interval(LINE_TEXT_X, (line_x - ((double)circle_pos.dx * scroll_speed * 0.0625)));
			}
			line_x = Line_query_x_y_size_interval(LINE_TEXT_X);
			line_y = Line_query_x_y_size_interval(LINE_TEXT_Y);

			if (scroll_bar_selected && hid_key_touch_held)
			{
				hid_touch_pos_x_move_left = 0;
				hid_touch_pos_y_move_left = 0;
				Line_set_x_y_size_interval(LINE_TEXT_Y, Line_query_max_y() * ((hid_touch_pos_y - 15.0) / 195.0));
				line_y = Line_query_x_y_size_interval(LINE_TEXT_Y);
			}
			else if (scroll_mode && hid_key_touch_held)
			{
				hid_touch_pos_x_move_left += hid_touch_pos_x_moved;
				hid_touch_pos_y_move_left += hid_touch_pos_y_moved;
			}

			Line_set_x_y_size_interval(LINE_TEXT_X, (line_x - (hid_touch_pos_x_move_left * scroll_speed)));
			Line_set_x_y_size_interval(LINE_TEXT_Y, (line_y - (hid_touch_pos_y_move_left * scroll_speed)));
			line_x = Line_query_x_y_size_interval(LINE_TEXT_X);
			line_y = Line_query_x_y_size_interval(LINE_TEXT_Y);

			if (line_y > 0.0)
				Line_set_x_y_size_interval(LINE_TEXT_Y, 0.0);
			if (line_y < Line_query_max_y())
				Line_set_x_y_size_interval(LINE_TEXT_Y, Line_query_max_y());
			if (line_x > 40.0)
				Line_set_x_y_size_interval(LINE_TEXT_X, 40.0);
			if (line_x < -500.0)
				Line_set_x_y_size_interval(LINE_TEXT_X, -500.0);
		}
		else if (gtr_run && !hid_disabled)
		{
			if (hid_key_START_press || (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 230 && hid_touch_pos_y >= 220 && hid_touch_pos_y <= 240))
				Gtr_suspend();

			if (Gtr_query_operation_flag(GTR_SELECT_SORCE_LANG_REQUEST))
			{
				if (hid_key_touch_press)
				{
					for (int i = 0; i < 10; i++)
					{
						if (hid_touch_pos_x >= 27 && hid_touch_pos_x <= 279 && hid_touch_pos_y >= 20 + (i * 18) && hid_touch_pos_y <= 37 + (i * 18))
						{
							if (i == (int)Gtr_query_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D))
							{
								Gtr_set_sorce_lang((int)Gtr_query_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D) + (int)Gtr_query_offset(GTR_SORCE_LANG_OFFSET));
								Gtr_set_operation_flag(GTR_SELECT_SORCE_LANG_REQUEST, false);
							}
							else
								Gtr_set_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D, i);

							break;
						}
					}
				}
				else if (hid_key_A_press)
				{
					Gtr_set_sorce_lang((int)Gtr_query_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D) + (int)Gtr_query_offset(GTR_SORCE_LANG_OFFSET));
					Gtr_set_operation_flag(GTR_SELECT_SORCE_LANG_REQUEST, false);
				}
				else if (hid_key_Y_press)
					Gtr_set_operation_flag(GTR_SELECT_SORCE_LANG_REQUEST, false);
				else if (hid_key_D_LEFT_held)
				{
					if((Gtr_query_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D) - 0.5) >= 0.0)
						Gtr_set_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D, Gtr_query_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D) - 0.5);
					else if((Gtr_query_offset(GTR_SORCE_LANG_OFFSET) - 0.5) >= 0.0)
						Gtr_set_offset(GTR_SORCE_LANG_OFFSET, Gtr_query_offset(GTR_SORCE_LANG_OFFSET) - 0.5);
				}
				else if (hid_key_D_RIGHT_held)
				{
					if((Gtr_query_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D) + 0.5) <= 9.75)
						Gtr_set_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D, Gtr_query_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D) + 0.5);
					else if ((Gtr_query_offset(GTR_SORCE_LANG_OFFSET) + 0.5) <= 95.75)
						Gtr_set_offset(GTR_SORCE_LANG_OFFSET, Gtr_query_offset(GTR_SORCE_LANG_OFFSET) + 0.5);
				}
				else if (hid_key_D_UP_held)
				{
					if((Gtr_query_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D) - 0.125) >= 0.0)
						Gtr_set_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D, Gtr_query_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D) - 0.125);
					else if ((Gtr_query_offset(GTR_SORCE_LANG_OFFSET) - 0.125) >= 0.0)
						Gtr_set_offset(GTR_SORCE_LANG_OFFSET, Gtr_query_offset(GTR_SORCE_LANG_OFFSET) - 0.125);
				}
				else if (hid_key_D_DOWN_held)
				{
					if((Gtr_query_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D) + 0.125) <= 9.75)
						Gtr_set_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D, Gtr_query_selected_num_d(GTR_SELECTED_SORCE_LANG_NUM_D) + 0.125);
					else if ((Gtr_query_offset(GTR_SORCE_LANG_OFFSET) + 0.125) <= 95.75)
						Gtr_set_offset(GTR_SORCE_LANG_OFFSET, Gtr_query_offset(GTR_SORCE_LANG_OFFSET) + 0.125);
				}
			}
			else if (Gtr_query_operation_flag(GTR_SELECT_TARGET_LANG_REQUEST))
			{
				if (hid_key_touch_press)
				{
					for (int i = 0; i < 10; i++)
					{
						if (hid_touch_pos_x >= 27 && hid_touch_pos_x <= 279 && hid_touch_pos_y >= 20 + (i * 18) && hid_touch_pos_y <= 37 + (i * 18))
						{
							if (i == (int)Gtr_query_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D))
							{
								Gtr_set_target_lang((int)Gtr_query_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D) + (int)Gtr_query_offset(GTR_TARGET_LANG_OFFSET));
								Gtr_set_operation_flag(GTR_SELECT_TARGET_LANG_REQUEST, false);
							}
							else
								Gtr_set_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D, i);

							break;
						}
					}
				}
				else if (hid_key_A_press)
				{
					Gtr_set_target_lang((int)Gtr_query_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D) + (int)Gtr_query_offset(GTR_TARGET_LANG_OFFSET));
					Gtr_set_operation_flag(GTR_SELECT_TARGET_LANG_REQUEST, false);
				}
				else if (hid_key_Y_press)
					Gtr_set_operation_flag(GTR_SELECT_TARGET_LANG_REQUEST, false);
				else if (hid_key_D_LEFT_held)
				{
					if ((Gtr_query_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D) - 0.5) >= 0.0)
						Gtr_set_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D, Gtr_query_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D) - 0.5);
					else if ((Gtr_query_offset(GTR_TARGET_LANG_OFFSET) - 0.5) >= 0.0)
						Gtr_set_offset(GTR_TARGET_LANG_OFFSET, Gtr_query_offset(GTR_TARGET_LANG_OFFSET) - 0.5);
				}
				else if (hid_key_D_RIGHT_held)
				{
					if ((Gtr_query_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D) + 0.5) <= 9.75)
						Gtr_set_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D, Gtr_query_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D) + 0.5);
					else if ((Gtr_query_offset(GTR_TARGET_LANG_OFFSET) + 0.5) <= 95.75)
						Gtr_set_offset(GTR_TARGET_LANG_OFFSET, Gtr_query_offset(GTR_TARGET_LANG_OFFSET) + 0.5);
				}
				else if (hid_key_D_UP_held)
				{
					if ((Gtr_query_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D) - 0.125) >= 0.0)
						Gtr_set_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D, Gtr_query_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D) - 0.125);
					else if ((Gtr_query_offset(GTR_TARGET_LANG_OFFSET) - 0.125) >= 0.0)
						Gtr_set_offset(GTR_TARGET_LANG_OFFSET, Gtr_query_offset(GTR_TARGET_LANG_OFFSET) - 0.125);
				}
				else if (hid_key_D_DOWN_held)
				{
					if ((Gtr_query_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D) + 0.125) <= 9.75)
						Gtr_set_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D, Gtr_query_selected_num_d(GTR_SELECTED_TARGET_LANG_NUM_D) + 0.125);
					else if ((Gtr_query_offset(GTR_TARGET_LANG_OFFSET) + 0.125) <= 95.75)
						Gtr_set_offset(GTR_TARGET_LANG_OFFSET, Gtr_query_offset(GTR_TARGET_LANG_OFFSET) + 0.125);
				}
			}
			else
			{
				if (hid_key_A_press || (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 84 && hid_touch_pos_y >= 190 && hid_touch_pos_y <= 204))
					Gtr_set_operation_flag(GTR_TYPE_TEXT_REQUEST, true);
				else if (hid_key_X_press || (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 84 && hid_touch_pos_y >= 210 && hid_touch_pos_y <= 224))
					Menu_set_clipboard(Gtr_query_tr_history(Gtr_query_selected_num(GTR_SELECTED_HISTORY_NUM)));
				else if (hid_key_D_RIGHT_held)
					Gtr_set_text_pos_x(Gtr_query_text_pos_x() - 5.0);
				else if (hid_key_D_LEFT_held)
				{
					if (Gtr_query_text_pos_x() + 5.0 <= 0.0)
						Gtr_set_text_pos_x(Gtr_query_text_pos_x() + 5.0);
					else
						Gtr_set_text_pos_x(0.0);
				}
				else if ((hid_key_D_DOWN_press || (hid_key_touch_press && hid_touch_pos_x >= 100 && hid_touch_pos_x <= 174 && hid_touch_pos_y >= 210 && hid_touch_pos_y <= 224)) && (Gtr_query_selected_num(GTR_SELECTED_HISTORY_NUM) + 1) <= 9)
					Gtr_set_selected_num(GTR_SELECTED_HISTORY_NUM, Gtr_query_selected_num(GTR_SELECTED_HISTORY_NUM) + 1);
				else if ((hid_key_D_UP_press|| (hid_key_touch_press && hid_touch_pos_x >= 100 && hid_touch_pos_x <= 174 && hid_touch_pos_y >= 190 && hid_touch_pos_y <= 204)) && (Gtr_query_selected_num(GTR_SELECTED_HISTORY_NUM) - 1) >= 0)
					Gtr_set_selected_num(GTR_SELECTED_HISTORY_NUM, Gtr_query_selected_num(GTR_SELECTED_HISTORY_NUM) - 1);
				else if (hid_key_L_press || (hid_key_touch_press && hid_touch_pos_x >= 190 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 190 && hid_touch_pos_y <= 204))
					Gtr_set_operation_flag(GTR_SELECT_SORCE_LANG_REQUEST, true);
				else if (hid_key_R_press || (hid_key_touch_press && hid_touch_pos_x >= 190 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 210 && hid_touch_pos_y <= 224))
					Gtr_set_operation_flag(GTR_SELECT_TARGET_LANG_REQUEST, true);
			}
		}
		else if (spt_run && !hid_disabled)
		{
    		if (hid_key_START_press || (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 230 && hid_touch_pos_y >= 220 && hid_touch_pos_y <= 240))
				Spt_suspend();

			if (hid_key_touch_press)
			{
				for (int i = 0; i < 7; i++)
				{
					if (hid_touch_pos_x >= 100 && hid_touch_pos_x <= 230 && hid_touch_pos_y >= 40 + (i * 20) && hid_touch_pos_y <= 59 + (i * 20))
						Spt_set_spt_data_size(i);
				}
			}
			else if (hid_key_A_press || (hid_touch_pos_x >= 150 && hid_touch_pos_x <= 189 && hid_touch_pos_y >= 190 && hid_touch_pos_y <= 209))
				Spt_start_request();
		}
		else if (imv_run && !hid_disabled)
		{
			if (hid_key_START_press || (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 230 && hid_touch_pos_y >= 220 && hid_touch_pos_y <= 240))
				Imv_suspend();

			if ((hid_key_touch_press || hid_key_touch_held) && hid_touch_pos_y)
			{
				hid_touch_pos_x_move_left = 0;
				hid_touch_pos_y_move_left = 0;

				if (scroll_mode)
				{
					hid_touch_pos_x_move_left += hid_touch_pos_x_moved;
					hid_touch_pos_y_move_left += hid_touch_pos_y_moved;
				}
				else if (hid_key_touch_press && hid_touch_pos_y <= 174)
					scroll_mode = true;
			}
			else
			{
				scroll_mode = false;
				hid_touch_pos_x_move_left -= (hid_touch_pos_x_move_left * 0.025);
				hid_touch_pos_y_move_left -= (hid_touch_pos_y_move_left * 0.025);
				if (hid_touch_pos_x_move_left < 0.5 && hid_touch_pos_x_move_left > -0.5)
					hid_touch_pos_x_move_left = 0;
				if (hid_touch_pos_y_move_left < 0.5 && hid_touch_pos_y_move_left > -0.5)
					hid_touch_pos_y_move_left = 0;
			}

			if (hid_key_C_UP_held || hid_key_C_DOWN_held)
				Imv_set_img_pos_y(Imv_query_img_pos_y() + ((double)circle_pos.dy * scroll_speed) * 0.0625);
			if (hid_key_C_LEFT_held || hid_key_C_RIGHT_held)
				Imv_set_img_pos_x(Imv_query_img_pos_x() - ((double)circle_pos.dx * scroll_speed) * 0.0625);

			if (hid_key_A_press || (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 74 && hid_touch_pos_y >= 175 && hid_touch_pos_y <= 187))
				Imv_set_operation_flag(IMV_IMG_PARSE_REQUEST, true);
			else if (hid_key_B_press || (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 74 && hid_touch_pos_y >= 195 && hid_touch_pos_y <= 207))
				Imv_set_operation_flag(IMV_IMG_DL_REQUEST, true);
			else if (hid_key_X_press || (hid_key_touch_press && hid_touch_pos_x >= 90 && hid_touch_pos_x <= 154 && hid_touch_pos_y >= 175 && hid_touch_pos_y <= 187))
			{
				Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
				Imv_set_operation_flag(IMV_SELECT_FILE_REQUEST, true);
			}
			else if (hid_key_Y_press || (hid_key_touch_press && hid_touch_pos_x >= 90 && hid_touch_pos_x <= 154 && hid_touch_pos_y >= 195 && hid_touch_pos_y <= 207))
				Imv_set_operation_flag(IMV_ADJUST_URL_REQUEST, true);
		  else if (hid_key_L_press || (hid_key_touch_press && hid_touch_pos_x >= 170 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 175 && hid_touch_pos_y <= 187))
				button_selected[0] = true;
			else if (hid_key_R_press || (hid_key_touch_press && hid_touch_pos_x >= 170 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 195 && hid_touch_pos_y <= 207))
				button_selected[1] = true;
			else if ((hid_key_L_held || (hid_key_touch_held && hid_touch_pos_x >= 170 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 175 && hid_touch_pos_y <= 187 && button_selected[0])) && (Imv_query_img_zoom() - 0.01) > 0.04)
				Imv_set_img_zoom(Imv_query_img_zoom() - 0.01);
			else if (hid_key_R_held || (hid_key_touch_held && hid_touch_pos_x >= 170 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 195 && hid_touch_pos_y <= 207 && button_selected[1]))
				Imv_set_img_zoom(Imv_query_img_zoom() + 0.01);

			Imv_set_img_pos_x(Imv_query_img_pos_x() - (hid_touch_pos_x_move_left * scroll_speed));
			Imv_set_img_pos_y(Imv_query_img_pos_y() - (hid_touch_pos_y_move_left * scroll_speed));
		}
		else if (cam_run && !hid_disabled)
		{
			if (hid_key_START_press || (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 230 && hid_touch_pos_y >= 220 && hid_touch_pos_y <= 240))
				Cam_suspend();

			if (!Cam_query_operation_flag(CAM_ENCODE_PICTURES_REQUEST))
			{
				if (hid_key_A_press)
					Cam_set_operation_flag(CAM_TAKE_PICTURES_REQUEST, true);
				else if (hid_key_touch_press && hid_touch_pos_x >= 40 && hid_touch_pos_x <= 279 && hid_touch_pos_y >= 85 && hid_touch_pos_y <= 104)
				{
					for (int i = 0; i < 3; i++)
					{
						if (hid_key_touch_press && hid_touch_pos_x >= 40 + (i * 85) && hid_touch_pos_x <= 109 + (i * 85) && hid_touch_pos_y >= 85 && hid_touch_pos_y <= 104)
						{
							if(Cam_query_cam_settings(CAM_CAMERA) != i)
								Cam_set_cam_settings(CAM_CAMERA, i);

							break;
						}
					}
				}
				else
				{
					for (int i = 0; i < 4; i++)
					{
						if (hid_key_touch_press && hid_touch_pos_x >= 40.0 + (60.0 * i) && hid_touch_pos_x <= 99.0 + (60.0 * i) && hid_touch_pos_y >= 110.0 && hid_touch_pos_y <= 119.0)
						{
							Cam_set_selected_num(CAM_SELECTED_MENU_MODE, i);
							break;
						}
					}
				}

				if (Cam_query_selected_num(CAM_SELECTED_MENU_MODE) == CAM_MENU_RESOLUTION)
				{
					pos_x = 40.0;
					pos_y = 120.0;
					for (int i = 0; i < 9; i++)
					{
						if (i != Cam_query_cam_settings(CAM_RESOLUTION) && hid_key_touch_press && hid_touch_pos_x >= pos_x && hid_touch_pos_x <= pos_x + 59 && hid_touch_pos_y >= pos_y && hid_touch_pos_y <= pos_y + 19)
						{
							Cam_set_cam_settings(CAM_RESOLUTION, i);
							break;
						}

						if (pos_y + 30.0 > 180.0)
						{
							pos_x += 90.0;
							pos_y = 120.0;
						}
						else
							pos_y += 30.0;
					}
				}
				else if (Cam_query_selected_num(CAM_SELECTED_MENU_MODE) == CAM_MENU_FPS)
				{
					pos_x = 40.0;
					pos_y = 120.0;
					for (int i = 0; i < 15; i++)
					{
						if (i != Cam_query_cam_settings(CAM_FPS) && hid_key_touch_press && hid_touch_pos_x >= pos_x && hid_touch_pos_x <= pos_x + 29 && hid_touch_pos_y >= pos_y && hid_touch_pos_y <= pos_y + 19)
						{
							Cam_set_cam_settings(CAM_FPS, i);
							break;
						}

						if (pos_y + 30.0 > 180.0)
						{
							pos_x += 50.0;
							pos_y = 120.0;
						}
						else
							pos_y += 30.0;
					}
				}
				else if (Cam_query_selected_num(CAM_SELECTED_MENU_MODE) == CAM_MENU_ADVANCED_0)
				{
					if (hid_key_touch_press && hid_touch_pos_x >= 40 && hid_touch_pos_x <= 149 && hid_touch_pos_y >= 130 && hid_touch_pos_y <= 149)
						button_selected[0] = true;
					else if (hid_key_touch_press && hid_touch_pos_x >= 40 && hid_touch_pos_x <= 149 && hid_touch_pos_y >= 160 && hid_touch_pos_y <= 179)
						button_selected[1] = true;
					else if (hid_key_touch_press && hid_touch_pos_x >= 180 && hid_touch_pos_x <= 279 && hid_touch_pos_y >= 130 && hid_touch_pos_y <= 149)
						button_selected[2] = true;
					else if (hid_key_touch_press && hid_touch_pos_x >= 180 && hid_touch_pos_x <= 279 && hid_touch_pos_y >= 160 && hid_touch_pos_y <= 179)
						button_selected[3] = true;

					pos_x = 35.0;
					for (int i = 0; i < 11; i++)
					{
						if (i != Cam_query_cam_settings(CAM_CONTRAST) && hid_key_touch_held && hid_touch_pos_x >= pos_x && hid_touch_pos_x <= pos_x + 9 && button_selected[0])
						{
							Cam_set_cam_settings(CAM_CONTRAST, i);
							break;
						}
						pos_x += 10.0;
					}
					pos_x = 30.0;
					for (int i = 0; i < 6; i++)
					{
						if (i != Cam_query_cam_settings(CAM_WHITE_BALANCE) && hid_key_touch_held && hid_touch_pos_x >= pos_x && hid_touch_pos_x <= pos_x + 19 && button_selected[1])
						{
							Cam_set_cam_settings(CAM_WHITE_BALANCE, i);
							break;
						}
						pos_x += 20.0;
					}
					pos_x = 155.0;
					for (int i = 0; i < 3; i++)
					{
						if (i != Cam_query_cam_settings(CAM_LENS_CORRECTION) && hid_key_touch_held && hid_touch_pos_x >= pos_x && hid_touch_pos_x <= pos_x + 49 && button_selected[2])
						{
							Cam_set_cam_settings(CAM_LENS_CORRECTION, i);
							break;
						}
						pos_x += 50.0;
					}
					pos_x = 170.0;
					for (int i = 0; i < 6; i++)
					{
						if (i != Cam_query_cam_settings(CAM_EXPOSURE) && hid_key_touch_held && hid_touch_pos_x >= pos_x && hid_touch_pos_x <= pos_x + 19 && button_selected[3])
						{
							Cam_set_cam_settings(CAM_EXPOSURE, i);
							break;
						}
						pos_x += 20.0;
					}
				}
				else if (Cam_query_selected_num(CAM_SELECTED_MENU_MODE) == CAM_MENU_ADVANCED_1)
				{
					if (hid_key_touch_press && hid_touch_pos_x >= 40 && hid_touch_pos_x <= 79 && hid_touch_pos_y >= 130 && hid_touch_pos_y <= 149)
						Cam_set_selected_num(CAM_SELECTED_FORMAT_MODE, 0);
					else if (hid_key_touch_press && hid_touch_pos_x >= 100 && hid_touch_pos_x <= 139 && hid_touch_pos_y >= 130 && hid_touch_pos_y <= 149)
						Cam_set_selected_num(CAM_SELECTED_FORMAT_MODE, 1);
					else if (hid_key_touch_press && hid_touch_pos_x >= 180 && hid_touch_pos_x <= 219 && hid_touch_pos_y >= 130 && hid_touch_pos_y <= 149)
						Cam_set_cam_settings(CAM_NOISE_FILTER, 1);
					else if (hid_key_touch_press && hid_touch_pos_x >= 240 && hid_touch_pos_x <= 279 && hid_touch_pos_y >= 130 && hid_touch_pos_y <= 149)
						Cam_set_cam_settings(CAM_NOISE_FILTER, 0);
					else if (hid_key_touch_press && hid_touch_pos_x >= 180 && hid_touch_pos_x <= 219 && hid_touch_pos_y >= 160 && hid_touch_pos_y <= 179)
						Cam_set_cam_settings(CAM_SHUTTER_SOUND, 1);
					else if (hid_key_touch_press && hid_touch_pos_x >= 240 && hid_touch_pos_x <= 279 && hid_touch_pos_y >= 160 && hid_touch_pos_y <= 179)
						Cam_set_cam_settings(CAM_SHUTTER_SOUND, 0);

					if (Cam_query_selected_num(CAM_SELECTED_FORMAT_MODE) == 1)
					{
						if (hid_key_touch_press && hid_touch_pos_x >= 40 && hid_touch_pos_x <= 139 && hid_touch_pos_y >= 160 && hid_touch_pos_y <= 179)
							button_selected[0] = true;

						for (int i = 0; i < 100; i++)
						{
							if (hid_key_touch_held && hid_touch_pos_x == 40 + i && button_selected[0])
							{
								Cam_set_selected_num(CAM_SELECTED_JPG_QUALITY, (i + 1));
								break;
							}
						}
					}
				}
			}
		}
		else if (mic_run && !hid_disabled)
		{
			if (hid_key_START_press || (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 230 && hid_touch_pos_y >= 220 && hid_touch_pos_y <= 240))
				Mic_suspend();

			if (hid_key_A_press || (hid_key_touch_press && hid_touch_pos_x >= 105 && hid_touch_pos_x <= 154 && hid_touch_pos_y >= 60 && hid_touch_pos_y <= 109))
				Mic_set_operation_flag(MIC_START_RECORDING_REQUEST, true);
			else if (hid_key_B_press || (hid_key_touch_press && hid_touch_pos_x >= 165 && hid_touch_pos_x <= 214 && hid_touch_pos_y >= 60 && hid_touch_pos_y <= 109))
				Mic_set_operation_flag(MIC_STOP_RECORDING_REQUEST, true);
    }
		else if (mup_run && !hid_disabled)
		{
			if (hid_key_START_press || (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 230 && hid_touch_pos_y >= 220 && hid_touch_pos_y <= 240))
				Mup_suspend();

			if(hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 310 && hid_touch_pos_y >= 120 && hid_touch_pos_y <= 124)
				Mup_set_offset((hid_touch_pos_x - 10));

			if (hid_key_A_press || (hid_key_touch_press && hid_touch_pos_x >= 105 && hid_touch_pos_x <= 154 && hid_touch_pos_y >= 60 && hid_touch_pos_y <= 109))
				Mup_set_operation_flag(MUP_PLAY_MUSIC_REQUEST, true);
			else if (hid_key_B_press || (hid_key_touch_press && hid_touch_pos_x >= 165 && hid_touch_pos_x <= 214 && hid_touch_pos_y >= 60 && hid_touch_pos_y <= 109))
				Mup_set_operation_flag(MUP_STOP_MUSIC_REQUEST, true);
			else if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 49 && hid_touch_pos_y >= 150 && hid_touch_pos_y <= 169)
				Mup_set_operation_flag(MUP_SHUFFLE_REQUEST, true);
			else if (hid_key_touch_press && hid_touch_pos_x >= 60 && hid_touch_pos_x <= 99 && hid_touch_pos_y >= 150 && hid_touch_pos_y <= 169)
				Mup_set_operation_flag(MUP_SHUFFLE_REQUEST, false);
			else if (hid_key_touch_press && hid_touch_pos_x >= 10 && hid_touch_pos_x <= 49 && hid_touch_pos_y >= 180 && hid_touch_pos_y <= 199)
				Mup_set_operation_flag(MUP_LOOP_REQUEST, true);
			else if (hid_key_touch_press && hid_touch_pos_x >= 60 && hid_touch_pos_x <= 99 && hid_touch_pos_y >= 180 && hid_touch_pos_y <= 199)
				Mup_set_operation_flag(MUP_LOOP_REQUEST, false);
			else if (hid_key_touch_press && hid_touch_pos_x >= 110 && hid_touch_pos_x <= 149 && hid_touch_pos_y >= 180 && hid_touch_pos_y <= 199)
				Mup_set_allow_sleep(true);
			else if (hid_key_touch_press && hid_touch_pos_x >= 160 && hid_touch_pos_x <= 199 && hid_touch_pos_y >= 180 && hid_touch_pos_y <= 199)
				Mup_set_allow_sleep(false);
			else if (hid_key_X_press || (hid_key_touch_press && hid_touch_pos_x >= 230 && hid_touch_pos_x <= 309 && hid_touch_pos_y >= 180 && hid_touch_pos_y <= 199))
			{
				Mup_set_operation_flag(MUP_SELECT_FILE_REQUEST, true);
				Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
			}
		}

		if (hid_key_A_press || hid_key_B_press || hid_key_X_press || hid_key_Y_press || hid_key_D_RIGHT_press
			|| hid_key_D_LEFT_press || hid_key_ZL_press || hid_key_ZR_press || hid_key_START_press
			|| hid_key_SELECT_press || hid_key_touch_press || hid_key_A_held || hid_key_B_held
			|| hid_key_X_held || hid_key_Y_held || hid_key_D_DOWN_held || hid_key_D_RIGHT_held
			|| hid_key_D_LEFT_held || hid_key_C_UP_held || hid_key_C_DOWN_held || hid_key_C_RIGHT_held
			|| hid_key_C_LEFT_held || hid_key_D_UP_held || hid_key_touch_held)
		{
			if (Menu_query_afk_time() > Sem_query_settings_i(SEM_TIME_TO_TURN_OFF_LCD) * 2)
				Menu_set_operation_flag(MENU_CHANGE_BRIGHTNESS_REQUEST, true);

			Menu_reset_afk_time();
		}
		if (hid_key_D_UP_held || hid_key_D_DOWN_held || hid_key_D_RIGHT_held || hid_key_D_LEFT_held
			|| hid_key_C_UP_held || hid_key_C_DOWN_held || hid_key_C_RIGHT_held
			|| hid_key_C_LEFT_held || hid_key_touch_held)
			hid_held_time++;
		else
		{
			cool_time = 0;
			hid_held_time = 0;
		}

		gspWaitForVBlank();
	}
	Log_log_save(hid_scan_hid_thread_string, "Thread exit", 1234567890, false);
	threadExit(0);
}

#pragma once
#include "system/types.hpp"

bool Cam_query_init_flag(void);

bool Cam_query_running_flag(void);

void Cam_hid(Hid_info key);

void Cam_resume(void);

void Cam_suspend(void);

Result_with_string Cam_load_msg(std::string lang);

void Cam_init(bool draw);

void Cam_exit(bool draw);

void Cam_main(void);


/*#define CAM_NUM_OF_MSG 32
#define CAM_NUM_OF_OPTION_MSG 52

#define CAM_MSG 0
#define CAM_OPTION_MSG 1
*/

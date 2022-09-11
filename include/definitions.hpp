#pragma once

//settings
#define DEF_MAIN_DIR (std::string)"/Line/"
#define DEF_UPDATE_DIR_PREFIX (std::string)"/3ds/Line_ver_"
#define DEF_UPDATE_FILE_PREFIX (std::string)"Line"
#define DEF_CHECK_INTERNET_URL (std::string)"http://connectivitycheck.gstatic.com/generate_204"
#define DEF_SEND_APP_INFO_URL (std::string)"https://script.google.com/macros/s/AKfycbyn_blFyKWXCgJr6NIF8x6ETs7CHRN5FXKYEAAIrzV6jPYcCkI/exec"
#define DEF_CHECK_UPDATE_URL (std::string)""
#define DEF_HTTP_USER_AGENT (std::string)"line for 3ds " + DEF_CURRENT_APP_VER
#define DEF_CURRENT_APP_VER (std::string)"v2.0.0"
#define DEF_CURRENT_APP_VER_INT 2048
#define DEF_ENABLE_LINE
#define DEF_ENABLE_GTR
#define DEF_ENABLE_SPT
#define DEF_ENABLE_IMV
#define DEF_ENABLE_CAM
#define DEF_ENABLE_MIC
#define DEF_ENABLE_MUP
#define DEF_ENABLE_VID
#define DEF_DRAW_MAX_NUM_OF_SPRITE_SHEETS 128
#define DEF_DECODER_MAX_CACHE_PACKETS 256
#define DEF_DECODER_MAX_AUDIO_TRACKS 8
#define DEF_DECODER_MAX_VIDEO_TRACKS 2
#define DEF_DECODER_MAX_SUBTITLE_TRACKS 8
#define DEF_DECODER_MAX_RAW_IMAGE 128
#define DEF_DECODER_MAX_SUBTITLE_DATA 16
#define DEF_DECODER_MAX_SESSIONS 3
#define DEF_ENCODER_MAX_SESSIONS 2
#define DEF_EXPL_MAX_FILES 1024
#define DEF_HTTP_POST_BUFFER_SIZE 0x80000
#define DEF_SOCKET_BUFFER_SIZE 0x40000
#define DEF_LOG_BUFFER_LINES 512
#define DEF_LOG_COLOR 0xFF00BB60
#define DEF_MUXER_MAX_SESSIONS 2
#define DEF_SPEAKER_MAX_BUFFERS 192
#define DEF_SWKBD_MAX_DIC_WORDS 128
#define DEF_DECODER_USE_DMA
#define DEF_DRAW_USE_DMA

//Line
#define DEF_LINE_NUM_OF_MSG 57
//#define DEF_LINE_ENABLE_ICON
#define DEF_LINE_ENABLE_NAME
#define DEF_LINE_ICON_PATH (std::string)"romfs:/"
#define DEF_LINE_NAME (std::string)"line"
#define DEF_LINE_VER (std::string)"v1.7.3"
#define DEF_LINE_MAIN_STR (std::string)"Line/Main"
#define DEF_LINE_INIT_STR (std::string)"Line/Init"
#define DEF_LINE_EXIT_STR (std::string)"Line/Exit"
#define DEF_LINE_WORKER_THREAD_STR (std::string)"Line/Worker thread"
#define DEF_LINE_LOG_THREAD_STR (std::string)"Line/Log thread"

#define DEF_LINE_GAS_VER 7

#define DEF_LINE_MENU_SEND 0
#define DEF_LINE_MENU_RECEIVE 1
#define DEF_LINE_MENU_COPY 2
#define DEF_LINE_MENU_SEARCH 3
#define DEF_LINE_MENU_SETTINGS 4
#define DEF_LINE_MENU_ADVANCED 5

//Google translation
#define DEF_GTR_NUM_OF_MSG 10
//#define DEF_GTR_ENABLE_ICON
#define DEF_GTR_ENABLE_NAME
#define DEF_GTR_ICON_PATH (std::string)"romfs:/"
#define DEF_GTR_NAME (std::string)"google\ntranslation"
#define DEF_GTR_VER (std::string)"v1.1.3"
#define DEF_GTR_MAIN_STR (std::string)"Gtr/Main"
#define DEF_GTR_INIT_STR (std::string)"Gtr/Init"
#define DEF_GTR_EXIT_STR (std::string)"Gtr/Exit"
#define DEF_GTR_TRANSLATION_THREAD_STR (std::string)"Gtr/Translation thread"

#define DEF_GTR_NUM_OF_LANG_LIST_MSG 105

//Speedtest
#define DEF_SPT_NUM_OF_MSG 12
//#define DEF_SPT_ENABLE_ICON
#define DEF_SPT_ENABLE_NAME
#define DEF_SPT_ICON_PATH (std::string)"romfs:/"
#define DEF_SPT_NAME (std::string)"speedtest"
#define DEF_SPT_VER (std::string)"v1.0.6"
#define DEF_SPT_MAIN_STR (std::string)"Spt/Main"
#define DEF_SPT_INIT_STR (std::string)"Spt/Init"
#define DEF_SPT_EXIT_STR (std::string)"Spt/Exit"
#define DEF_SPT_SPEEDTEST_THREAD_STR (std::string)"Spt/Speedtest thread"
#define DEF_SPT_TIMER_THREAD_STR (std::string)"Spt/Timer thread"

//Image viewer
#define DEF_IMV_NUM_OF_MSG 6
//#define DEF_IMV_ENABLE_ICON
#define DEF_IMV_ENABLE_NAME
#define DEF_IMV_ICON_PATH (std::string)"romfs:/"
#define DEF_IMV_NAME (std::string)"image\nviewer"
#define DEF_IMV_VER (std::string)"v1.0.6"
#define DEF_IMV_MAIN_STR (std::string)"Imv/Main"
#define DEF_IMV_INIT_STR (std::string)"Imv/Init"
#define DEF_IMV_EXIT_STR (std::string)"Imv/Exit"
#define DEF_IMV_PARSE_THREAD_STR (std::string)"Imv/Parse thread"
#define DEF_IMV_LOAD_THREAD_STR (std::string)"Imv/Load thread"
#define DEF_IMV_DL_THREAD_STR (std::string)"Imv/Download thread"

//Camera
#define DEF_CAM_NUM_OF_MSG 32
#define DEF_CAM_NUM_OF_OPTION_MSG 50
//#define DEF_CAM_ENABLE_ICON
#define DEF_CAM_ENABLE_NAME
#define DEF_CAM_ICON_PATH (std::string)"romfs:/"
#define DEF_CAM_NAME (std::string)"camera"
#define DEF_CAM_VER (std::string)"v1.1.0"
#define DEF_CAM_MAIN_STR (std::string)"Cam/Main"
#define DEF_CAM_INIT_STR (std::string)"Cam/Init"
#define DEF_CAM_EXIT_STR (std::string)"Cam/Exit"
#define DEF_CAM_ENCODE_THREAD_STR (std::string)"Cam/Encode thread"
#define DEF_CAM_CAPTURE_THREAD_STR (std::string)"Cam/Capture thread"

#define DEF_CAM_MENU_RESOLUTION 0
#define DEF_CAM_MENU_FPS 1
#define DEF_CAM_MENU_ADVANCED_0 2
#define DEF_CAM_MENU_ADVANCED_1 3

//Mic
#define DEF_MIC_NUM_OF_MSG 4
//#define DEF_MIC_ENABLE_ICON
#define DEF_MIC_ENABLE_NAME
#define DEF_MIC_ICON_PATH (std::string)"romfs:/"
#define DEF_MIC_NAME (std::string)"mic"
#define DEF_MIC_VER (std::string)"v1.1.1"
#define DEF_MIC_MAIN_STR (std::string)"Mic/Main"
#define DEF_MIC_INIT_STR (std::string)"Mic/Init"
#define DEF_MIC_EXIT_STR (std::string)"Mic/Exit"
#define DEF_MIC_RECORD_THREAD_STR (std::string)"Mic/Record thread"

//Music player
#define DEF_MUP_NUM_OF_MSG 21
//#define DEF_MUP_ENABLE_ICON
#define DEF_MUP_ENABLE_NAME
#define DEF_MUP_ICON_PATH (std::string)"romfs:/"
#define DEF_MUP_NAME (std::string)"music\nplayer"
#define DEF_MUP_VER (std::string)"v1.1.2"
#define DEF_MUP_MAIN_STR (std::string)"Mup/Main"
#define DEF_MUP_INIT_STR (std::string)"Mup/Init"
#define DEF_MUP_EXIT_STR (std::string)"Mup/Exit"
#define DEF_MUP_WORKER_THREAD_STR (std::string)"Mup/Worker thread"
#define DEF_MUP_PLAY_THREAD_STR (std::string)"Mup/Play thread"

//video player
#define DEF_VID_NUM_OF_MSG 40
#define DEF_VID_ENABLE_ICON
//#define DEF_VID_ENABLE_NAME
#define DEF_VID_ICON_PATH (std::string)"romfs:/gfx/draw/icon/vid_icon.t3x"
#define DEF_VID_NAME (std::string)"Video\nplayer"
#define DEF_VID_VER (std::string)"v1.5.0"
#define DEF_VID_MAIN_STR (std::string)"Vid/Main"
#define DEF_VID_INIT_STR (std::string)"Vid/Init"
#define DEF_VID_EXIT_STR (std::string)"Vid/Exit"
#define DEF_VID_DECODE_THREAD_STR (std::string)"Vid/Decode thread"
#define DEF_VID_DECODE_VIDEO_THREAD_STR (std::string)"Vid/Decode video thread"
#define DEF_VID_CONVERT_THREAD_STR (std::string)"Vid/Convert thread"
#define DEF_VID_READ_PACKET_THREAD_STR (std::string)"Vid/Read packet thread"
#define DEF_VID_WORKER_THREAD_STR (std::string)"Vid/Worker thread"
#define DEF_VID_TEX_FILTER_MSG 0
#define DEF_VID_CONTROLS_MSG 1
#define DEF_VID_SKIP_FRAME_MSG 2
#define DEF_VID_CONTROL_DESCRIPTION_MSG 3
#define DEF_VID_AUDIO_TRACK_DESCRIPTION_MSG 10
#define DEF_VID_AUDIO_TRACK_MSG 11
#define DEF_VID_FULL_SCREEN_MSG 12
#define DEF_VID_HW_DECODER_MSG 13
#define DEF_VID_HW_CONVERTER_MSG 14
#define DEF_VID_MULTI_THREAD_MSG 15
#define DEF_VID_SKIP_KEY_FRAME_MSG 16
#define DEF_VID_LOWER_RESOLUTION_MSG 17
#define DEF_VID_SEEKING_MSG 18
#define DEF_VID_SEEK_MSG 19
#define DEF_VID_VOLUME_MSG 20
#define DEF_VID_ASPECT_RATIO_MSG 21
#define DEF_VID_MOVE_MODE_MSG 22
#define DEF_VID_REMEMBER_POS_MSG 23
#define DEF_VID_PLAY_METHOD_MSG 24
#define DEF_VID_NO_REPEAT_MSG 25
#define DEF_VID_REPEAT_MSG 26
#define DEF_VID_IN_ORDER_MSG 27
#define DEF_VID_RANDOM_MSG 28
#define DEF_VID_MOVE_MODE_EDIABLE_MSG 29
#define DEF_VID_MOVE_MODE_ENABLE_BOTH_MSG 30
#define DEF_VID_MOVE_MODE_ENABLE_VIDEO_MSG 31
#define DEF_VID_MOVE_MODE_ENABLE_SUBTITLE_MSG 32
#define DEF_VID_SUBTITLE_TRACK_DESCRIPTION_MSG 33
#define DEF_VID_SUBTITLE_TRACK_MSG 34
#define DEF_VID_BRIGHTNESS_MSG 35
#define DEF_VID_DISABLE_AUDIO_MSG 36
#define DEF_VID_DISABLE_VIDEO_MSG 37
#define DEF_VID_DISABLE_SUBTITLE_MSG 38
#define DEF_VID_DL_MSG 39

#define DEF_VID_MENU_NONE -1
#define DEF_VID_MENU_SETTINGS_0 0
#define DEF_VID_MENU_SETTINGS_1 1
#define DEF_VID_MENU_INFO 2

#define DEF_VID_NO_REPEAT 0
#define DEF_VID_REPEAT 1
#define DEF_VID_IN_ORDER 2
#define DEF_VID_RANDOM 3

#define DEF_VID_MOVE_DISABLE 0
#define DEF_VID_MOVE_BOTH 1
#define DEF_VID_MOVE_VIDEO 2
#define DEF_VID_MOVE_SUBTITLE 3

#define DEF_VID_BUFFERS 3

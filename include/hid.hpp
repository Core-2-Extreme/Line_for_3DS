
//Press
#define KEY_P_A 0 //A
#define KEY_P_B 1 //B
#define KEY_P_X 2 //X
#define KEY_P_Y 3 //Y
#define KEY_P_C_UP 4 //Circle pad UP
#define KEY_P_C_RIGHT 5 //Circle pad RIGHT
#define KEY_P_C_DOWN 6 //Circle pad DOWN
#define KEY_P_C_LEFT 7 //Circle pad LEFT
#define KEY_P_D_UP 8 //Direction pad UP
#define KEY_P_D_RIGHT 9 //Direction pad RIGHT
#define KEY_P_D_DOWN 10 //Direction pad DOWN
#define KEY_P_D_LEFT 11 //Direction pad LEFT
#define KEY_P_L 12 //L
#define KEY_P_R 13 //R
#define KEY_P_ZL 14 //ZL
#define KEY_P_ZR 15 //ZR
#define KEY_P_START 16 //START
#define KEY_P_SELECT 17 //SELECT
#define KEY_P_CS_UP 18 //C stick UP
#define KEY_P_CS_RIGHT 19 //C stick RIGHT
#define KEY_P_CS_DOWN 20 //C stick DOWN
#define KEY_P_CS_LEFT 21 //C stick LEFT
#define KEY_P_TOUCH 22 //Touch
//Held
#define KEY_H_A 23
#define KEY_H_B 24
#define KEY_H_X 25
#define KEY_H_Y 26
#define KEY_H_C_UP 27
#define KEY_H_C_RIGHT 28
#define KEY_H_C_DOWN 29
#define KEY_H_C_LEFT 30
#define KEY_H_D_UP 31
#define KEY_H_D_RIGHT 32
#define KEY_H_D_DOWN 33
#define KEY_H_D_LEFT 34
#define KEY_H_L 35
#define KEY_H_R 36
#define KEY_H_ZL 37
#define KEY_H_ZR 38
#define KEY_H_START 39
#define KEY_H_SELECT 40
#define KEY_H_CS_UP 41
#define KEY_H_CS_RIGHT 42
#define KEY_H_CS_DOWN 43
#define KEY_H_CS_LEFT 44
#define KEY_H_TOUCH 45

void Hid_init(void);

void Hid_exit(void);

bool Hid_query_disable_flag(void);

bool Hid_query_key_press_state(int key_num);

bool Hid_query_key_held_state(int key_num);

//true = x, false = y
int Hid_query_touch_pos(bool x);

void Hid_set_disable_flag(bool flag);

void Hid_key_flag_reset(void);

void Hid_scan_hid_thread(void* arg);

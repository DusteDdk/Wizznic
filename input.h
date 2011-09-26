#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#define C_UP 0
#define C_DOWN 1
#define C_LEFT 2
#define C_RIGHT 3
#define C_BTNY 4
#define C_BTNX 5
#define C_BTNA 6
#define C_BTNB 7
#define C_SHOULDERA 8
#define C_SHOULDERB 9
#define C_BTNMENU 10
#define C_BTNSELECT 11
#define C_BTNVOLUP 12
#define C_BTNVOLDOWN 13
#define C_NUM 14

inline int getButton(int btn);
inline int getBtnTime(int btn);
inline void resetBtnTimer(int btn);
inline void resetBtn(int btn);
void resetBtnAll();
int runControls();
void initControls();

#endif // INPUT_H_INCLUDED

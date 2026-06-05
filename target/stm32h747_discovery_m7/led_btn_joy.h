/* LED, BUTTON, JOYSTICK */

/* LED */
void led_on( uint32_t no );
void led_off( uint32_t no );
void led_set( int val );
void led_blink( int i, int j );
void led_blink_btn( int i, int j );     /* ボタンが押されるのを待つ */

/* ボタン */
int   btn_stat(void);
void  btn_pos_edge(void);
void  btn_neg_edge(void);

/* Joystick */
/* Joystck 番号 JOY_SEL..JOY_UP */
typedef enum  {
    JOY_SEL = 0, JOY_DOWN, JOY_LEFT, JOY_RIGHT, JOY_UP
} Joy_Index;

int  joy_stat(void);

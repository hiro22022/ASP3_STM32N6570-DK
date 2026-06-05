#if 1
/*
 * libc.a(lib_a-init.o): in function `__libc_init_array':
 *    init.c:40: undefined reference to `_init'
 */
/*
 *この関数は STM32cube 下の makefile だと 
 *   lib/gcc/arm-none-eabi/9.3.1/thumb/v7e-m+dp/hard/crti.o
 * で定義される
 */
void _init(void){}
#endif // 0

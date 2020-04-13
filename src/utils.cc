#include "../inc/utils.h"
/*==================================
 *
 * 函数：get_time
 * 
 * 函数描述：
 *
 * 参数：
 *
 * 返回值：　char*
 *
 * 抛出异常：
 *
 * 创建人：
 *
 * 修改：
 *
 * ====================================*/
char *get_time() {
    static char tmp[20];
    time_t t;
    struct tm *ltime;
    time(&t);
    ltime = localtime(&t);
    strftime(tmp, 20, "%Y-%m-%d %H:%M:%S", ltime);
    return tmp;
}

unsigned int check_sum(void *buf, unsigned int initsum, unsigned int len){
    return 1;
}


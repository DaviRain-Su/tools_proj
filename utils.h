#pragma once

#include <time.h>

#define DEBUG

#ifdef DEBUG
#define DBG_INFO(fmt, ...) printf("%s " fmt, get_time(), ##__VA_ARGS__)
#define DBG_ERR printf
#define DBG_ASSERT(x)
#else
#define DBG_INFO(...)
#define DBG_ERR printf
#define DBG_ASSERT(x)
#endif

// color setting
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLR "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"

// MD5 AES校验码计算，优化速度可以忽略
unsigned int check_sum(void *buf, unsigned int initsum, unsigned int len);

//获取时间
char *get_time();


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



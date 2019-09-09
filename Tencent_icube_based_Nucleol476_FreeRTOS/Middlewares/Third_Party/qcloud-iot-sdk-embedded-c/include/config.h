#ifndef _SETTING_CONFIG_
#define _SETTING_CONFIG_

#define AUTH_MODE_KEY
#define AUTH_WITH_NOTLS
#define EVENT_POST_ENABLED

#define  OS_USED			//sample use os or nonos
//#undef   OS_USED

#define AT_TCP_ENABLED

#ifdef  AT_TCP_ENABLED
#define AT_UART_RECV_IRQ
#define AT_DEBUG
#undef  AT_DEBUG
#endif

#endif

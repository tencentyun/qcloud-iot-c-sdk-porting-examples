#include <stdio.h>
#include <string.h>

#include "config.h"
#include "cmsis_os.h"
#include "shell.h"
#include "stm32l4xx_hal.h"
#include "utils_ringbuff.h"
#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#define ANSI_COLOR_RED     			"\x1b[31m"
#define ANSI_COLOR_GREEN   			"\x1b[32m"
#define ANSI_COLOR_YELLOW  			"\x1b[33m"
#define ANSI_COLOR_BLUE    			"\x1b[34m"
#define ANSI_COLOR_RESET   			"\x1b[0m"


#define SHELL_PROMPT 				ANSI_COLOR_RED"tc_shell>>"ANSI_COLOR_RESET
#define SHELL_UART_IRQHandler       USART2_IRQHandler

extern char g_WIFI_SSID[];
extern char g_WIFI_PASSWORD[];

extern UART_HandleTypeDef huart2;

static UART_HandleTypeDef *pShellUart = &huart2;
static shell g_shell;
static sRingbuff g_ring_buff;

static void wificonfig_deal(char *cmd);
static void print_help(char *cmd);


static shell_cmd sg_shell_cmd_list[] = {
	//=====cmd not exit shell==========//
	{"help",          			print_help},
	{"wificonfig",   			wificonfig_deal},	

	//=====cmd  exit shell==========//
	{"mqtt_sample",   			mqtt_sample},
	{"shadow_sample",   		shadow_sample},
	{"light_sample",   			light_data_template_sample},
	{"icube_data_template_sample",	icube_data_template_sample},
	
};

#define CMD_EXIT_SHELL_START 	2
 
void shell_uart_init(void)
{
	char * ringBuff = HAL_Malloc(SHELL_RRING_BUFF_LEN);
	if(NULL == ringBuff)
	{
		Log_e("malloc ringbuff err");
		
	}
	ring_buff_init(&g_ring_buff, ringBuff, SHELL_RRING_BUFF_LEN);
}

int shell_uart_get_char(void)
{
	uint8_t ch;
	if(0 == ring_buff_pop_data(&g_ring_buff, &ch, 1)) //push data to ringbuff @ AT_UART_IRQHandler
	{
		return -1;
	}
	return ch;
}

int shell_uart_put_char(int ch)
{
  HAL_UART_Transmit(pShellUart, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}


void SHELL_UART_IRQHandler(void)
{ 
	uint8_t ch;
	if(__HAL_UART_GET_FLAG(pShellUart, UART_FLAG_RXNE) == SET)
	{	
		ch = (uint8_t) READ_REG(pShellUart->Instance->RDR)&0xFF;	
		ring_buff_push_data(&g_ring_buff, &ch, 1);
	}
	__HAL_UART_CLEAR_PEFLAG(pShellUart);
}


int cmd_parse_args(const char *req_args, const char *req_expr, ...)
{
    va_list args;
    int req_args_num = 0;

    va_start(args, req_expr);
    req_args_num = vsscanf(req_args, req_expr, args);  
    va_end(args);

    return req_args_num;
}

static void print_help(char *cmd)
{
	printf("\n\r=========================================MENU==================================================\r\n");
	printf("|----------------CMD----------------------|----------------DESCRIPTION------------------------|\r\n");
	printf("|help                                     | Print Menu\r\n");
	printf("|wificonfig                               | Config Wifi ssid and psk\r\n");
	printf("|---------------------------------------IOT-HUB-SAMPLE------------------------------------------|\r\n");
	printf("|mqtt_sample                              | Run mqtt sample\r\n");
	printf("|shadow_sample                            | Run shadow sample\r\n");
	printf("----------------------------------------IOT-EXPLORER-SAMPLE-------------------------------------|\r\n");
	printf("|light_sample                             | Run smart light demo with data_template\r\n");
	printf("|icube_data_template_sample               | Run icube sample with self-define data_template\r\n");
	printf("=================================================================================================\r\n");
}

static void wificonfig_deal(char *cmd)
{
	const char *req_expr = "%*s %s %s";
	uint8_t argc;
	
	argc = cmd_parse_args(cmd, req_expr, g_WIFI_SSID, g_WIFI_PASSWORD);
	if(argc != 2)
	{
		printf("Usage: wificonfig \"SSID\" \"PASSWORD\"\r\n");
	}
	else
	{
		printf("Input wificonfig SSID:%s, PSK:%s\r\n", g_WIFI_SSID, g_WIFI_PASSWORD);
	}			
}



int handle_cmd(char *input)
{	
	char *cmd = input;
	const char *cmd_expr = "%s %*s";
	uint8_t argc;
	char cmd_buff[SHELL_CMD_SIZE];
	uint8_t i;
	bool matchFlag = false;
	int ret = -1;
	
	memset(cmd_buff, '\0', SHELL_CMD_SIZE);
	argc = cmd_parse_args(cmd, cmd_expr, cmd_buff);
	(void)argc;

	if(strlen(cmd_buff) > 0)
	{
		for(i = 0; i < sizeof(sg_shell_cmd_list)/sizeof(sg_shell_cmd_list[0]); i++)
		{
			if(!strcmp(cmd_buff, sg_shell_cmd_list[i].cmd_str))
			{
				sg_shell_cmd_list[i].func(cmd);
				matchFlag = true;
				
				if(i < CMD_EXIT_SHELL_START)
				{
					ret = -1;
				}
				else
				{
					ret = 0;
				}
				break;
			}

		}

		if(!matchFlag)
		{
			printf("INVAILD CMD!\r\n");
		}		
	}
	
	return ret;

}

static inline void shell_handle_history()
{
    printf("\033[2K\r");
	printf("%s%s", SHELL_PROMPT, g_shell.line);
}

static inline void shell_push_history()
{
    if (g_shell.line_position != 0)
    {
        /* push history */
        if (g_shell.history_count >= SHELL_HISTORY_LINES)
        {
            /* move history */
            int index;
            for (index = 0; index < SHELL_HISTORY_LINES - 1; index ++)
            {
                memcpy(&g_shell.cmd_history[index][0],
                    &g_shell.cmd_history[index + 1][0], SHELL_CMD_SIZE);
            }
            memset(&g_shell.cmd_history[index][0], 0, SHELL_CMD_SIZE);
            memcpy(&g_shell.cmd_history[index][0], g_shell.line, g_shell.line_position);

            /* it's the maximum history */
            g_shell.history_count = SHELL_HISTORY_LINES;
        }
        else
        {
            memset(&g_shell.cmd_history[g_shell.history_count][0], 0, SHELL_CMD_SIZE);
            memcpy(&g_shell.cmd_history[g_shell.history_count][0], g_shell.line, g_shell.line_position);

            /* increase count and set current history position */
            g_shell.history_count ++;
        }
    }
    g_shell.current_history = g_shell.history_count;
}

static void *shell_memmove(void *dest, const void *src, unsigned long n)
{
    char *tmp = (char *)dest, *s = (char *)src;

    if (s < tmp && tmp < s + n)
    {
        tmp += n;
        s += n;

        while (n--)
            *(--tmp) = *(--s);
    }
    else
    {
        while (n--)
            *tmp++ = *s++;
    }

    return dest;
}

void shell_handle()
{
	int ch;
	
	shell_uart_init();
	print_help(NULL);
	printf(SHELL_PROMPT);
	
	while (1)
	{
		ch = shell_uart_get_char();
		if(ch != -1)
		{	
			if(ch == '\r' || ch == '\t')
			{
				continue;
			}
			
			if (ch == 0x1b)
			{
				g_shell.stat = WAIT_SPEC_KEY;
				continue;
			}
			else if (g_shell.stat == WAIT_SPEC_KEY)
			{
				if (ch == 0x5b)
				{
					g_shell.stat = WAIT_FUNC_KEY;
					continue;
				}

					g_shell.stat = WAIT_NORMAL;
			}
			else if (g_shell.stat == WAIT_FUNC_KEY)
			{
				if (ch == 0x41) /* up key */
				{
					/* prev history */
					if (g_shell.current_history > 0)
					{
						g_shell.current_history --;
					}						
					else
					{
						g_shell.current_history = 0;
						continue;
					}

					/* copy the history command */
					memcpy(g_shell.line, &g_shell.cmd_history[g_shell.current_history][0], SHELL_CMD_SIZE);
					g_shell.line_curpos = g_shell.line_position = strlen(g_shell.line);
					shell_handle_history();
					continue;
				}
				else if (ch == 0x42) /* down key */
				{
					/* next history */
					if (g_shell.current_history < g_shell.history_count - 1)
					g_shell.current_history ++;
				else
				{
					/* set to the end of history */
					if (g_shell.history_count != 0)
					{
						g_shell.current_history = g_shell.history_count - 1;
					}
					else
					{
						continue;
					}					
				}

				memcpy(g_shell.line, &g_shell.cmd_history[g_shell.current_history][0],
				SHELL_CMD_SIZE);
				g_shell.line_curpos = g_shell.line_position = strlen(g_shell.line);
				shell_handle_history();
				continue;
				}
				else if (ch == 0x44) /* left key */
				{
					if (g_shell.line_curpos)
					{
						printf("\b");
						g_shell.line_curpos --;
					}
					continue;
				}
				else if (ch == 0x43) /* right key */
				{
					if (g_shell.line_curpos <g_shell.line_position)
					{
						printf("%c", g_shell.line[g_shell.line_curpos]);
						g_shell.line_curpos ++;
					}
					continue;
				}
			}
			/* handle backspace key */
			if (ch == 0x7f || ch == 0x08)
			{
				if (g_shell.line_curpos == 0)
				{
					continue;	
				}
				g_shell.line_position--;
				g_shell.line_curpos--;

				if (g_shell.line_position > g_shell.line_curpos)
				{
					int i;

					shell_memmove(&g_shell.line[g_shell.line_curpos],\
									&g_shell.line[g_shell.line_curpos + 1],\
						 			g_shell.line_position - g_shell.line_curpos);

					g_shell.line[g_shell.line_position] = 0;
					printf("\b%s  \b", &g_shell.line[g_shell.line_curpos]);
					/* move the cursor to the origin position */
					for (i =  g_shell.line_curpos; i <= g_shell.line_position; i++)
					printf("\b");
				}
				else
				{
					printf("\b \b");
					g_shell.line[g_shell.line_position] = 0;
				}
				continue;
			}

			/* handle end of line, break */
			if (ch == '\n')
			{		
				g_shell.line[g_shell.line_position] = '\0';
				printf("\r\n");

				int results = handle_cmd(g_shell.line);
				if(!results) 
				{
					break;
				}
				shell_push_history();
				printf(SHELL_PROMPT);
				memset((void*)g_shell.line, 0, sizeof(g_shell.line));
				g_shell.line_curpos = g_shell.line_position = 0;
				continue;
			}


			/* it's a large line, discard it */
			if (g_shell.line_position >= SHELL_CMD_SIZE)
			{
				g_shell.line_position = 0;
			}
			
			/* normal character */
			if (g_shell.line_curpos < g_shell.line_position)
			{
				int i;

				shell_memmove(&g_shell.line[g_shell.line_curpos + 1],\
								&g_shell.line[g_shell.line_curpos],\
								g_shell.line_position - g_shell.line_curpos);

				g_shell.line[g_shell.line_curpos] = ch;
				printf("%s", &g_shell.line[g_shell.line_curpos]);
				/* move the cursor to new position */
				for (i = g_shell.line_curpos; i < g_shell.line_position; i++)
				{
					printf("\b");
				}				
			}
			else
			{
				g_shell.line[g_shell.line_position] = ch;
				printf("%c", ch);
			}

				ch = 0;
				g_shell.line_position ++;
				g_shell.line_curpos++;	
		} 
	}
}

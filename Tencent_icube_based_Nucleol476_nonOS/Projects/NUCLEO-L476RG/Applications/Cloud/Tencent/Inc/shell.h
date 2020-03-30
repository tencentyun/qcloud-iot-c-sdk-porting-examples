#ifndef __SHELL_H
#define __SHELL_H

#define SHELL_RRING_BUFF_LEN 1024

#define SHELL_HISTORY_LINES 5
#define SHELL_CMD_SIZE 80

typedef enum 
{
	WAIT_NORMAL,
	WAIT_SPEC_KEY,
	WAIT_FUNC_KEY,
}eInputStat;


typedef struct 
{
	
	unsigned int current_history;
	unsigned int history_count;
	char cmd_history[SHELL_HISTORY_LINES][SHELL_CMD_SIZE];
	
	eInputStat stat;
	char line[SHELL_CMD_SIZE];
	unsigned int line_position;
	unsigned int line_curpos;
}shell;


typedef struct 
{
	const char *cmd_str;
	void (*func)(char *cmd);	
}shell_cmd;


extern void mqtt_sample(char *cmd);
extern void shadow_sample(char *cmd);
extern void light_data_template_sample(char *cmd);
extern void icube_data_template_sample(char *cmd);


#endif 

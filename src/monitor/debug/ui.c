#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q},
    { "info", "info r: print the number of register info b: print the number of monitor", cmd_info },
    { "si", "continue to execute some steps", cmd_si},
    { "x", "scan the memory", cmd_x},

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_si(char *args)
{
    char *argc=strtok(NULL," ");
    if(argc==NULL)
    {
        cpu_exec(1);
    }else
    {
        int temp=atoi(argc);
        if(temp>=1)
        {
            cpu_exec(temp);
        }else if(temp==-1)
        {
            cpu_exec(-1);
        }
    }
    return 0;
}

static int cmd_info(char *args)
{
    char *argc=strtok(NULL," ");
    if(strcmp(argc,"r")==0)
    {
        int i,j;
        for(i=0;i<8;i++)
        {
            if(i<4)
            {
                printf("%s\t : 0x%02x\n",regsb[i],reg_b(i));
                printf("%s\t : 0x%02x\n",regsb[j],reg_b(j));
            }
            printf("%s\t : 0x%04x\n",regsw[i],reg_w(i));
            printf("%s\t : 0x%08x\n",regsl[i],reg_l(i));
        }
        printf("eip\t : 0x%08x\n",cpu.eip);
    }else if(strcmp(argc,"b")==0)
    {
        
    }else
    {
        printf("Unknown command !");
        return 0;
    }
    return 0;
}

static int cmd_x(char *args)
{
    swaddr_t expr;
    char *argc=strtok(args," ");
    int num=atoi(argc);
    argc=strtok(NULL,"x");
    argc=strtok(NULL," ");
    int i;
    for(i=0;*（argc+i)!='\0';i++)
    {
        expr=expr*16+*(argc+i)-'0';
    }
    for(i=0;i<num;i++)
    {
        printf("0x%08x : %08x\n",expr+i,swaddr_read(expr+i,4));
    }
    return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}

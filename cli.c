#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linenoise.h"
#include <net/if.h>
#include "gateway.h"
#include "network.h"
#include "cmd.h"

void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 's') {
        linenoiseAddCompletion(lc,"set");
        linenoiseAddCompletion(lc,"set ip");
    }
}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf,"seti ")) {
        *color = 35;
        *bold = 0;
        return "ip {ip addr} {netmaks} {gateway}";
    }
    if (!strcasecmp(buf,"set ip")) {
        *color = 35;
        *bold = 0;
        return " {ip addr} {netmaks} {gateway}";
    }
    if (!strcasecmp(buf,"ping")) {
        *color = 35;
        *bold = 0;
        return " [ip addr]";
    }
    if (!strcasecmp(buf,"ping ")) {
        *color = 35;
        *bold = 0;
        return " [ip addr]";
    }
    return NULL;
}

int main(int argc, char **argv) {
    char *line;
    char *prgname = argv[0];

    while(argc > 1) {
        argc--;
        argv++;
        if (!strcmp(*argv,"--multiline")) {
            linenoiseSetMultiLine(1);
            printf("Multi-line mode enabled.\n");
        } else if (!strcmp(*argv,"--keycodes")) {
            linenoisePrintKeyCodes();
            exit(0);
        } else {
            fprintf(stderr, "Usage: %s [--multiline] [--keycodes]\n", prgname);
            exit(1);
        }
    }

    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);

    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

	char shell_prompt[25],*host_ip;
	char *ip="0.0.0.0";
	host_ip = get_host_name();
	if(host_ip==NULL)host_ip=ip;
	snprintf(shell_prompt , sizeof(shell_prompt), "config@[%s] ",host_ip);	
    while((line = linenoise(shell_prompt)) != NULL) {
        if (line[0] != '\0' && line[0] != '/') {
			switch_by_cmd(line);      
            linenoiseHistoryAdd(line); /* Add to the history. */
            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        } else if (!strncmp(line,"/historylen",11)) {
            int len = atoi(line+11);
            linenoiseHistorySetMaxLen(len);
        } else if (line[0] == '/') {
            printf("Unreconized command: %s\n", line);
        }
        free(line);
    }
    return 0;
}

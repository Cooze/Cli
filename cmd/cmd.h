#define SET		"set"
#define PING		"ping"
#define IP		"ip"
#define SHOW		"show"
#define SHUTDOWN	"shutdown"
#define EXIT		"exit"

int juge_cmd(const char* cmd,const char* param_cmd);
void show_usage(const char* args);
void switch_by_cmd(const char* line);


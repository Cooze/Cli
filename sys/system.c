#include <stdlib.h>
#include "system.h"
void shutdown_now(){
   system("/sbin/shutdown -h now");    
}

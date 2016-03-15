#ifndef __PIDOF_HXX__
#define __PIDOF_HXX__

#include <set>
#include <stdlib.h>

std::set<pid_t> get_pidof(char *argv);

#endif

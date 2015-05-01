#pragma once
#include <glog/logging.h>

#define Log(logType)    LOG(logType) 
#define ALog(logType)   Log(logType) << myId()
#define AVLog(level)    VLOG(level) << myId()

/* Verbose logs */
#define LMSG                1
#define LCONFIG             2

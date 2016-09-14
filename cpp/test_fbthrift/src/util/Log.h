#pragma once
#include <glog/logging.h>

/* Context based logging macros */
#define CLog(logType)   LOG(logType) << " [" << getLogContext() << "] "
#define CVLog(logType)  VLOG(logType) << " [" << getLogContext() << "] "

#define ALog(logType)   LOG(logType) << myId()
#define AVLog(level)    VLOG(level) << myId()

/* Verbose logs */
#define LMSG                1
#define LCONFIG             2

#pragma once
#include <glog/logging.h>

#define Log(logType)    LOG(logType) 
#define ALog(logType)   Log(logType) << myId()

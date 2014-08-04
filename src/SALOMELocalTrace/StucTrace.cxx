#include "StucTrace.hxx"
#include <ctime>
#include <iostream>

#include "LocalTraceBufferPool.hxx"

StucTrace::StucTrace(const std::string& label)
: _label(label),
  _beginTime(),
  _endTime(),
  _trace()
{
  begin();
}

StucTrace::~StucTrace()
{
}

void StucTrace::begin()
{
  time_t now = time(0);
  struct tm tstruct = *localtime(&now);
  strftime(_beginTime, sizeof(_beginTime), "%Y-%m-%d %X", &tstruct);
}

void StucTrace::end()
{
  time_t now = time(0);
  struct tm tstruct = *localtime(&now);
  strftime(_endTime, sizeof(_endTime), "%Y-%m-%d %X", &tstruct);
  
  print();
}

const std::string& StucTrace::getTrace()const
{
  _trace = "[STUC TRACE]{\"label\":\"";
  _trace += _label + "\",\"begin\":\"";
  _trace += _beginTime;
  _trace += "\",\"end\":\"";
  _trace += _endTime;
  _trace += "\"}";
  
  return _trace;
}

void StucTrace::print()const
{
  LocalTraceBufferPool::instance()->insert(NORMAL_MESS,(getTrace()+"\n").c_str());
}

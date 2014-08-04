#ifndef __STUC__TRACE__HXX__
#define __STUC__TRACE__HXX__

#include <string>

/*!
 * Print a timestamped and labeled trace.
 * These traces will be recognised by sendTraces.py.
 */
class StucTrace
{
  public:
    /*!
    *  Save the label and the begin time.
    *  The begin time can be changed later by begin().
    */
    StucTrace(const std::string& label);
    
    virtual ~StucTrace();
    
    /*!
    *  Reset the begin time
    */
    void begin();
    
    /*!
    *  Save the end time and print the trace
    */
    void end();
    
    /*!
    *  Get the formated trace.
    */
    const std::string& getTrace()const;
    
    /*!
    *  Overload this method in your derived class in order to define where
    *  the trace should be written.
    */
    virtual void print()const;
    
  private:
    std::string _label;
    char _beginTime[64];
    char _endTime[64];
    mutable std::string _trace;
};

#define STUC_TRACE(label) std::ostringstream os; os << label; StucTrace t(os.str().c_str()); t.print();

#endif
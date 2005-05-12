#ifndef  SALOMEDSCLIENT_DEF_HXX
#define  SALOMEDSCLIENT_DEF_HXX

#include <boost/shared_ptr.hpp> 

template<class T> class shared_ptr: public boost::shared_ptr<T>
{
public:
  shared_ptr() {}
  
  template<class Y>
    explicit shared_ptr(Y * p)
      {
	reset(p);
      }
  
  template<class Y>
    shared_ptr(shared_ptr<Y> const & r):
      boost::shared_ptr<T>(r,boost::detail::dynamic_cast_tag())
	{}
  
  template<class Y>
    shared_ptr & operator=(shared_ptr<Y> const & r)
      {
	shared_ptr<T>(r).swap(*this);
	return *this;
      }
  
  template<class Y> shared_ptr& operator()(Y * p) // Y must be complete
    {
      if(T* pt = dynamic_cast<T*>(p))
	boost::shared_ptr<T>::reset(pt);
      else
	boost::throw_exception(std::bad_cast());
      return *this;
    }
    
};

#define _PTR(Class) shared_ptr<SALOMEDSClient_##Class>
#define _CAST(Class, shared_ptr_Obj) dynamic_cast<SALOMEDS_##Class*>(shared_ptr_Obj.get())

struct STextColor
{
    double R;
    double G;
    double B;
};

#endif

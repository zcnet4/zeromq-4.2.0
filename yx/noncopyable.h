/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\noncopyable.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-10 14:37
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

// -------------------------------------------------------------------------
namespace yx {
  class noncopyable
  {
  protected:
    noncopyable() {}
    ~noncopyable() {}
  private:
    noncopyable(const noncopyable&);
    noncopyable& operator=(const noncopyable&);
  };
}; // namespace yx

typedef yx::noncopyable noncopyable;

// -------------------------------------------------------------------------
#endif /* NONCOPYABLE_H_ */

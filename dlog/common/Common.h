#ifndef DLOG_COMMON_H_
#define DLOG_COMMON_H_

#include <assert.h>

#define DLOG_NS_BEGIN namespace dlog {
#define DLOG_NS_END }

#define DLOG_BEGIN_NAMESPACE(x) namespace dlog { namespace x{ 

#define DLOG_END_NAMESPACE(x) } }

#define DLOG_USE_ROOT_NAMESPACE using namespace dlog;

#define DLOG_USE_NAMESPACE(x) using namespace dlog::x

#define DLOG_NS(x) dlog::x

#define DLOG_ALIAS_NAMESAPCE(x, y) namespace dlog { namespace x = y; }

#include <memory>
#define DLOG_TYPEDEF_PTR(x) typedef std::shared_ptr<x> x##Ptr

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#include <dlog/common/CommonDefine.h>

#endif /*DLOG_COMMON_H_*/



#ifndef DLOG_COMMON_H_
#define DLOG_COMMON_H_

#include <assert.h>

#define DLOG_NS_BEGIN namespace lew {
#define DLOG_NS_END }

#define DLOG_BEGIN_NAMESPACE(x) namespace lew { namespace x{ 

#define DLOG_END_NAMESPACE(x) } }

#define DLOG_USE_ROOT_NAMESPACE using namespace lew;

#define DLOG_USE_NAMESPACE(x) using namespace lew::x

#define DLOG_NS(x) lew::x

#define DLOG_ALIAS_NAMESAPCE(x, y) namespace lew { namespace x = y; }

#include <memory>
#define DLOG_TYPEDEF_PTR(x) typedef std::shared_ptr<x> x##Ptr

#include <dlog/common/CommonDefine.h>

#endif /*DLOG_COMMON_H_*/



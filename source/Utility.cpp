#include "sns/Utility.h"

#include <memmgr/LinearAllocator.h>

namespace sns
{

char* Utility::CopyStr2Alloc(mm::LinearAllocator& alloc, const rapidjson::Value& val)
{
	size_t len = val.GetStringLength();
	if (len == 0) {
		return nullptr;
	}

	char* ret = static_cast<char*>(alloc.alloc<char>(len + 1));

	strncpy(ret, val.GetString(), len);
	ret[len] = 0;

	return ret;
}

char* Utility::CopyStr2Alloc(mm::LinearAllocator& alloc, const std::string& str)
{
	size_t len = str.size();
	if (len == 0) {
		return nullptr;
	}

	char* ret = static_cast<char*>(alloc.alloc<char>(len + 1));

	strncpy(ret, str.c_str(), len);
	ret[len] = 0;

	return ret;
}

}
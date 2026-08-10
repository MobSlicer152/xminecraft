#include "stdafx.h"

struct __crt_internal_free_policy
{
	template <typename T> void operator()(T const* const p) const noexcept
	{
		_free_crt(const_cast<T*>(p));
	}
};

struct __crt_public_free_policy
{
	template <typename T> void operator()(T const* const p) const noexcept
	{
		free(const_cast<T*>(p));
	}
};

extern "C"
{

	void _wassert(_In_z_ wchar_t const* _Message, _In_z_ wchar_t const* _File, _In_ unsigned _Line)
	{
	}

#ifdef _DEBUG
	_NODISCARD _Check_return_ unsigned __int64 __cdecl _byteswap_uint64(_In_ unsigned __int64 _Number)
	{
		return static_cast<unsigned __int64>(_byteswap_ulong(_Number >> 32) |
											 ((unsigned __int64)_byteswap_ulong(_Number & 0xFFFFFFFF) << 32));
	}

	_NODISCARD _Check_return_ unsigned long __cdecl _byteswap_ulong(_In_ unsigned long _Number)
	{

		return static_cast<unsigned long>(_byteswap_ushort(_Number >> 16) | (_byteswap_ushort(_Number & 0xFFFF) << 16));
	}

	_NODISCARD _Check_return_ unsigned short __cdecl _byteswap_ushort(_In_ unsigned short _Number)
	{
		return static_cast<unsigned short>((_Number << 8) | (_Number >> 8));
	}
#endif

	void __cdecl __std_exception_copy(_In_ __std_exception_data const* const from, _Inout_ __std_exception_data* const to)
	{
		_ASSERTE(to->_What == nullptr && to->_DoFree == false);

		if (!from->_DoFree || !from->_What)
		{
			to->_What = from->_What;
			to->_DoFree = false;
			return;
		}

		size_t const buffer_count = strlen(from->_What) + 1;

		auto buffer = static_cast<char*>(malloc(buffer_count));
		if (!buffer)
		{
			return;
		}

		strncpy(buffer, from->_What, buffer_count - 1);
		to->_What = buffer;
		to->_DoFree = true;
	}

	void __cdecl __std_exception_destroy(_Inout_ __std_exception_data* _Data)
	{
		if (_Data->_DoFree)
		{
			free(const_cast<char*>(_Data->_What));
		}

		_Data->_DoFree = false;
		_Data->_What = nullptr;
	}

	void __cdecl __CxxFrameHandler3()
	{
	}

	void __cdecl __std_terminate()
	{
		abort();
	}

	int __cdecl _CrtDbgReportW(_In_ int _ReportType, _In_opt_z_ wchar_t const* _FileName, _In_ int _LineNumber,
							   _In_opt_z_ wchar_t const* _ModuleName, _In_opt_z_ wchar_t const* _Format, ...)
	{
		return 0;
	}
}

namespace std
{

void __cdecl _Xlength_error(const char*)
{
}

void __cdecl _Xout_of_range(_In_z_ const char*)
{
}

} // namespace std

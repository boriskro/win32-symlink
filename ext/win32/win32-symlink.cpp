#include "ruby.h"

#define filecp_to_wstr(str, plen) cstr_to_wstr(filecp(), str, -1, plen)

typedef BOOL (WINAPI* create_symbolic_linkW_t)(LPWSTR src, LPWSTR dest, DWORD flags);
create_symbolic_linkW_t create_symbolic_linkW = NULL;


typedef struct _REPARSE_DATA_BUFFER
{
	ULONG ReparseTag;
	USHORT ReparseDataLength;
	USHORT Reserved;
	union
	{
		struct
		{
			USHORT SubstituteNameOffset;
			USHORT SubstituteNameLength;
			USHORT PrintNameOffset;
			USHORT PrintNameLength;
			ULONG Flags;
			WCHAR PathBuffer[1];
		} SymbolicLinkReparseBuffer;
		struct
		{
			USHORT SubstituteNameOffset;
			USHORT SubstituteNameLength;
			USHORT PrintNameOffset;
			USHORT PrintNameLength;
			WCHAR PathBuffer[1];
		} MountPointReparseBuffer;
		struct
		{
			UCHAR DataBuffer[1];
		} GenericReparseBuffer;
	} DUMMYUNIONNAME;
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;


static inline UINT
filecp(void)
{
	UINT cp = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	return cp;
}

static wchar_t *
cstr_to_wstr(UINT code_page, const char *str, int clen, int *wstr_len)
{
    int len;
	wchar_t* wstr;

    len = MultiByteToWideChar(code_page, 0, str, clen, NULL, 0);
    wstr = (wchar_t *)xmalloc(len * sizeof(wchar_t));

    MultiByteToWideChar(code_page, 0, str, clen, wstr, len);
	if( wstr_len )
		*wstr_len = len;
	return wstr;
}

static VALUE
wstr_to_str(UINT code_page, const wchar_t* wstr, int clen)
{
	int len, term_len;
	VALUE v;
	term_len = clen < 0 ? 1 : 0;
	len = WideCharToMultiByte(code_page, 0, wstr, clen, NULL, 0, NULL, NULL) - term_len;
	v = rb_str_new(NULL, len);
	WideCharToMultiByte(code_page, 0, wstr, clen, RSTRING_PTR(v), len + term_len, NULL, NULL);
	return v;
}

static VALUE
make_error_str(VALUE klass, const char* fmt, ...)
{
	VALUE exc;
	VALUE str;

	va_list va;
	va_start(va, fmt);
	str = rb_vsprintf(fmt, va);
	va_end(va);
	exc = rb_exc_new3(klass, str);
	rb_str_free(str);
	return exc;
}

static VALUE
make_api_error(const char* msg)
{
	DWORD w32_err;
	int myerrno;
	w32_err = GetLastError();
	myerrno = rb_w32_map_errno(w32_err);
	return rb_syserr_new( myerrno == EINVAL ? w32_err : myerrno, msg);
}

static wchar_t*
resolve_api_path(wchar_t* path)
{
	if( 0 == wcsncmp(L"\\??\\UNC\\", path, 8) )
	{
		path += 6;
		*path =  L'\\';
	}
	else if( 0 == wcsncmp(L"\\??\\", path, 4) )
	{
		path += 4;
	}
	return path;
}

static VALUE
win32_readlink(const char* file)
{
	VALUE error = Qnil;
	VALUE target = Qnil;
	HANDLE h = INVALID_HANDLE_VALUE;
	bool ok = true;
	REPARSE_DATA_BUFFER* buf = NULL;
	const size_t buf_size = MAXIMUM_REPARSE_DATA_BUFFER_SIZE;

	buf = (REPARSE_DATA_BUFFER*)xmalloc(buf_size);
	if(ok)
	{
		const DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
		const DWORD access_mode = FILE_READ_EA;
		const DWORD attributes =
				  FILE_FLAG_BACKUP_SEMANTICS
				| FILE_ATTRIBUTE_REPARSE_POINT
				| FILE_FLAG_OPEN_REPARSE_POINT;

		h = CreateFile( file, access_mode, share_mode,
			0, OPEN_EXISTING, attributes, NULL);

		if(!(ok = (h != INVALID_HANDLE_VALUE)))
		{
			error = make_api_error(file);
		}
	}

	if(ok)
	{
		DWORD returned_size;
		ok = 0 != DeviceIoControl (h, FSCTL_GET_REPARSE_POINT,
			NULL, 0, buf, buf_size, &returned_size, NULL);
		if(!ok)
		{
			error = make_api_error(file);
		}
	}
	if(ok)
	{
		wchar_t* raw_target = NULL;
		long target_len = 0;

		switch (buf->ReparseTag)
		{
		case IO_REPARSE_TAG_MOUNT_POINT:
		{
			raw_target = &buf->MountPointReparseBuffer.PathBuffer[
				buf->MountPointReparseBuffer.SubstituteNameOffset / sizeof(wchar_t)];
			target_len = buf->MountPointReparseBuffer.SubstituteNameLength / sizeof(wchar_t);
			break;
		}
		case IO_REPARSE_TAG_SYMLINK:
		{
			raw_target = &buf->SymbolicLinkReparseBuffer.PathBuffer[
				buf->SymbolicLinkReparseBuffer.SubstituteNameOffset / sizeof(wchar_t)];
			target_len = buf->SymbolicLinkReparseBuffer.SubstituteNameLength / sizeof(wchar_t);
			break;
		}
		default:
			ok = false;
			error = make_error_str(rb_eIOError, "file %s is not a symlink", file);
		}
		if(ok)
		{
			wchar_t* resolved;
			resolved = resolve_api_path(raw_target);
			target_len -= resolved - raw_target;
			target = wstr_to_str(filecp(), resolved, target_len);
		}
	}


	if( h != INVALID_HANDLE_VALUE )
	{
		CloseHandle(h);
	}
	xfree(buf);

	if( !NIL_P(error) )
	{
		rb_exc_raise(error);
	}
	return target;
}

static VALUE
rb_readlink(VALUE mod, VALUE file)
{
	(void)mod;
	FilePathValue(file);
	file = rb_str_encode_ospath(file);
	return win32_readlink(RSTRING_PTR(file));
}

static VALUE
rb_symlink_p(VALUE mod, VALUE file)
{
	(void)mod;
	DWORD attrs;
	FilePathValue(file);
	file = rb_str_encode_ospath(file);
	attrs = GetFileAttributes(RSTRING_PTR(file));
	if( attrs == INVALID_FILE_ATTRIBUTES )
	{
		rb_exc_raise(make_api_error(RSTRING_PTR(file)));
	}
	return (attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
}

static bool
is_directory(wchar_t* path)
{
	HANDLE h = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW fd;
	h = FindFirstFileW(path, &fd);
	if( h != INVALID_HANDLE_VALUE )
	{
		FindClose(h);
		return (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ;
	}
	return false;
}

static VALUE
rb_symlink(VALUE mod, VALUE src, VALUE dest)
{
	(void)mod;

	VALUE error = Qnil;
	wchar_t* wsrc = NULL;
	wchar_t* wdest = NULL;
	BOOL res;
	DWORD flags = 0;

	src = FilePathValue(src);
	dest = FilePathValue(dest);
	src = rb_str_encode_ospath(src);
	dest = rb_str_encode_ospath(dest);

	wsrc = filecp_to_wstr(RSTRING_PTR(src), NULL);
	wdest = filecp_to_wstr(RSTRING_PTR(dest), NULL);

	if( is_directory(wdest) )
	{
		flags = SYMBOLIC_LINK_FLAG_DIRECTORY;
	}
	res = create_symbolic_linkW(wsrc, wdest, flags);
	if( !res )
	{
		error = make_api_error(RSTRING_PTR(src));
	}

	xfree(wsrc);
	xfree(wdest);

	if( !NIL_P(error) )
	{
		rb_exc_raise(error);
	}
	return INT2FIX(0);
}

extern "C" void
Init_win32_symlink()
{
	VALUE mod_win32;
	VALUE mod;

	mod_win32 = rb_define_module("Win32");
	mod = rb_define_module_under(mod_win32, "Symlink");

	rb_define_module_function(mod, "readlink", RUBY_METHOD_FUNC(rb_readlink), 1);
	rb_define_module_function(mod, "symlink?", RUBY_METHOD_FUNC(rb_symlink_p), 1);

	create_symbolic_linkW = (create_symbolic_linkW_t)GetProcAddress(
		GetModuleHandle("kernel32.dll"), "CreateSymbolicLinkW");
	if( create_symbolic_linkW )
	{
		rb_define_module_function(mod, "symlink", RUBY_METHOD_FUNC(rb_symlink), 2);
	}
}

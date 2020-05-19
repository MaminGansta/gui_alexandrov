
void doutput(const char* format, ...)
{
#if _DEBUG
	char log[128];
	va_list args;
	va_start(args, format);
	vsprintf_s(log, format, args);
	OutputDebugStringA(log);
	va_end(args);
#endif
}

void doutput(const wchar_t* format, ...)
{
#if _DEBUG
	wchar_t log[128];
	va_list args;
	va_start(args, format);
	vswprintf_s(log, format, args);
	OutputDebugStringW(log);
	va_end(args);
#endif
}


void output(const char* format, ...)
{
	char log[128];
	va_list args;
	va_start(args, format);
	vsprintf_s(log, format, args);
	OutputDebugStringA(log);
	va_end(args);
}

void output(const wchar_t* format, ...)
{
	wchar_t log[128];
	va_list args;
	va_start(args, format);
	vswprintf_s(log, format, args);
	OutputDebugStringW(log);
	va_end(args);
}


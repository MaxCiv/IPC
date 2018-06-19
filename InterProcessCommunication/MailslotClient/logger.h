#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdarg.h>
#include <time.h>

// Полное название файла
FILE* logfile;

// Прототипы функций
void initlog(const _TCHAR* prog);
void closelog();
void writelog(_TCHAR* format, ...);

// Инициализировать файл для лога
void initlog(const _TCHAR* prog) {
    _TCHAR logname[255];
    _TCHAR newext[10];
    wcscpy_s(logname, prog);
    // добавляем к названию лога ID процесса
    swprintf_s(logname, _T("%s.%d.log"), logname, GetCurrentProcessId());
	// пытаемся открыть файл
	if (_wfopen_s(&logfile, logname, _T("a+"))) {
		_wperror(_T("The following error occurred"));
		_tprintf(_T("Can't open log file %s\n"), logname);
		exit(-1);
	}
    fwprintf(logfile, _T("\n--------------------------------------------------\n"));
	writelog(_T("%s is starting."), prog);
}

// Закрыть файл лога
void closelog() {
	writelog(_T("Shutting down."));
	fclose(logfile);
}

// Записать в лог
void writelog(_TCHAR* format, ...) {
	_TCHAR buf[255];
	va_list ap;
	struct tm newtime;
	__time64_t long_time;
	// получить время как 64-bit integer
	_time64(&long_time);
	// конвертировать в локальный вид
	_localtime64_s(&newtime, &long_time);
	// конвертировать в нормальное отображение
	swprintf_s(buf, _T("[%02d/%02d/%04d %02d:%02d:%02d] "), newtime.tm_mday,
		newtime.tm_mon + 1, newtime.tm_year + 1900, newtime.tm_hour,
		newtime.tm_min, newtime.tm_sec);
	// записать дату и время
	fwprintf(logfile, _T("%s"), buf);
	// записать все параметры
	va_start(ap, format);
	_vsnwprintf_s(buf, sizeof(buf) - 1, format, ap);
	fwprintf(logfile, _T("%s"), buf);
	va_end(ap);
	// добавить запись в лог
	fwprintf(logfile, _T("\n"));
}
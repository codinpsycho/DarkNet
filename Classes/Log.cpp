#include <stdarg.h>
#include "Log.h"

#if defined(WIN32)
	#define VC_EXTRALEAN
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

Log::Log(void)
{
	AttachFile(DEFAULT_LOGFILE);
	ClearFile();
}

void Log::ClearFile()
{
	FILE *file = fopen(m_fileName, "w");
	if (file)
	{
		fclose(file);
	}	
}

Log::~Log(void)
{
	Write("Self Destructing now ");
}


void Log::AttachFile( char *fileName )
{
	strcpy_s(m_fileName, sizeof(m_fileName), fileName);
}

int Log::Write(const char* format,...)
{	
  int written = 0;
  FILE *file = fopen(m_fileName,"a");
  if(file)
  {
    va_list list;
	char buff[LOG_BUFF_SIZE];
    size_t size = sizeof(buff);
    memset(buff,sizeof(buff),0);				
    va_start(list, format);					
    vsnprintf(buff, size - 2, format, list );	
    va_end(list);

	buff[size - 2] = '\n';
	buff[size - 1] = '\0';

    written  = fputs(buff, file);
    fclose(file);
  }	
  return written;
}

//General printf like function used to print into output window
void OUTPUT(char *format_, ...)
{
  va_list list;
  char buff[LOG_BUFF_SIZE];
  size_t size = sizeof(buff);
  memset(buff,sizeof(buff),0);					

  va_start(list, format_);						
  vsnprintf(buff, size - 2, format_, list );	
  va_end(list);									

  buff[size - 2] = '\n';
  buff[size - 1] = '\0';  

  OutputDebugString(buff);
}


#include <stdarg.h>
#if defined(WIN32)
#include <Windows.h>
#endif
#include "Log.h"

Log::Log(void)
{
  AttachFile("Log.txt");  
}


Log::~Log(void)
{
	Write("Self Destructing now ");
}


void Log::AttachFile( char *fileName )
{
  strcpy(m_fileName, fileName);	
  //Open it once to make it Null
  FILE *file = fopen(fileName,"w");
  if(file)
    fclose(file);
}

int Log::Write(const char* format,...)
{	
  int written = 0;
  FILE *file = fopen(m_fileName,"a");
  if(file)
  {
    va_list list;
    char buff[1024];							//Buffer to hold final output
    size_t size = sizeof(buff);
    memset(buff,sizeof(buff),0);				//0 out the memory
    va_start(list, format);					//Start iterating
    vsnprintf(buff, size - 2, format, list );	//Print into Buffer
    va_end(list);								//Free the memory used by list

    written  = fputs(buff, file);
    fclose(file);
  }	
  return written;
}

//General printf like function used to print into output window
void OUTPUT(char *format_, ...)
{
  va_list list;
  char buff[2048];							//Buffer to hold final output
  size_t size = sizeof(buff);
  memset(buff,sizeof(buff),0);				//0 out the memory

  va_start(list, format_);					//Start iterating
  vsnprintf(buff, size - 3, format_, list );	//Print into Buffer
  va_end(list);								//Free the memory used by list

  buff[size - 1]	= '\0';							//To keep it null terminated
  buff[size - 2]	= '\n';						//To keep it in Human readable form

  OutputDebugString(buff);					//Only works in Debug mode :(
}


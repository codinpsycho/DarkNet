#pragma once
#include <fstream>
#include <assert.h>


class Log
{
	
public:
	static Log& Instance()
	{
		static Log obj;
		return obj;
	}
	
	void AttachFile(char *fileName);
	int operator<<(const char *data )
	{
		int written = 0;
		FILE *file = fopen_s(m_fileName,"a");
		if(file)
		{
			written = fputs(data, file);
			fclose(file);
	}
	return written;
	}
	int Write(const char* data,...);	
	virtual ~Log(void);

private:
	Log(void);
	Log(const Log& rhs);
	Log operator=(Log& rhs);
	
	//Data Members
  char m_fileName[256];	
};

void OUTPUT(char *format_, ...);
#define LOG         Log::Instance()
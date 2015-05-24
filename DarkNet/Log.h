#pragma once
#include <fstream>

#define DEFAULT_LOGFILE "log.txt"
#define LOG_BUFF_SIZE 1024

class Log
{
	
public:
	static Log& Instance()
	{
		static Log obj;
		return obj;
	}
	
	void	AttachFile(char *fileName);
	void	ClearFile();
	int		Write(const char* data, ...);

	int operator<<(const char *data)
	{
		int written = 0;
		FILE *file = fopen(m_fileName,"a");
		if(file)
		{
			written = fputs(data, file);
			fclose(file);
		}
		return written;
	}	
	virtual ~Log(void);

private:
	Log(void);
	Log(const Log& rhs);
	Log operator=(Log& rhs);
	char m_fileName[256];
};

void OUTPUT(char *format_, ...);
#define LOG         Log::Instance()
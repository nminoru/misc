#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
static LARGE_INTEGER _tstart, _tend;
static LARGE_INTEGER freq;

void tstart(void)
{
	static int first = 1;

	if(first) {
		QueryPerformanceFrequency(&freq);
		first = 0;
	}
	QueryPerformanceCounter(&_tstart);
}
void tend(void)
{
	QueryPerformanceCounter(&_tend);
}

double tval()
{
	return ((double)_tend.QuadPart -
				(double)_tstart.QuadPart)/((double)freq.QuadPart);
}
#else
#include <sys/time.h>
#include <unistd.h>
static struct timeval _tstart, _tend;
static struct timezone tz;

void tstart(void)
{
	gettimeofday(&_tstart, &tz);
}
void tend(void)
{
	gettimeofday(&_tend,&tz);
}

double tval()
{
	double t1, t2;

	t1 =  (double)_tstart.tv_sec + (double)_tstart.tv_usec/(1000*1000);
	t2 =  (double)_tend.tv_sec + (double)_tend.tv_usec/(1000*1000);
	return t2-t1;
}
#endif
#ifdef _WIN32
#include <windows.h>
#define SLASHC		'\\'
#define SLASHSTR	"\\"
#else
#include <sys/utsname.h>
#define SLASHC		'/'
#define SLASHSTR	"/"
#endif
#include <stdio.h>

char *ver()
{

#ifdef _WIN32
	static char verbuf[256];
	OSVERSIONINFO VersionInfo;

	VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
	if(GetVersionEx(&VersionInfo)) {
		if(strlen(VersionInfo.szCSDVersion) > 200)
			VersionInfo.szCSDVersion[100] = 0;
		sprintf(verbuf, "Windows %d.%d build%d PlatformId %d SP=\"%s\"",
			VersionInfo.dwMajorVersion,
			VersionInfo.dwMinorVersion,
			VersionInfo.dwBuildNumber,
			VersionInfo.dwPlatformId,
			VersionInfo.szCSDVersion);
		return verbuf;
	}
	else {
		return "WINDOWS UNKNOWN";
	}
#else
	struct utsname ubuf;
	static char verbuf[4*SYS_NMLN + 4];

	if(uname(&ubuf))
		return "LINUX UNKNOWN";
	sprintf(verbuf,"%s %s %s %s",
		ubuf.sysname,
		ubuf.release,
		ubuf.version,
		ubuf.machine);
	return verbuf;
#endif
}

#include <ctype.h>

size_t atoik(char *s)
{
	size_t ret = 0;
	size_t base;

	if(*s == '0') {
		base = 8;
		if(*++s == 'x' || *s == 'X') {
			base = 16;
			s++;
		}
	}
	else
		base = 10;

	for(; isxdigit(*s); s++) {
		if(base == 16)
			if(isalpha(*s))
				ret = base*ret + (toupper(*s) - 'A');
			else
				ret = base*ret + (*s - '0');
		else if(isdigit(*s))
				ret = base*ret + (*s - '0');
		else
			break;
	}
	for(; isalpha(*s); s++) {
		switch(toupper(*s)) {
		case 'K': ret *= 1024; break;
		case 'M': ret *= 1024*1024; break;
		default:
			return ret;
		}
	}
	return ret;
}

char *applname;

int main(int ac, char *av[])
{
	long count = 100000;
	long i;
	double t;
	char *v = ver();
	char *q;

	applname = av[0];
	if(strrchr(applname,SLASHC))
		applname = strrchr(applname,SLASHC) + 1;

	if(ac > 1) {
		count = atoik(av[1]);
		ac--;
		av++;
		if(count < 0)
			count = 100000;
	}
	tstart();
	for(i = 0; i < count; i++)
		tend();
	tend();
	t = tval();
	printf("%s: ",applname);
	printf("%d calls to tend() = %8.3f seconds %8.3f usec/call\n",
		count,
		t,
		(t/( (double) count ))*1E6);

	return 0;
}

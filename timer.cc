// #include <iostream>
// using namespace std
//
#if defined(__GNUC__)
	#include <sys/time.h>
#elif  defined(_WIN32)
	#include <time.h>
	#include <windows.h>
 
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};

/*
 * Structure used in select() call, taken from the BSD file sys/time.h.
 */
struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};


int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag = 0;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }

  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}

#endif

class Timer {
	private:
		struct timeval tv;
		struct timezone tz;
		long long start_time,cur_time;
		inline long long gettod(void){
			if ( gettimeofday(&tv,&tz) ){
//				cerr<<"Error: gettimeofday()"<<endl;
//				exit(-1);
			}
			return (long long) tv.tv_usec + (long long) tv.tv_sec * 1000000;
		}
	public:
		inline Timer(){ start(); };
		inline void start(void){ start_time=gettod(); }
		inline long long getusec(void){ return gettod() - start_time; }
		inline double getsec(void){ return (double) getusec() / 1000000; }

};



#ifndef ACE_TIMER_H_
#define ACE_TIMER_H_

#include <list>
#include <chrono>
#include <ctime>

#define ace_elapsed Timer::getElapsed

namespace ace {
	
	class Timer {
	public:
		typedef double type_t; //double or float
		typedef std::chrono::high_resolution_clock tclock;
		typedef std::chrono::microseconds microseconds;
		typedef std::chrono::milliseconds milliseconds;
		typedef tclock::time_point time_point_t;
		typedef tclock::duration   duration_t;
	public:
		Timer(type_t limit = 1.0);
		Timer(const Timer & rhs);
		~Timer();
		void operator = (const Timer & rhs);
		
		bool expired ();
		void start   ();
		void pause   ();
		void resume  ();
		void reset   ();
		void restart ();
		void   setTime  (type_t time);
		void   setLimit (type_t limit);
		type_t getTime  () {return m_Time;}
		type_t getLimit () {return m_Limit;}
		
		static void   init                  ();                    //initialises time
		static void   updateAll             ();                    //should be called once a frame
		static void   updateAll             (type_t elapsedTime);  //explicitly updates all timers with elapsedTime
		static type_t getElapsed            () {return s_Elapsed;} //returns the value which was last given to updateAll()
		static type_t getTimeSinceInit      ();                    //returns the time since init
	private:
		void update  (type_t elapsedTime); //call updateAll() instead, this is private
		
		type_t m_Time;
		type_t m_Limit;

		bool   m_bPaused;
		
		static std::list<Timer*> s_lpTimer;  //pointers to all timer instances
		static type_t            s_Elapsed;  //time since last frame
		static duration_t        s_Duration; //same as elapsed but with chrono-type
		static time_point_t      s_LastTimePoint; //last time_point of updating
		static time_point_t      s_FirstTimePoint; //first time_point in init
	};
	
}

#endif /*ACETIMER_H_*/

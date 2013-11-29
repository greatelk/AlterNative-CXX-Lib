#include "Thread.h"
#include "../Exception/SystemException/NotImplementedException.h"


//http://antonym.org/2009/05/threading-with-boost---part-i-creating-threads.html

namespace System{
	namespace Threading{

		ThreadStart::ThreadStart()
		{
		}

		ThreadStart::ThreadStart(boost::function<void ()> target)
		{
			this->functor = target;
		}

		ThreadStart::operator boost::function<void ()>()
		{
			return this->functor;
		}

		ParameterizedThreadStart::ParameterizedThreadStart()
		{
		}

		ParameterizedThreadStart::ParameterizedThreadStart(boost::function<void (System::Object*)> target)
		{
			this->functor = target;
		}

		ParameterizedThreadStart::operator boost::function<void (System::Object*)>()
		{
			return this->functor;
		}

		Thread::Thread(ThreadStart* target)
		{			
			functor = *target;
		}

		void Thread::Sleep(int milliseconds)
		{
			boost::posix_time::milliseconds sleepTime(milliseconds);
			boost::this_thread::sleep(sleepTime);
		}

		void Thread::Sleep(TimeSpan* span)
		{
			boost::posix_time::milliseconds sleepTime(span->TotalMilliseconds);
			boost::this_thread::sleep(sleepTime);
		}

		void Thread::Start()
		{	
			workerThread = boost::thread(this->functor.functor);
		}

		void Thread::Join()
		{
			workerThread.join();
		}

		void Thread::Abort()
		{
			workerThread.interrupt();
		}
	}
}
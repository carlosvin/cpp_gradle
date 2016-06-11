#pragma once

#ifdef _MSC_VER
	#ifdef HELLO_EXPORT
		#define HELLO_API __declspec(dllexport)
	#else
		#define HELLO_API __declspec(dllimport)
	#endif
#else
	#define HELLO_API
#endif

class HELLO_API Hello
{
	private:
		const char * who;

	public:
		Hello(const char * who);

		void sayHello(unsigned n = 1);

};

#include <iostream>
#include <string>
#include <cstdint>

#include "tr_timer/time_wheel.h"


static void test_time_wheel()
{
	// TODO:
}



int main(int argc, char* argv[])
{
 	std::cout << "hello test" << std::endl;
	int64_t a = 10;
	auto test_fun = [b{a}](){
	  std::cout << "b = " << b << std::endl;
	};
	test_fun();
	return 0;
}

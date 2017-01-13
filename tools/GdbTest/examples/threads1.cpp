#include <iostream>
#include <thread>
 
void foo() 
{
	std::cout << "foo()\n";
}

void bar(int x)
{
	std::cout << "bar(" << x << ')';
	std::cout << std::endl;
}

int main() 
{
  std::thread first (foo);
  std::thread second (bar, 13);

  std::cout << "main, foo and bar now execute concurrently...\n";

  // synchronize threads:
  // pauses until first and second finish
  first.join();
  second.join();

  std::cout << "foo and bar completed.\n";
  return 0;
}

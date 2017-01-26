#include <iostream>
#include <thread>

int run(int tr)
{
	std::cout << "thread" << tr;
	std::cout << std::endl;
	return 0;
}

int main()
{
	const int n = 20;
	std::thread threads[n];

	for (int i = 0; i < n; ++i)
		threads[i] = std::thread(run, i);

	for (int i = 0; i < n; ++i)
		threads[i].join();

	return 0;
}

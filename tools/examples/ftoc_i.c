#include <stdio.h>

int main()
{
	double fahrenheit = 0, celsius = 0;
	printf("Temperature (Fahrenheit): ");
	if ( scanf("%lf", &fahrenheit) == 1 )
	{
		celsius = 5 / 9 * (fahrenheit - 32);
		printf("%lg Fahrenheit == %lg Celsius\n", fahrenheit, celsius);
	}
	return 0;
}

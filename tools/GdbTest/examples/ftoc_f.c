#include <stdio.h>

int main()
{
	double fahrenheit = 0.0, celsius = 0.0;
	printf("Temperature (Fahrenheit): ");
	scanf("%lf", &fahrenheit);
	celsius = 5.0 / 9.0 * (fahrenheit - 32.0);
	printf("%lg Fahrenheit == %lg Celsius\n", fahrenheit, celsius);
	return 0;
}

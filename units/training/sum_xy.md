# Sumador simple

Este chip era un simple sumador de enteros. Nada sofisticado. Quizá por eso poco fue afectado por el atacante. Todo el código lo encontramos legible menos una línea. Lo que logramos recuperar está cargado en el editor de código, y nos sirve, de paso, para entender cómo funciona un chip.

Un chip es una pieza de procesamiento. Es decir, hace una tarea. Este chip en particular es un sumador de enteros, y su tarea es sumar parejas de números.

Los chips no trabajan en solitario, sino que son parte de un gran sistema de chips interconectados. Los chips se comunican unos con otros por tubos neumáticos. Un chip recibe datos de otros chips a través de un tubo de entrada, conocido como *entrada estándar* (`std::cin` en C++). El chip hace su tarea y envía el resultado a otros chips a través del tubo de *salida estándar* (`std::cout` en C++).

Algunos chips, como este sumador de enteros, se mantienen encendidos procesando largas secuencias de entrada, y se apagan sólo hasta que no haya más entrada. Hay una razón para ello, y es ahorro de energía. Encender y apagar un chip requiere recursos. Por eso el sistema espera que el sumador de enteros no sólo reciba una única pareja de enteros, sino muchas parejas de enteros. Por eso la lectura y procesamiento debe hacerse en un ciclo, como el siguiente:

	long a, b;
	while ( std::cin >> a >> b )
	{
	
	}

El sistema avisa al chip que ya se ha terminado la entrada y que puede apagarse, enviándole un dato especial, conocido en C++ como `EOF` (end-of-file). Cuando `std::sin` ve este dato `EOF`, rompe el ciclo, el programa termina y el chip se apaga.

Ahora sólo falta recuperar la línea que imprime la suma de los dos números leídos! En la descripción de cada unidad verá las secciones de abajo. La sección *Entrada* provee ejemplos de datos que recibirá el programa, y la sección *Salida* su correspondiente resultado.

## Entrada

Debe recibir parejas de dos enteros separadas por un espacio en blanco. Las parejas se separan entre ellas por cambios de línea. Los enteros son mayores a un millón negativo y menores a un millón positivo.

	2 5
	-4 7
	100 -0
	-80 -120
	

## Salida

Debe imprimir el resultado de la suma de cada pareja. Un resultado por línea.

	7
	3
	100
	-200

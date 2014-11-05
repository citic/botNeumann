# bot Neumann: Documento de diseño

![Logotipo del juego]()

bot Neumann es un videojuego serio para el aprendizaje de conceptos abstractos de un lenguaje de programación, en particular administración de memoria y concurrencia en C++.

## Ficha técnica

* *Plataformas*: Linux, OS X, Windows, Android, iOS

* *Audiencia*: Aprendices de programación en C++, que ya tienen algún dominio del lenguaje y necesitan comprender conceptos abstractos, en especial, administración de memoria y concurrencia.

* *Características distintivas*: Único videojuego cuyos elementos de juego representan conceptos abstractos de programación. Los objetos del juego son objetos de programación (robot = hilo de ejecución, área de trabajo = memoria de pila, espacio exterior = memoria dinámica, etc.). Permite retar a programadores contra los ejercicios de los concursos de programación de la ACM.

* *Productos relacionados*: CodeCombat, CodeSpells, RoboCode.


# Relato (story)

Año 3000. Tras monumentales retos ambientales y sociales, la tecnología de avanzada ha permitido la conquista espacial. Complejas super computadoras controlan las abundantes estaciones espaciales. Sin embargo, un mega ataque informático ha destruido millones de chips. La vida y el orden en las estaciones espaciales donde reside el 80% de la raza humana está en peligro. El sobredimensionamiento de la emergencia es tal, que se ha hecho un llamado a todos los programadores de la humanidad para retornar las super computadoras a su estado operacional, e incrementar sus niveles de seguridad. Se encontró que el ataque fue posible en algunos chips debido a que se implementaron bajo prácticas de programación no seguras.

Las avanzadas super computadoras se construyeron como complejas organizaciones de chips genéricos. Todos los chips son iguales entre sí. Un chip puede ser programado para realizar una única función. Los chips inicialmente están vacíos. Mediante pruebas automáticas por software es fácil encontrar cuáles chips están defectuosos. Gracias a un catálogo se tiene una descripción de lo que debe realizar cada chip. Son tantos que la Agencia Internacional de Cooperación Espacial (AICE o ICSA) tiene el honor y la urgencia de invitar a todos los programadores, como usted, para la delicada labor de sustituir los chips afectados con nuevos, pero programados con técnicas más seguras. La AICE ofrece un plan de capacitación y generosas recompensas para los héroes que se comprometan con la causa.


# Escena: Menú principal

Además de PC, el juego correrá en dispositivos móviles, por lo que mostrará el menú principal tan rápido como pueda. No se mostrarán animaciones. Quizá un splash screen únicamente con el logotipo del juego. El menú principal tendrá las siguientes secciones:

1. **Sección Jugador** (`PlayerSection`). Mostrará en un área reducida, detalles del jugador actual: su nombre, número de chips reparados y su nivel de pericia. Al presionar esta árae mostrará el diálogo de administración de jugadores (`PlayerManagement`).

2. **Sección trofeos** (`ArchievementSection`). Los jugadores pueden obtener trofeos cuando sus soluciones exceden las expectativas {eficiencia, buenas prácticas}, cuando reparan cantidades considerables de chips, u otras circunstancias {listarlas todas}. La sección trofeos muestra un estante con espacio para todos los trofeos que dispone el juego y aquellos que el jugador haya acumulado. Al accionar sobre un trofeo o su espacio, se muestra información de cómo se puede obtener o cuando se consiguió. Los trofeos incrementan el nivel de pericia del jugador.

3. **Sección configuración** (`GameConfig`). Permite al jugador configurar preferencias como métodos para entrada de código, intensidad de los efectos de sonido y la música. Invoca al diálogo de Configuración.

4. **Sección modos de juego** (`GameModes`). Ocupa parte prominente la pantalla del menú. Sus opciones corresponden con las modalidades de juego disponibles: Cada una se explica más adelante.

	- Entrenamiento/Training
	- Misión/Mission
	- Colaboración/Collaboration
	- Construcción/Building

5. **Sección créditos** (`CreditsSection`). Muestra información sobre el juego (versión, sitio web), los autores, la investigación subyacente, licencias, y reconocimiento de terceros.



## Diálogo: Administración jugadores (`PlayerManagement`)

Consta de tres partes:

1. Información sobre el jugador actual: nickname, número de chips reparados, pericia.
2. Otros jugadores registrados con el dispositivo, que al accionarlos permite cambiar de jugador. Una opción permite agregar nuevos jugadores.
3. Otros jugadores del mundo: los mejores marcadores, los marcadores de los amigos (Facebook?).

El juego registra la pericia del jugador. Es un indicador (superficial, no científicamente fundamentado) del nivel de conocimiento de programación -o al menos empeño- del jugador. Los retos se organizan por niveles, de tal forma que todos los de un mismo nivel requieren la aplicación del mismo concepto en formas diferentes. Un cambio de nivel implica un cambio de concepto. La pericia del jugador incrementa conforme el jugador avanza por el juego. Se incrementa en 1 cada vez que repara un chip. Se incrementa en 10 cada vez que se pasa de nivel. Una escala asigna descripciones a ciertos hitos:

	   0  Novato
	 100  Principiante
	 500  Intermedio
	1000  Avanzado
	5000  Experto


## Diálogo: Trofeos



## Diálogo: Configuración del juego (`GameConfig`)

Diálogo que puede ser llamado desde el menú del juego o desde cualquier nivel. Es un diálogo porque no reemplaza la escena, sólo se sobrepone a ella.




# Entrenamiento/Training

Aunque existen muchos programadores en la humanidad, no todos saben cómo programar los particulares chips que requieren las super computadoras. La AICE ha preparado, entonces, una secuencia programas que deben ser implementados en chips. Los programas están ordenados desde el más básico hasta el más complejo.

Al ingresar a la sección de entrenamiento, el programador verá la secuencia de chips. Cada chip está identificado, utilizando la notación `Txxxyyy` donde `T` indica "Training", `xxx` es un número de nivel y `yyy` es el número de problema de ese nivel. Cada nivel expone un concepto de programación diferente.

1. Entrada/salida, variables, expresiones aritméticas
2. .

## 1. Entrada/salida, variables, expresiones

El entrenamiento para el primer chip solicita crear un programa que lea repetitivamente dos números e imprima su sumatoria. El juego muestra el ambiente con que debe interaccionar el chip. El chip está conectado a tres tubos. Uno de entrada llamado `cin` donde llegan valores con un orden conocido de antemano. Dos de salida, uno llamado `scout` donde debe imprimirse resultados de la operación correcta y otro llamado `cerr` para reportar errores si la entrada resultara no válida.

Los chips son suficientemente sofisticados y tienen un robot dentro, aunque pueden tener más. El robot (execution thread) sigue las instrucciones que se le programen. El robot comprende un lenguaje llamado C++.


# Misión / Mission

# Colaboración / Collaboration

# Construction / Building


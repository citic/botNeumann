## assets


| image | description | preview |
|---|---|---|
| game_logo | logotipo del juego para iconos | ![game_logo](game_logo.svg) |
| game_title | nombre del juego para pantalla principal | ![game_title](game_title.svg) |
| button_back | retornar a la escena anterior | ![button_back](button_back.svg) |
| button_code_editor | show/hide code editor | ![button_code_editor](button_code_editor.svg) |
| button_config | player configuration/preferences | ![button_config](button_config.svg) |
| button_information | botón de información. | ![button_information](button_information.svg) |
| button_rewards | rewards | ![button_rewards](button_rewards.svg) |


## game_menu:
| image | description | preview |
|---|---|---|
| background | fondo para la pantalla de inicio/menú. | ![background](game_menu/background.svg) |


## unit_selection

| image | description | preview |
|---|---|---|
| background | fondo tipo tarjeta con diferenciación de la barra de menú | ![background](unit_selection/background.svg) |
| chip_restored | chip "bueno" | ![chip_restored](unit_selection/chip_restored.svg) |
| chip_damaged | chip "dañado" | ![chip_damaged](unit_selection/chip_damaged.svg) |
| air_tube_short | segmento de tubo transparente para conectar los chips entre ellos. | ![air_tube_short](unit_selection/air_tube_short.svg) |
| air_tube_pod | cápsula que pasa dentro de los tubos entre los chips para hacer animación. | ![air_tube_pod](unit_selection/air_tube_pod.svg) |

## unit_playing

| image | description | preview |
|---|---|---|
| background | fondo del laboratorio | ![background](unit_playing/background.svg) |
| shelves_closed | Heap segment hidden to avoid cognitive overload at first levels. | ![shelves_closed](unit_playing/shelves_closed.svg) |
| shelf_left | extremo izquierdo del estante. | ![shelf_left](unit_playing/shelf_left.svg) |
| shelf_middle | parte interna/del medio del estante. Se repite en mosaico para generar el ancho del estante. | ![shelf_middle](unit_playing/shelf_middle.svg) |
| shelf_right | parte derecha del estante. | ![shelf_right](unit_playing/shelf_right.svg) |
| robot1_front | robot 1 mirando hacia el jugador, para tomar datos del segmento de datos o los tubos entrada/salida | ![robot1_front](unit_playing/robot1_front.svg) |
| robot1_side | robot 1 mirando de lado para hacer animación de giro | ![robot1_side](unit_playing/robot1_side.svg) |
| robot1_back | vista trasera del robot 1: posición de trabajo normal | ![robot1_back](unit_playing/robot1_back.svg) |
| robot2_front | robot 2 mirando hacia el jugador, para tomar datos del segmento de datos o los tubos entrada/salida | ![robot2_front](unit_playing/robot2_front.svg) |
| robot2_side | robot 2 mirando de lado para hacer animación de giro | ![robot2_side](unit_playing/robot2_side.svg) |
| robot2_back | vista trasera del robot 2: posición de trabajo normal | ![robot2_back](unit_playing/robot2_back.svg) |
| robot3_front | robot 3 mirando hacia el jugador, para tomar datos del segmento de datos o los tubos entrada/salida | ![robot3_front](unit_playing/robot3_front.svg) |
| robot3_side | robot 3 mirando de lado para hacer animación de giro | ![robot3_side](unit_playing/robot3_side.svg) |
| robot3_back | vista trasera del robot 3: posición de trabajo normal | ![robot3_back](unit_playing/robot3_back.svg) |
| robot4_front | robot 4 mirando hacia el jugador, para tomar datos del segmento de datos o los tubos entrada/salida | ![robot4_front](unit_playing/robot4_front.svg) |
| robot4_side | robot 4 mirando de lado para hacer animación de giro | ![robot4_side](unit_playing/robot4_side.svg) |
| robot4_back | vista trasera del robot 4: posición de trabajo normal | ![robot4_back](unit_playing/robot4_back.svg) |
| wave | ancho igual al de 1 char | ![wave](unit_playing/wave.svg) |
| shelf_interface | los controles para acceder a los estantes (embudo) | ![shelf_interface](unit_playing/shelf_interface.svg) |
| workstation_table | mesa del robot, pueden haber varias una encima de la otra | ![workstation_table](unit_playing/workstation_table.svg) |
| workstation_frame | el resto de la estación de trabajo del robot. | ![workstation_frame](unit_playing/workstation_frame.svg) |
| shared_table | mesa compartida por todos los robots: segmento de datos. | ![shared_table: mesa compartida por todos los robots](unit_playing/shared_table.svg) |
| tube_in | tubo de entrada, hacerle una sección abierta de 8 bytes. | ![tube_in](unit_playing/tube_in.svg) |
| tube_out | tubo de salida: también requiere la sección abierta. | ![tube_out: tubo de salida](unit_playing/tube_out.svg) |


## unit_playing / buttons

| image | description | preview |
|---|---|---|
| pause | pauses the visualisation to allow user to run it step by step | ![pause](unit_playing/button_pause.svg) |
| run | compiles the code and starts the visualisation | ![run](unit_playing/button_run.svg) |
| button_step_into | if visualisation is paused, executes the next code statement entering in functions if they are defined by user | ![button_step_into](unit_playing/button_step_into.svg) |
| button_step_out | if visualisation is paused, executes the next code statement in the current function | ![button_step_out](unit_playing/button_step_out.svg) |
| button_stop | stops the visualisation | ![button_stop](unit_playing/button_stop.svg) |


## unit_playing / data_types

| image | description | preview |
|---|---|---|
| address_0 | direccion de memoria 0, gris, del mismo color que pointer_x_null; es un espacio reservado, donde no se puede alojar nada | ![address_0](unit_playing/data_types/address_0.svg) |
| bool_false | representado con un círculo vacío `( )` de 1 byte | ![bool_false](unit_playing/data_types/bool_false.svg) |
| bool_right | representado con un círculo marcado `(✓)`, preferiblemente con una marca de verificación ✓ | ![bool_right](unit_playing/data_types/bool_right.svg) |
| char | romboidal de 1 byte: `/_/`, permite fácilmente hacer cadenas: `/_/_/_/_/` | ![char](unit_playing/data_types/char.svg) |
| int_left | rectángulo redondeado, extremo izquierdo | ![int_left](unit_playing/data_types/int_left.svg) |
| int_middle | rectángulo redondeado, extremo central que puede prolongarse a varios bytes | ![int_midldle](unit_playing/data_types/int_middle.svg) |
| int_right | rectángulo redondeado, extremo derecho | ![int_right](unit_playing/data_types/int_right.svg) |
| pointer_left | es igual que el entero (int) pero de otro color, puede tener un punto de color: verde (OK), rojo (dirección inválida). Ese punto puede estar fuera del rectángulo. Puede ser una especie de antena parabólica pequeñita con esos colores | ![pointer_left](unit_playing/data_types/pointer_left.svg) |
| pointer_left_ok | antena con luz verde (OK) | ![pointer_left_ok](unit_playing/data_types/pointer_left_ok.svg)
| pointer_left_invalid | antena con luz roja (dirección inválida) | ![pointer_left_invalid](unit_playing/data_types/pointer_left_invalid.svg)|
| pointer_middle | extremo central del apuntador, que se puede extender por varios bytes | ![pointer_middle](unit_playing/data_types/pointer_middle.svg) |
| pointer_right | extremo derecho del apuntador | ![pointer_right](unit_playing/data_types/pointer_right.svg) |
| float_left | rectángulo con esquinas cuadradas, extremo izquierdo. Nota ya no es necesario el double, ni el long double | ![float_left](unit_playing/data_types/float_left.svg) |
| float_middle | rectángulo con esquinas cuadradas, extremo central que puede extenderse | ![float_middle](unit_playing/data_types/float_middle.svg) |
| float_right | rectángulo con esquinas cuadradas, extremo derecho | ![float_right](unit_playing/data_types/float_right.svg) |
| struct_left | el tubo para agrupar tipos de datos diferentes (el tubo que contiene otros tubos), debe ser transparente y sus bordes ocupar el menor espacio posible | ![struct_left](unit_playing/data_types/struct_left.svg) |
| struct_middle | parte central del tubo que puede extenderse | ![struct_middle](unit_playing/data_types/struct_middle.svg) |
| struct_right | extremo derecho del tubo | ![struct_right](unit_playing/data_types/struct_right.svg) |
| array_struct_left | unión rosca para pegar dos tubos de struct (left) | ![array_struct_left](unit_playing/data_types/array_struct_left.svg) |
| array_struct_right | unión rosca para pegar dos tubos de struct (right) | ![array_struct_right](unit_playing/data_types/array_struct_right.svg) |
| waste1 | basura, escombro dibujo 1 | ![waste1](unit_playing/data_types/waste1.svg) |
| waste2 | basura, escombro dibujo 2 | ![waste2](unit_playing/data_types/waste2.svg) |
| waste3 | basura, escombro dibujo 3 | ![waste3](unit_playing/data_types/waste3.svg) |

![Game logo](assets/game_title.svg)


**bot Neumann\+\+** is a metaphorized and gamified visualisation of the von Neumann architecture for programming learning. Initially it is intended for memory handling and concurrency in C\+\+. It is developed as part of a research project at the University of Costa Rica. The game elements are represented with more colloquial concepts, such as: robots (execution threads), workstations (stack segments), shared tables (data segment), shelves (dynamic heap segment), pneumatic tubes (input/output streams), and so on.

Learning theories state that learning is unavoidably performed by constructing new concepts through association with previous concepts. Students must learn abstract programming concepts, like memory segments and threads, associating them with... what? These are not concrete concepts; so, students cannot see or touch them to build concrete associations. Students must resort to imagination to have something to associate with. This is a rich scenery for creating wrong and weak associations. Therefore, it is not a surprise learning abstract programming concepts is considered as difficult task for computer science students worldwide.

We hypothesize that representing abstract programming concepts with colloquial concepts help the association process. That is, game elements and the abstract concepts that they represent, share similar properties, behaviors and relationships. Students playing directly with the video game will construct associations and therefore, learn the abstract concepts.

We surveyed our Computer Science students at Universidad de Costa Rica, asking them for the approximate usage percent of Java, C\+\+ and other programming languages that we teach. In average, C\+\+ was reported as the most used programming language. In fact, C\+\+ is a mandatory for computer science students, but it is not intended for programming learning, therefore it normally becomes a daunting experience for students. We also asked for the most difficult and useful topics of C\+\+. They revealed concurrent/parallel programming and memory management have the highest levels of both indicators: difficult to learn and useful.

**bot Neumann\+\+** is a video serious game where main abstract programming concepts of concurrent/parallel programming and memory management in C\+\+ are illustrated by an metaphorized visualisation of the computational machine. In this game, the player (a C\+\+ student) should fix several processing units (chips) that were damaged by an attacker. Their original programs were partially or totally lost. The robots execute the program literally. They only understand C\+\+ (not English)...

**bot Neumann\+\+** is developed by Jeisson Hidalgo-Céspedes, under a research project of [Centro de Investigaciones en Tecnologías de la Información y Comunicación](http://www.citic.ucr.ac.cr/) (CITIC) of [Universidad de Costa Rica](http://www.ucr.ac.cr/). It is licensed under GPL v2. More information:

- Game design document: English | [Spanish](doc/gdd.es.md).
- [API documentation](doc/api/)
- [Project oficial web page at CITIC](http://www.citic.ucr.ac.cr/proyecto/incentivo_lenguaje_programacion_en_estudiantes) (in Spanish)
- Informal document with [Requirements](doc/requirements.md)
- Build instructions can be found in [BUILD.md](BUILD.md) file.

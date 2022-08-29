# Paraordenar
Eina d'emmagatzematge i gestió de traces Paraver.

## Requeriments
Pel que fa a compilació i runtime, l'aplicació necessita un compilador compatible amb la ISO C++20 de la llibreria standard de C++. Les funcionalitats necessàries han estat comprovades correctament amb el compilador `clang+` versió 13 o superior.

Les llibreries de les que depen `paraordenar` son:
- PugiXML: per la gestió de XML. El codi de la llibreria està inclos en el projecte
- CLI11: per la configuració dels arguments d'entrada. El codi de la llibreria està inclos en el projecte.
- Boost.Filesystem: Per accedir i modificar fàcilment el sistema de fitxers
- Boost.Regex: comprovacions d'entrada de l'usuari
- Boost.DateTime: tipus de dates
- fmt: per a la formatació del text de sortida.

## Compilació
L'aplicació es compila amb el sistema de build CMake.  Per fer-ho, crea un directori on construir l'aplicació, i allà crida a cmake passant el camí arrel:
```
mkdir build
cd build
cmake ../
```

A continuacció es pot modificar l'arxiu CMakeCache.txt o utilitzar el programa ccmake per personalitzar els paràmetres de compilació.  Quan s'hagi fet això, es pot compilar fent
```
make
```

Si es vol compilar només la llibreria, es pot fer indicant el target:
```
make ParaordenarCore
```


Finalment, per obtenir la documentació, caldrà tenir instal·lat Doxygen, i utilitzar la crida:
```
make doc
```
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
Per compilar l'aplicació, només fa falta executar `make` amb el target `pro`:
```
make pro
```

Si es vol compilar tot el set de llibreries (estàtica, compartida i dinàmica), es pot fer amb
```
make lib
```

Finalment, per obtenir la documentació, caldrà tenir instal·lat Doxygen, i utilitzar la crida:
```
make doc
```

La crida `all` de `make` executa els tres targets indicats a sobre.
# Text Editor 

Un editor de texto estilo VIM 

### Prerequisitos 

Tener instalado make 

### Construyendo el proyecto 

En el fichero Makefile esta todo lo necesario para el compilado del proyecto 

Corre el comando make en la carpeta del fichero para construir  las binarios correspndiantes para correr el programa 

```
make 
```

### Corriendo el programa 

Una vez que se haga make este te va a crear el fichero editor.o 

para correrlo solo corre el comando siguiente en tu consola 

```
./editor.o
```
### Ejemplo 

Ahora puedes ver el contenido de un fichero pero sin editarlo tadavia , tambien puedes mover el cursor en la pantalla y todo funciona ok , como en un editor normal (se puede mover el texto con las frechar de arriba de abajo y home key y demas funciones ) 

Puedes correr el sigiente ejamplo para poder ver las nuevas funcionalidades 

```
./editor.o editor.c
```

### Nota 

El pryecto esta en desarrollo 
Solo funciona mover el cursor en la pantalla de la terminal 
El objetivo es simular algo parecido al vim (claro con muchas menos funcioanlidades ) 

Puedes tambien cargar un fichero y ver todo su contenido 


## Autor

* **Daniel de la Cruz Prieto** - *Trabajo de desarrollo* - [LINK](https://github.com/dcruzp)


## Agradecimiento 

* A todo aquiel que quiera constribuir en el proyecto 
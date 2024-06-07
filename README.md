Sistemas Operativos II - Laboratorio II IPC
###  Ingeniería en Compitación - FCEFyN - UNC
# Interprocess Communication

## Introducción
Los mecanismos de IPC permiten a los procesos intercambiar información. Hemos visto que se pueden clasificar en dos grandes grupos, los de transferencia de datos (pipe, FIFO, message queue, sockets, etc.) y los de memoria compartida (shared memory, memory mapping, etc.). Todo este conjunto de mecanismos son sumamente útiles en todo proyecto y es lo que motiva el presente trabajo.

## Objetivo
El objetivo del presente trabajo práctico es que el estudiante sea capaz de diseñar e implementar un software que haga uso de los mecanismos de IPC que provee el Sistema Operativo, implementando lo visto en el teórico, práctico y haciendo uso todos los conocimientos adquiridos en Ingeniería de Software y Sistemas Operativos I y los desarollado en el Laboratorio I.

## Desarrollo
### Clientes
- Cliente continuo: Reliza la query **QUERY_CONTINUO** cada un intervalo **INTERVALO** ambos configurados en un *define* al comienzo del archivo C.
```Shell
./bin/cliente_continuo.out localhost <Puerto_IPv4> 
```

- Cliente descarga: Realiza una query que descarga la BDD, lleva como argumentos de ejecucion el socket **UNIX** y el nombre del archivo a descargar.
```Shell
./bin/cliente_descarga.out <Socket_UNIX> <Nombre_descarga>
```

- Cliente query: Realiza una query enviada por el usuario en la CLI.
```Shell
./bin/cliente_query.out localhost <Puerto_IPv4> 
```

### Servidor
El servidor cuenta con una base de datos con 5 conexiones maximas definidas en un array y gestionadas por un semaforo. La base de datos es una SQLite.
```Shell
./bin/server.out <Puerto_IPv4> <Puerto_IPv6> <Socket_UNIX>
```

## Dependencias
La unica dependencia que hace falta es **libsqlite3-dev**. Se instala utilizando el comando:
```Shell
make dependencies
```

## Links / Referencias
- [SQLite](https://www.sqlite.org/quickstart.html)
- [Git Workflow](https://www.atlassian.com/git/tutorials/comparing-workflows)

[workflow]: https://www.atlassian.com/git/tutorials/comparing-workflows "Git Workflow"
[sqlite]: https://www.sqlite.org/quickstart.html


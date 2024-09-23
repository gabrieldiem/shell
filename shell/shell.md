# shell

### Búsqueda en $PATH

#### ¿Cuáles son las diferencias entre la syscall execve(2) y la familia de wrappers proporcionados por la librería estándar de C (libc) exec(3)?

`execve(2)` es una syscall definida en `<unistd.h>` que recibe un path absoluto o relativo al current directory que apunta a un archivo binario ejecutable o un script, además recibe un vector de argumentos `argv` y un vector de variables de entorno con forma `KEY=VALUE`.

Por otro lado, la familia de funciones `exec(3)` de la libc, son wrappers implementados en C de la syscall `execve(2)`, donde su comportamiento es variable dependiendo de cuál wrapper se usó, usualmete denominadas por las letras que le siguen a `exec` y comportamientos que se combinan:

- `p`: tiene en cuenta si se busca el binario a ejecutar en los directorios a donde apunta el `$PATH`, si en lugar de pasarle un vector con los argumentos.

- `e`: permiten elegir qué variables de entorno pasarle al proceso a ejecutar mediante la estructura de strings `KEY=VALUE`. Las que no son de esta variante, le heredan todo el entorno al proceso a ejecutar.

- `v` o `l`: la forma de estructurar el argv a pasarle al objeto a ejecutar, si es un vector (`v`) con su último elemento en NULL, o si se le pasa una lista variádica (`va_list`) (`l`) con los argumentos, siendo el último elemento NULL.

#### ¿Puede la llamada a exec(3) fallar? ¿Cómo se comporta la implementación de la shell en ese caso?\_

Tanto `execve(2)` como los wrappers de la familia `exec(3)` pueden fallar, y al hacerlo retorna a la función invocadora con -1 de valor de retorno y el tipo de error guardado en `errno`. El comportamiento de qué hacer cuando ocurre la falla, está a manos de la función invocadora.

Esta implementación de la shell, en caso de fallar un `exec(3)` (que se ejecuta dentro de un proceso separado, previamente habiendo llamado a fork), llama a la syscall `_exit(2)`, que a diferencia de la syscall `exit(2)`, no llama a las funciones de cleanup `atexit(3)` y `on_exit(3)`, y sí cierra file descriptors abiertos por el proceso. El proceso padre que inició el fork, el cual es la shell misma, deja de estar bloqueado por el `wait(2)` y vuelve a pedir input al usuario.

### Procesos en segundo plano

#### Explicar detalladamente el mecanismo completo utilizado.

Al inicializar la shell (en sh.c), se utiliza `sigaction` para setear el manejo de la señal SIGCHLD con un handler custom. Este último invoca `waitpid` solamente para aquellos procesos hijos que se ejecutan en segundo plano mediante el process group id. Además, dicho wait se realiza con un flag, `WNOHANG`, que transforma a la operación en no bloqueante, permitiendo así que la shell siga recibiendo comandos del usuario. También imprime por pantalla el pid del proceso finalizado y su estado.

Luego, en cada iteración del ciclo que ejecuta cada comando (runcmd) se verifica si el comando actual es de primer y segundo plano. Para aquellos en primero plano se realiza un wait e imprime el estado, mientras que para los de segundo plano se imprime la información correspondiente con `print_back_info` y no se realiza ningún wait. Esto se debe a que el wait para dichos procesos será aquel implementado en el handler de la shell, como se mencionó anteriormente. De no ser así, la shell se bloquearía esperando a que termine y no se trataría de un proceso en segundo plano.

Los comandos marcados como "BACK" (background process) funcionan correctamente ya sea que posean una redirección de i/o o no, ya que dicha información es verificada con el parámetro "c" del struct backcmd, y se ejecutan las mismas funciones de los casos EXEC y REDIR respectivamente.

Debido a que el handler para SIGCHLD se ejecuta en el espacio de usuario, se utilizó un stack alternativo con `malloc` y `sigaltstack` para evitar bugs.

#### ¿Por qué es necesario el uso de señales?

En el caso de procesos en segundo plano, las señales son necesarias para que el shell sepa cuándo un proceso hijo ha terminado sin bloquear su ejecución. En particular, SIGCHLD se usa para notificar al shell cuando un proceso hijo (en segundo plano) termina.
Un handler de SIGCHLD permite manejar la señal de manera asíncrona, llamando a waitpid() con la opción WNOHANG para recolectar el estado del proceso terminado sin interrumpir la ejecución del shell. Esto evita procesos zombis y permite que el shell siga aceptando comandos mientras los procesos en segundo plano corren.

### Flujo estándar

---

### Tuberías múltiples

---

### Variables de entorno temporarias

---

### Pseudo-variables

---

### Comandos built-in

---

### Historial

---

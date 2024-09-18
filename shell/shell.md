# shell

### Búsqueda en $PATH

_Responder: ¿cuáles son las diferencias entre la syscall execve(2) y la familia de wrappers proporcionados por la librería estándar de C (libc) exec(3)?_

execve es una syscall definida en <unistd.h> que recibe un path absoluto o relativo al current directory que apunta a un archivo binario ejecutable o un script, además recibe un vector de argumentos argv y un vector de variables de entorno con forma KEY=VALUE.

Por otro lado, la familia de funciones exec de la libc, son wrappers implementados en C de la syscall execve, donde su comportamiento es variable dependiendo de cuál wrapper se usó, usualmete denominadas por las letras que le siguen a `exec` y comportamientos se combinan:

- p: tiene en cuenta si se busca el binario a ejecutar en los directorios a donde apunta el $PATH, si en lugar de pasarle un vector con los argumentos.

- e: permiten elegir qué variables de entorno pasarle al proceso a ejecutar mediante la estructura de strings KEY=VALUE. Las que no son de esta variante, le heredan todo el entorno al proceso a ejecutar.

- v o l: la forma de estructurar el argv a pasarle al objeto a ejecutar, si es un vector (v) con su último elemento en null, o si se le pasa una lista variádica (va_list) (l) con los argumentos, siendo el último elemento NULL.

_Responder: ¿Puede la llamada a exec(3) fallar? ¿Cómo se comporta la implementación de la shell en ese caso?_

Tanto execve como los wrappers de la familia exec pueden fallar, y al hacerlo retorna a la función invocadora con -1 de valor de retorno y el tipo de error guardado en errno. El comportamiento de qué hacer cuando ocurre la falla, está a manos de la función invocadora.

Esta implementación de la shell, en caso de fallar un exec (que se ejecuta dentro de un proceso separado, previamente habiendo llamado a fork), llama a la syscall \_exit(2), que a diferencia de la syscall exit(2), no llama a las funciones de cleanup atexit(3) y on_exit(3), y sí cierra file descriptors abiertos por el proceso. El proceso padre que inició el fork, el cual es la shell misma, deja de estar bloqueado por el wait y vuelve a pedir input al usuario.

### Procesos en segundo plano

---

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

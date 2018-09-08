# shell

A shell implemented in C.
Executes commands read from the standard input or from a file

## Functionality
* Background execution using '&' (Eg. pwd &)
* Handles multiple pipes (Eg. who | grep foobar | uniq -c | sort -nr | less)
* Redirection (Eg. sort < foo | uniq -c >> bar)
* Built in commands: 'cd' and "exit'

```
make clean
make
./shell
```


`my_shell.c` is a simple shell program that can execute commands with arguments. It is implemented using the system calls `fork()`, `execvp()`, and `wait()`. <br/>
the shell program is able to execute commands with arguments, and it can also execute commands in the background by appending an ampersand `&` at the end of the command.<br/>
use `ps` command to see the list of processes running in the background.<br/>
`exit` command to exit the shell along with it's background processes.<br/>
`cd` command to change the current working directory. <br/>
`ctrl+c` will give signal to foreground process to terminate, but it will not terminate the shell program and background processes. (achieved by creating new process group for background processes.) <br/>

## How to run the program

1. Clone the repository
2. Compile the program using the following command:

```bash
gcc my_shell.c -o my_shell
```

3. Run the program using the following command:

```bash
./my_shell
```

you can run this shell program to run itself. (shell-ception)

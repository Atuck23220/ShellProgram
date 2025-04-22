/*
 * Aaron Tuck
 * CMSC 257 - Computer Systems
 * Date: April 24, 2025
 * Project 3 - Simple Shell
 *
 * This project is a minimal Unix-like shell. It supports running system
 * commands, a set of built-in commands (like cd, help, pid), and handles
 * Ctrl+C to stay alive. You can also launch it with a custom prompt using
 * the -p flag.
 */

 #include <stdbool.h>
 #include "csapp.h"

 #define MAXARGS 128
 static char prompt[MAXLINE] = "sh257> "; // default prompt

 /* Prototypes for built-in command functions */
 static void built_exit(int, char **);
 static void built_pid(int, char **);
 static void built_ppid(int, char **);
 static void built_cd(int, char **);
 static void built_help(int, char **);

 /* Core shell function prototypes */
 void eval(char *cmdline);
 int parseline(char *buf, char **argv);
 int builtin_command(char **argv);

 /* Table to dispatch built-in commands by name */
 typedef void (*cmdfun)(int, char **);
 static struct {
     const char *name;
     cmdfun fn;
 } builtins[] = {
     {"exit", built_exit},
     {"pid", built_pid},
     {"ppid", built_ppid},
     {"cd", built_cd},
     {"help", built_help},
     {NULL, NULL}
 };

 /* Signal handler for SIGINT (Ctrl+C).
  * Instead of quitting, this keeps the shell alive and reprints the prompt. */
 static void sigint_handler(int sig) {
     write(STDOUT_FILENO, "\n", 1);
     write(STDOUT_FILENO, prompt, strlen(prompt));
     fflush(stdout); // pushes prompt out again
 }

 /* Main function — entry point for the shell.
  * If -p <prompt> is given, use that prompt instead of the default.
  * Repeats: print prompt, read command, evaluate command. */
 int main(int argc, char **argv) {
     char cmdline[MAXLINE];

     // Checks if user gave prompt
     if (argc == 3 && strcmp(argv[1], "-p") == 0) {
         snprintf(prompt, sizeof(prompt), "%s> ", argv[2]);
     }

     Signal(SIGINT, sigint_handler);

     while (true) {
         printf("%s", prompt);
         fflush(stdout);
         if (fgets(cmdline, sizeof(cmdline), stdin) == NULL) {
             if (feof(stdin)) break;
             if (ferror(stdin)) {
                 clearerr(stdin); // Clears error if prompt is interrupted
                 continue;
             }
         }

         eval(cmdline);
     }

     return 0;
 }

 /* eval - Handles execution of commands.
  * Checks for built-ins; if not, forks a child process to run the command. */
 void eval(char *cmdline) {
     char buf[MAXLINE];
     char *argv[MAXLINE / 2];
     int bg;
     pid_t pid;
     int status;

     strcpy(buf, cmdline);
     bg = parseline(buf, argv);
     if (argv[0] == NULL) return;

     if (builtin_command(argv)) return;

     pid = Fork();
     if (pid == 0) {
         execvp(argv[0], argv);
         printf("Execution failed (in fork)\n");
         printf("%s: Command not found.\n", argv[0]);
         exit(1);
     }

     if (!bg) {
         if (Waitpid(pid, &status, 0) < 0)
             unix_error("waitpid error");
         if (WIFEXITED(status))
             printf("Process exited with status code %d\n", WEXITSTATUS(status));
     } else {
         printf("%d %s", pid, cmdline);
     }
 }

 /* parseline - Splits the command line into argv array.
  * Also checks for & at the end (for background job — not required but left in). */
 int parseline(char *buf, char **argv) {
     char *delim;
     int argc = 0;
     int bg;

     buf[strlen(buf) - 1] = ' ';
     while (*buf && (*buf == ' '))
         buf++;

     while ((delim = strchr(buf, ' '))) {
         argv[argc++] = buf;
         *delim = '\0';
         buf = delim + 1;
         while (*buf && (*buf == ' '))
             buf++;
     }
     argv[argc] = NULL;

     if (argc == 0) return 0;

     if ((bg = (*argv[argc - 1] == '&')) != 0)
         argv[--argc] = NULL;

     return bg;
 }

 /* builtin_command - Checks if command is a built-in.
  * If yes, runs it and returns 1. Otherwise returns 0. */
 int builtin_command(char **argv) {
     if (!strcmp(argv[0], "&")) return 1;

     for (int i = 0; builtins[i].name; i++) {
         if (!strcmp(argv[0], builtins[i].name)) {
             int argc = 0;
             while (argv[argc]) argc++;
             builtins[i].fn(argc, argv);
             return 1;
         }
     }
     return 0;
 }

 /* built_exit - Terminates the shell. */
 static void built_exit(int argc, char **argv) {
     (void)argc; (void)argv;
     raise(SIGKILL);
 }

 /* built_pid - Prints the process ID of the shell. */
 static void built_pid(int argc, char **argv) {
     (void)argc; (void)argv;
     printf("%d\n", getpid());
 }

 /* built_ppid - Prints the parent process ID of the shell. */
 static void built_ppid(int argc, char **argv) {
     (void)argc; (void)argv;
     printf("%d\n", getppid());
 }

 /* built_cd - Changes the current directory, or prints it if no argument. */
 static void built_cd(int argc, char **argv) {
     char cwd[MAXLINE];
     if (argc == 1) {
         if (getcwd(cwd, sizeof(cwd)))
             printf("%s\n", cwd);
     } else {
         if (chdir(argv[1]) == -1)
             perror("cd");
     }
 }

 /* built_help - Displays a list of built-in commands and usage. */
 static void built_help(int argc, char **argv) {
     (void)argc; (void)argv;
     puts("*********************************************************************");
     puts("");
     puts("A Custom Shell for CMSC 257");
     puts("- Developer: Aaron Tuck");
     puts("");
     puts("Usage:");
     puts("- Run './sh257' to start the shell with default prompt \"sh257>\"");
     puts("- Run './sh257 -p <prompt>' to start the shell with a custom prompt");
     puts("*********************************************************************");
     puts("");
     puts("BUILTIN COMMANDS:");
     puts("- exit   : Exit the shell");
     puts("- pid    : Print the process ID of the shell");
     puts("- ppid   : Print the parent process ID");
     puts("- cd     : Change the current working directory or print it");
     puts("- help   : Display this help message");
     puts("");
     puts("SYSTEM COMMANDS:");
     puts("- Use 'man <command>' to learn about other system commands.");
 }

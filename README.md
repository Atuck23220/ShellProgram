# Simple Shell in C (`sh257`)

## Overview
This project is a minimal Unix-like shell written in C for the CMSC 257 - Computer Systems course.  
It supports executing system commands, several built-in commands, and basic signal handling for a simple, interactive shell environment.

Developed by **Aaron Tuck** — April 2025.

---

## Features
- Run standard system commands (e.g., `ls`, `pwd`, `echo`).
- Built-in commands:
  - `exit` — Exit the shell immediately.
  - `pid` — Display the shell’s process ID.
  - `ppid` — Display the parent process ID.
  - `cd [directory]` — Change the working directory (or display it if no argument is given).
  - `help` — Display available commands and usage instructions.
- Customizable prompt via command-line argument (`-p` option).
- Handles `Ctrl+C` (`SIGINT`) to prevent shell termination and re-display the prompt.
- Background execution support (`&` at end of command).

---

## Usage

### 1. Compile the Shell
```bash
gcc -o sh257 shell_program.c csapp.c

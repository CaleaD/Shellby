# Shellby
University project describing a Linux Shell with external and self-implemented commands, redirection and piping.
The application is made to be used in a Linux environment and be compiled with the commands "gcc Shell.c -o Shell.out -lreadline", "./Shell.out"
-lreadline is used in order to link the GNU readline library, which I used for the history.

I have self-implemented the following commands:

| Command  | Definition |
| ------------- | ------------- |
| wc  | print newline, word, and byte counts for each file  |
| expand  | convert tabs to spaces  |
| env | run a program in a modified environment/print environment variables|
| version | offers author information, versioning information, and other valuable information |
| quit | exits the application |


Besides them, any other external command should run as usual.

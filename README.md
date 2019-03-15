# mush

TO FUTURE STUDENTS: Please do not take this code and upload
it as your own. The professors know you're doing this and it
only hurts you. My program isn't even complete so it would
be silly to copy this.

--

The Minimally Useful Shell. This shell can run all commands
available to it in its given path. The shell also supports
pipelines and features both an interactive and a batch
processing mode.

This shell is limited to a maximum pieline size of 10, a
maximum amount of arguments to a given function of 10, and
a maximum line length of 512. These values can be adjusted in
the "misc.h" header.

Mush also has support for redirection of the input and output
of a file, using ">" and "<" like one would in BASH.

The shell also handles an interrupt signal. If the signal is
generated during the running of a child process, it will
terminate the child but continue running the shell. However,
if no children are being run and the shell receives an
interrupt signal, it will close the shell.

The shell can also be closed by running the built-in "exit"
command.

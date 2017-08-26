#include <sys/inotify.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

static void displayInotifyEvent(struct inotify_event *i, int fd);
int main(int argc, char *argv[])
{
    char *msg = "This is a message";
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    char *p;
    struct inotify_event *event;
    int f_stderr, f_stdout;

        printf("%s\n", msg);

    // Fork (make a orphan process)
    int pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    // kill the parent process
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

   // set umask (don't mask anything)
    umask(0);

    // creation new session
    if (setsid() == -1) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }

    // Change the current working directory to root
    chdir("/");

    // Close stdin, stdout and stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // open the streams
    if ((f_stderr = creat("/tmp/stderr_daemon.log", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
        perror("fopen f_error");
        exit(EXIT_FAILURE);
    }

    if ((f_stdout = creat("/tmp/stdout_daemon.log", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
        dprintf(f_stderr, "fopen f_stdout: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int inotify_fd = inotify_init();
    if (inotify_fd == -1) {
        dprintf(f_stderr, "inotify_init: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

 
    int watch_descriptor = inotify_add_watch(inotify_fd, "/tmp/file_to_watch", IN_ALL_EVENTS);
    if (watch_descriptor == -1) {
        dprintf(f_stderr, "inotify_add_watch: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int bytes_read;
    while (1) {
        bytes_read = read(inotify_fd, buf, BUF_LEN);

        if (bytes_read == -1) {
            dprintf(f_stderr, "read: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (bytes_read == 0) {
            dprintf(f_stderr, "read() from inotify fd returned 0: %s\n", strerror(errno));
        }

        for (p = buf; p < buf + bytes_read; p += sizeof(struct inotify_event) +
                event->len) {
            event = (struct inotify_event *) p;
            displayInotifyEvent(event, f_stdout);
        }
    }
    return 0;
}

static void displayInotifyEvent(struct inotify_event *i, int f_stdout)
{
    dprintf(f_stdout, "      mask = ");
    if (i->mask & IN_ACCESS) {
        dprintf(f_stdout, "IN_ACCESS ");
    }
    if (i->mask & IN_ATTRIB) {
        dprintf(f_stdout, "IN_ATTRIB ");
    }
    if (i->mask & IN_CLOSE_NOWRITE) {
        dprintf(f_stdout, "IN_CLOSE_NOWRITE ");
    }
    if (i->mask & IN_CLOSE_WRITE) {
        dprintf(f_stdout, "IN_CLOSE_WRITE ");
    }
    if (i->mask & IN_CREATE) {
        dprintf(f_stdout, "IN_CREATE ");
    }
    if (i->mask & IN_DELETE) {
        dprintf(f_stdout, "IN_DELETE ");
    }
    if (i->mask & IN_DELETE_SELF) {
        dprintf(f_stdout, "IN_DELETE_SELF ");
    }
    if (i->mask & IN_IGNORED) {
        dprintf(f_stdout, "IN_IGNORED ");
    }
    if (i->mask & IN_ISDIR) {
        dprintf(f_stdout, "IN_ISDIR ");
    }
    if (i->mask & IN_MODIFY) {
        dprintf(f_stdout, "IN_MODIFY ");
    }
    if (i->mask & IN_MOVE_SELF) {
        dprintf(f_stdout, "IN_MOVE_SELF ");
    }
    if (i->mask & IN_MOVED_FROM) {
        dprintf(f_stdout, "IN_MOVED_FROM ");
    }
    if (i->mask & IN_MOVED_TO) {
        dprintf(f_stdout, "IN_MOVED_TO ");
    }
    if (i->mask & IN_OPEN) {
        dprintf(f_stdout, "IN_OPEN ");
    }
    if (i->mask & IN_Q_OVERFLOW) {
        dprintf(f_stdout, "IN_Q_OVERFLOW ");
    }
    if (i->mask & IN_UNMOUNT) {
        dprintf(f_stdout, "IN_UMOUNT ");
    }
    dprintf(f_stdout, "\n");
    if (i->len > 0) {
        dprintf(f_stdout, "        name = %s\n", i->name);
    }
}

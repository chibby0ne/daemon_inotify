/* A daemon that monitors for events in a given file/directory */
/* Copyright © 2017 Antonio Gutierrez */

/* This program is free software; you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation; either version 2 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program; if not, see <http://www.gnu.org/licenses/>. */

#include <sys/inotify.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/* Macros */
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

/* Function declarations */
static void display_inotify_event(struct inotify_event *i, int fd);
static void make_process_daemon(void);
static void print_usage(char *binary_name, int fd);
static char *get_absolute_path(char *path, int fd);
static void check_if_file_exists(char *path, int fd);

/* main function */
int main(int argc, char *argv[])
{
    make_process_daemon();

    // open the streams for logs
    int f_stderr, f_stdout;
    if ((f_stderr = creat("/tmp/stderr_daemon.log", 0644)) == -1) {
        exit(EXIT_FAILURE);
    }

    if ((f_stdout = creat("/tmp/stdout_daemon.log", 0644)) == -1) {
        dprintf(f_stderr, "creat f_stdout: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (argc != 2) {
        print_usage(argv[0], f_stderr);
        exit(EXIT_FAILURE);
    }

    char *filename_abs = get_absolute_path(argv[1], f_stderr);
    check_if_file_exists(filename_abs, f_stderr);

    int inotify_fd = inotify_init();
    if (inotify_fd == -1) {
        dprintf(f_stderr, "inotify_init: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int watch_descriptor = inotify_add_watch(inotify_fd, filename_abs, IN_ALL_EVENTS);
    if (watch_descriptor == -1) {
        dprintf(f_stderr, "inotify_add_watch: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int bytes_read;
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    while (1) {
        bytes_read = read(inotify_fd, buf, BUF_LEN);

        if (bytes_read == -1) {
            dprintf(f_stderr, "read: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (bytes_read == 0) {
            dprintf(f_stderr, "read() from inotify fd returned 0: %s\n", strerror(errno));
        }

        char *p;
        struct inotify_event *event;
        for (p = buf; p < buf + bytes_read; p += sizeof(struct inotify_event) +
                event->len) {
            event = (struct inotify_event *) p;
            display_inotify_event(event, f_stdout);
        }
    }
    return 0;
}

static void display_inotify_event(struct inotify_event *i, int f_stdout)
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

static void make_process_daemon(void)
{
    // Make an orphan process by forking and killing parent process
    int pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

   // set umask (don't mask anything from file mode flags)
    umask(0);

    // create new session ID (make process group leader)
    if (setsid() == -1) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }

    // Change the current working directory to root
    if (chdir("/") == -1) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }

    // Close stdin, stdout and stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

}

static void print_usage(char *binary_name, int fd)
{
    dprintf(fd, "%s FILE\nWhere FILE is a file or a directory", binary_name);
}

static void check_if_file_exists(char *path, int fd)
{
    if (access(path, R_OK) != 0) {
        dprintf(fd, "access: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    dprintf(fd, "file %s does exist\n", path);
}

static char *get_absolute_path(char *path, int fd)
{
   char *full_path = realpath(path, NULL);
   if (full_path == NULL) {
        dprintf(fd, "realpath %s %s\n", strerror(errno), path);
        exit(EXIT_FAILURE);
    }
   dprintf(fd, "full_path: %s\n", full_path);
   return full_path;
}

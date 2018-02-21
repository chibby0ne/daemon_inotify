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

#include "linked_list.h"

#include <sys/inotify.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>

/* Macros */
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

/* Function declarations */
static void display_inotify_event(struct inotify_event *i, int fd, struct
        linked_list *ll);
static void make_process_daemon(void);
static void print_usage(char *binary_name);
static char *get_absolute_path(char *path, int fd);
static void check_if_file_readable(char *path, int fd);
static int check_for_creation_dir(struct linked_list *ll);
static int check_for_creation_file(struct linked_list *ll);


/* main function */
int main(int argc, char *argv[])
{
    if (argc != 2) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    char *filename_abs = get_absolute_path(argv[1], STDERR_FILENO);
    check_if_file_readable(filename_abs, STDERR_FILENO);
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
    struct linked_list *ll = create_linked_list();
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
            display_inotify_event(event, f_stdout, ll);
        }
        // Check whether a directory or a file was created
        int created_dir = 0;
        if ((created_dir = check_for_creation_dir(ll)) || check_for_creation_file(ll)) {
            time_t result = time(NULL);
            char *time = ctime(&result);
            // Needed because ctime(), format is "Wed Jun 30 21:49:08 1993\n",
            // and we print the timestamp in the same line as the action
            time[strlen(time) - 1] = '\0';
            if (created_dir) {
                dprintf(f_stdout, "[%s] Directory created: %s\n", time, event->name);
            } else {
                dprintf(f_stdout, "[%s] File created: %s\n", time, event->name);
            }
            delete_linked_list(ll);
            ll = create_linked_list();
        }
    }
    return 0;
}

static int check_for_creation_dir(struct linked_list *ll)
{
    if (ll->size == 2 && ll->front->mask == IN_CREATE && ll->front->next->mask == IN_ISDIR) {
        return 1;
    }
    return 0;
}

static int check_for_creation_file(struct linked_list *ll)
{
    if (ll->size == 4 && ll->front->mask == IN_CREATE && ll->front->next->mask == IN_OPEN &&
            ll->front->next->next->mask == IN_MODIFY &&
            ll->front->next->next->next->mask == IN_CLOSE_WRITE) {
        return 1;
    }
    return 0;
}

static void add_only_different_mask(struct linked_list *ll, uint32_t mask)
{
    if (ll->last ==  NULL || ll->last != NULL && ll->last->mask != mask) {
        insert_node(ll, mask);
    }
}

static void display_inotify_event(struct inotify_event *i, int f_stdout, struct
        linked_list *ll)
{
    if (i->mask & IN_ACCESS) {
        add_only_different_mask(ll, IN_ACCESS);
    }
    if (i->mask & IN_ATTRIB) {
        add_only_different_mask(ll, IN_ATTRIB);
    }
    if (i->mask & IN_CLOSE_NOWRITE) {
        add_only_different_mask(ll, IN_CLOSE_NOWRITE);
    }
    if (i->mask & IN_CLOSE_WRITE) {
        add_only_different_mask(ll, IN_CLOSE_WRITE);
    }
    if (i->mask & IN_CREATE) {
        add_only_different_mask(ll, IN_CREATE);
    }
    if (i->mask & IN_DELETE) {
        add_only_different_mask(ll, IN_DELETE);
    }
    if (i->mask & IN_DELETE_SELF) {
        add_only_different_mask(ll, IN_DELETE_SELF);
    }
    if (i->mask & IN_IGNORED) {
        add_only_different_mask(ll, IN_IGNORED);
    }
    if (i->mask & IN_ISDIR) {
        add_only_different_mask(ll, IN_ISDIR);
    }
    if (i->mask & IN_MODIFY) {
        add_only_different_mask(ll, IN_MODIFY);
    }
    if (i->mask & IN_MOVE_SELF) {
        add_only_different_mask(ll, IN_MOVE_SELF);
    }
    if (i->mask & IN_MOVED_FROM) {
        add_only_different_mask(ll, IN_MOVED_FROM);
    }
    if (i->mask & IN_MOVED_TO) {
        add_only_different_mask(ll, IN_MOVED_TO);
    }
    if (i->mask & IN_OPEN) {
        add_only_different_mask(ll, IN_OPEN);
    }
    if (i->mask & IN_Q_OVERFLOW) {
        add_only_different_mask(ll, IN_Q_OVERFLOW);
    }
    if (i->mask & IN_UNMOUNT) {
        add_only_different_mask(ll, IN_UNMOUNT);
    }
}

static void make_process_daemon(void)
{
    // Make an orphan process by forking and killing parent process
    fprintf(stdout, "Starting process as daemon\n");
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

static void print_usage(char *binary_name)
{
    fprintf(stdout, "USAGE: %s FILE\nWhere FILE could be either a file or "\
            "directory. (File/Directory must to exist)\n", binary_name);
}

static void check_if_file_readable(char *path, int fd)
{
    char *full_path = get_absolute_path(path, fd);
    if (access(full_path, R_OK) != 0) {
        dprintf(fd, "access: %s. File: %s\n", strerror(errno), full_path);
        exit(EXIT_FAILURE);
    }
}

static char *get_absolute_path(char *path, int fd)
{
    char *full_path = realpath(path, NULL);
    if (full_path == NULL) {
        dprintf(fd, "realpath: %s. File: %s\n", strerror(errno), path);
        exit(EXIT_FAILURE);
    }
    return full_path;
}

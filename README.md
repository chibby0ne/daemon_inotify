# Daemon that uses inotify to monitor files/directories

[![Build Status](https://travis-ci.org/chibby0ne/daemon_inotify.svg?branch=master)](https://travis-ci.org/chibby0ne/daemon_inotify)

A daemon that monitors for events in a given file/directory, and informs the
user of the changes in the log.

Not very useful but I always wanted to use inotify for something...

At the moment it only detects/logs file and directory creation.


# Build

```bash
mkdir build
cd build
cmake .. -DBUILDDOC=1
make
```

or if using [ninja](https://ninja-build.org/) (Recommended)

```bash
mkdir build
cd build
cmake .. -GNinja -DBUILDDOC=1
ninja
```

## Usage

```bash
daemon_inotify FILENAME
Where FILENAME is the directory or file to watch (Required argument)
```

If something like this happens:

```
cp backup_dr_automated_testing.vp8.webm DIR
cp -r Richard_Feynman DIR
```

Example log:

```
[Wed Feb 21 23:20:28 2018] File created: backup_dr_automated_testing.vp8.webm
[Wed Feb 21 23:20:33 2018] Directory created: Richard_Feynman
```

## Improvements

- [] Using variable number of arguments
- [x] More user friendly log messages
- [] Add systemd unit
- [] Add log entries for deletion, modification and rename of file/directories 

## License
GNU GPLv2

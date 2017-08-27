# Daemon that uses inotify to monitor files/directories

A daemon that monitors for events in a given file/directory, and informs the
user of the changes in the log.

Not very useful but I always wanted to use inotify for something...


# Build

```bash
mkdir build
cd build
cmake .. 
make
```

or if using [ninja](https://ninja-build.org/) (Recommended)

```bash
mkdir build
cd build
cmake .. -GNinja
ninja
```

## Usage

```bash
daemon_inotify FILENAME
Where FILENAME is the directory or file to watch (Required argument)
```

## Improvements

- [] Using variable number of arguments
- [] More user friendly log messages
- [] Add systemd unit

## License
GNU GPLv2

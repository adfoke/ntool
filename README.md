# ntool

A network statistics tool similar to `netstat`, written in C for Linux and macOS.

## Features

- Display active TCP and UDP connections
- Support for various command-line options similar to netstat
- Cross-platform support (Linux and macOS)

## Usage

```
ntool [options]

Options:
  -t    Display TCP connections
  -u    Display UDP connections
  -n    Show numerical addresses instead of resolving hostnames
  -l    Show only listening sockets
  -a    Show all sockets (including listening)
  -p    Show process ID and program name

If neither -t nor -u is specified, both TCP and UDP connections are shown.
```

## Building

```bash
make
```

## Installation

```bash
make install  # Requires sudo
```

## Examples

```bash
# Show all TCP connections
ntool -t

# Show listening ports
ntool -l

# Show all connections with numerical addresses
ntool -a -n

# Show TCP connections with process information
ntool -t -p
```

## Platform Notes

- **Linux**: Full implementation using `/proc/net/tcp` and `/proc/net/udp`
- **macOS**: Basic implementation (work in progress), suggests using system `netstat`

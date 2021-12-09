# lineset

`lineset` is a cli tool for performing set operations on lines in files.

## Usage

```
Usage: lineset file1 operator file2

Operators:
  +  union
  &  intersection
  -  difference
  ^  symmetric difference
```
## Building

```shell
$ ./configure
$ make
```

## Examples

1.txt:
```
1
2
3
```

2.txt:
```
2
3
4
```

```shell
$ lineset 1.txt '+' 2.txt
1
2
3
4
$ lineset 1.txt '-' 2.txt
1
$ lineset 1.txt '&' 2.txt
2
3
$ lineset 1.txt '^' 2.txt
1
4 
```

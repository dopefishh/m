# m
Concept music library that is fully customizable and supports all tags

## Intro
It uses the vorbiscomments standard as a golden standard. This means that id3
tags are translated to their according vorbiscomments standard.

## License
See `COPYING`

## Installation
See `INSTALL`

## Requirements
### Required
- none

### Optional
These options can be set and unset in `conf.mk`

- `libflac` (for flac support).
- `libvorbis` (for ogg support).
- `libopusfile` (for opus support).
- `libid3tag` (for mp3 support).

## Compilation
- Generate the autotools files with `autoreconf --install`
- Run `./configure` (`./configure --help` to see all options)
- Run `make`

## Usage
### Command line
todo

### Config file
The config is searched for according to the `XDG` spec. Thus in
`~/.config/m/config`. It can also be specified with the `-c` flag.

For information about the format and the possible options, refer to the
`config.example` file.

### CI
![travis](https://travis-ci.org/dopefishh/m.svg?branch=master)

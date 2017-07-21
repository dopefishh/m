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

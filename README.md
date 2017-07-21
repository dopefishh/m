# m
Concept music library that is fully customizable and supports all tags

## Intro
It uses the vorbiscomments standard as a golden standard. This means that id3
tags are translated to their according vorbiscomments standard.

## Requirements
### Required
- none

### Optional
- `libflac` (for flac support).
- `libvorbis` (for ogg support).
- `libopusfile` (for opus support).
- `libid3tag` (for mp3 support).

## Compilation
- Specific settings for enabling or disabling formats can be controllen in
  `conf.mk`.
- Build by running `make`
- Install by running `make install` (not implemented yet)

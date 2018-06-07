#!/bin/bash
USE_MP3=no USE_FLAC=no USE_OGG=no USE_OPUS=no make -B &&\
USE_MP3=no USE_FLAC=no USE_OGG=no USE_OPUS=yes make -B &&\
USE_MP3=no USE_FLAC=no USE_OGG=yes USE_OPUS=no make -B &&\
USE_MP3=no USE_FLAC=no USE_OGG=no USE_OPUS=yes make -B &&\
USE_MP3=no USE_FLAC=no USE_OGG=yes USE_OPUS=yes make -B &&\
USE_MP3=no USE_FLAC=yes USE_OGG=no USE_OPUS=no make -B &&\
USE_MP3=no USE_FLAC=yes USE_OGG=yes USE_OPUS=no make -B &&\
USE_MP3=no USE_FLAC=yes USE_OGG=no USE_OPUS=yes make -B &&\
USE_MP3=no USE_FLAC=yes USE_OGG=yes USE_OPUS=yes make -B &&\
USE_MP3=yes USE_FLAC=no USE_OGG=no USE_OPUS=no make -B &&\
USE_MP3=yes USE_FLAC=no USE_OGG=no USE_OPUS=yes make -B &&\
USE_MP3=yes USE_FLAC=no USE_OGG=yes USE_OPUS=no make -B &&\
USE_MP3=yes USE_FLAC=no USE_OGG=no USE_OPUS=yes make -B &&\
USE_MP3=yes USE_FLAC=no USE_OGG=yes USE_OPUS=yes make -B &&\
USE_MP3=yes USE_FLAC=yes USE_OGG=no USE_OPUS=no make -B &&\
USE_MP3=yes USE_FLAC=yes USE_OGG=yes USE_OPUS=no make -B &&\
USE_MP3=yes USE_FLAC=yes USE_OGG=no USE_OPUS=yes make -B &&\
USE_MP3=yes USE_FLAC=yes USE_OGG=yes USE_OPUS=yes make -B

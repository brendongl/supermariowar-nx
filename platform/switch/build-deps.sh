#!/usr/bin/env bash
# Builds SDL3 (sdl3-nx: SDL 3.4.18 + devkitPro's Switch backend), SDL3_image,
# SDL3_mixer and toml11 for the Switch into $PREFIX. Runs inside devkitpro/devkita64.
set -euo pipefail

SRC_DIR=${SRC_DIR:-$PWD/.deps-src}
BUILD_DIR=${BUILD_DIR:-$PWD/.deps-build}
PREFIX=${PREFIX:-$PWD/.deps-switch}
TOOLCHAIN=$DEVKITPRO/cmake/Switch.cmake
PATCH_DIR=$(cd "$(dirname "$0")" && pwd)/patches

SDL_REPO=${SDL_REPO:-https://github.com/nx-mod/sdl3-nx.git}
SDL_COMMIT=${SDL_COMMIT:-07ea26a3c}
SDL_IMAGE_REF=${SDL_IMAGE_REF:-release-3.4.8}
SDL_MIXER_REF=${SDL_MIXER_REF:-release-3.2.4}
TOML11_REF=${TOML11_REF:-v4.4.0}

fetch() { # name url ref
    [ -d "$SRC_DIR/$1" ] || git clone -q --depth 1 -b "$3" "$2" "$SRC_DIR/$1"
}

mkdir -p "$SRC_DIR" "$BUILD_DIR" "$PREFIX"
if [ ! -d "$SRC_DIR/SDL3" ]; then
    git clone -q --filter=blob:none "$SDL_REPO" "$SRC_DIR/SDL3"
    git -C "$SRC_DIR/SDL3" -c advice.detachedHead=false checkout -q "$SDL_COMMIT"
    for p in "$PATCH_DIR"/sdl3-*.patch; do git -C "$SRC_DIR/SDL3" apply "$p"; done
fi
fetch SDL3_image https://github.com/libsdl-org/SDL_image.git   "$SDL_IMAGE_REF"
fetch SDL3_mixer https://github.com/libsdl-org/SDL_mixer.git   "$SDL_MIXER_REF"
fetch toml11     https://github.com/ToruNiina/toml11.git       "$TOML11_REF"

common=(-G Ninja -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX="$PREFIX" -DCMAKE_PREFIX_PATH="$PREFIX"
        -DBUILD_SHARED_LIBS=OFF)

build() { # name extra-args...
    local name=$1; shift
    cmake -S "$SRC_DIR/$name" -B "$BUILD_DIR/$name" "${common[@]}" "$@"
    cmake --build "$BUILD_DIR/$name"
    cmake --install "$BUILD_DIR/$name"
}

build SDL3 -DSDL_SHARED=OFF -DSDL_STATIC=ON -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF \
    -DSDL_TEST_LIBRARY=OFF -DSDL_CAMERA=OFF -DSDL_TRAY=OFF -DSDL_GPU=OFF -DSDL_VULKAN=OFF

build SDL3_image -DSDLIMAGE_VENDORED=OFF -DSDLIMAGE_DEPS_SHARED=OFF -DSDLIMAGE_SAMPLES=OFF \
    -DSDLIMAGE_TESTS=OFF -DSDLIMAGE_BACKEND_STB=OFF -DSDLIMAGE_PNG=ON -DSDLIMAGE_BMP=ON \
    -DSDLIMAGE_ANI=OFF -DSDLIMAGE_AVIF=OFF -DSDLIMAGE_GIF=OFF -DSDLIMAGE_JPG=OFF -DSDLIMAGE_JXL=OFF \
    -DSDLIMAGE_LBM=OFF -DSDLIMAGE_PCX=OFF -DSDLIMAGE_PNM=OFF -DSDLIMAGE_QOI=OFF -DSDLIMAGE_SVG=OFF \
    -DSDLIMAGE_TGA=OFF -DSDLIMAGE_TIF=OFF -DSDLIMAGE_WEBP=OFF -DSDLIMAGE_XCF=OFF -DSDLIMAGE_XPM=OFF \
    -DSDLIMAGE_XV=OFF

build SDL3_mixer -DSDLMIXER_VENDORED=OFF -DSDLMIXER_DEPS_SHARED=OFF -DSDLMIXER_SAMPLES=OFF \
    -DSDLMIXER_TESTS=OFF -DSDLMIXER_WAVE=ON -DSDLMIXER_VORBIS_STB=ON -DSDLMIXER_VORBIS_VORBISFILE=OFF \
    -DSDLMIXER_VORBIS_TREMOR=OFF -DSDLMIXER_FLAC=OFF -DSDLMIXER_GME=OFF -DSDLMIXER_MOD=OFF \
    -DSDLMIXER_MP3=OFF -DSDLMIXER_MIDI=OFF -DSDLMIXER_OPUS=OFF -DSDLMIXER_WAVPACK=OFF \
    -DSDLMIXER_AIFF=OFF -DSDLMIXER_VOC=OFF -DSDLMIXER_AU=OFF

build toml11 -DTOML11_BUILD_TESTS=OFF

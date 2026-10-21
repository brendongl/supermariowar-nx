# Host build of the same tree with sanitizers, for fast headless auto-test soaks
FROM ubuntu:24.04
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build git ca-certificates pkg-config python3 llvm-18 \
    && rm -rf /var/lib/apt/lists/*
ENV ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer-18

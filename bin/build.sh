#!/usr/bin/env zsh

set -euo pipefail

bazelisk build //... --java_language_version=11
cargo build
dotnet build
cmake -B build-cmake -S . -GNinja && cmake --build build-cmake

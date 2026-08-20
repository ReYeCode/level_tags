name: Build Geode Mod

on:
  push:
    branches:
      - main
      - master
  pull_request:
  workflow_dispatch:

jobs:
  build:
    name: ${{ matrix.config.name }}
    runs-on: ${{ matrix.config.os }}
    strategy:
      fail-fast: false
      matrix:
        config:
          - name: Windows
            os: windows-latest
          - name: macOS
            os: macos-latest
          - name: Android (64-bit)
            os: ubuntu-latest
            target: android64
          - name: Android (32-bit)
            os: ubuntu-latest
            target: android32

    steps:
      - name: Checkout Code
        uses: actions/checkout@v4

      - name: Build Mod
        uses: geode-sdk/build-geode-mod@main
        with:
          target: ${{ matrix.config.target }}
          combine: true

  package:
    name: Package Mod
    runs-on: ubuntu-latest
    needs: build
    steps:
      - name: Package Geode Mod
        uses: geode-sdk/build-geode-mod/combine@main
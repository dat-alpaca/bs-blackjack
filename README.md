# BS-Blackjack
> This is a university assignment written in C. It consists of a blackjack clone for the POSIX terminals.

# Getting Started
There are two variants for this project. The first being a single file monolithic translation unit under `original/`. The folder also contains the `gcc` command to compile the file.

The `src/` folder contains a modularized version of `original/` and is identical in behavior, but may contain less compile-time sanity checks. The rest of this README explains how to compile from scratch, but a release version should be available for download.

* Note: this project has only been tested in [Ubuntu 24.04 LTS](https://ubuntu.com/blog/tag/ubuntu-24-04-lts) using the default terminal and [Fedora Linux 43 (KDE Plasma Edition)](https://www.fedoraproject.org/), using Konsole.

## Prerequisites
Building this project requires the following:
* [CMake](https://cmake.org/download/)

## Installation

1. **Clone the repository**
    ```bash
    git clone https://github.com/dat-alpaca/bs-blackjack
    ```

2. **Generate the project**
    ```bash
    ./scripts/generate.sh
    ```
   
    You can edit the [CMakePresets.json](https://github.com/dat-alpaca/bs-blackjack/blob/main/CMakePresets.json) file directly.
    <br>

4. **Compile the project**
    ```bash
    ./scripts/build.sh
    ```
   
    After that, you should have a working copy under `build/`.

# Playing
Run the executable and follow the instruction on the terminal to play. The game does not have serialization for client data, so the client stats, namely "win count", "lost count", and "draw count" are reset after closing the game.


# License
This project is licensed under the [MIT license](https://licenses.opensource.jp/MIT/MIT.html) - see the [LICENSE](LICENSE) file for details.
# Learn C
Some examples including vscode configuration to make the approach easier

# Requirements linux
- gcc

# Build
- navigate in the desired folder
- gcc hello.c -o hello
- gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 -o myprogram myprogram.c

## Build with vscode
- Press ctrl+maiusc+B to launch the default task
- Press F5 to build and attach in debug mode

## Release

Some utilities in this repository are useful standalone tools. Pre-built binaries are available via `GitHub Releases`.

### Building release binaries

```sh
# Clean up
make clean
# Build all binaries to release
make
```

## Format
Code is formatted using `clang-format`, bundled with the `ms-vscode.cpptools` extension (see [.vscode/extensions.json](.vscode/extensions.json)).

Formatting rules are defined in the `.clang-format` file.

```sh
# Format all files
find . -name "*.c" -o -name "*.h" | xargs ~/.vscode/extensions/ms-vscode.cpptools-*/LLVM/bin/clang-format -i
 ```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Third-Party Components

This project includes third-party components located in the `third_party/` directory.
These components are not covered by this project's MIT License and retain their
original licenses and credits

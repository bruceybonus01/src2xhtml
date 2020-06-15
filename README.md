## Work in progress
Further optimisations and cleaner code will be coming shortly, along with some completed default themes.

# src2xhtml
A tool for generating syntax highlighted, XHTML 1.0 Strict, CSS 1 HTML documents for your source code.

src2xhtml allows you add any languages and themes you want, including your own languages and languages that are not supported by other HTML generators.

## Preview
It's not perfect, but I'm working on it. Here is the output of running the example shown below. 

On my system (Intel core i5 running Manjaro), creating this file took 1ms. So theoretically, this can achieve 1000 files per second. Although, time taken is proportional to both the file length, and the complexity of the theme.

<img src="https://adambruce.dev/res/xhtml_beta.png" alt="drawing" width="800"/>

## Compiling

### *nix (Linux, BSD, Darwin)
Tested on:
- MINIX 3.2.1 (VirtualBox)
- FreeBSD 13 (AARCH64)
- Manjaro Linux 5.6 (AMD64)


If you have make, you can compile using the Makefile provided. This will create an dynamically linked executable called `src2xhtml`:
```
make
```

Additionally, you can compile with your favorite compiler:

Clang:
```
clang -Iinclude/ -o src2xhtml src/*.c
```

GCC: 
```
gcc -Iinclude/ -o src2xhtml src/*.c
```

### Windows
Tested on Windows 10 (AMD64).

1. Open `src2xhtml.sln` in Visual Studio 2019.

2. From the menu at the top, select `Build` >  `Build Solution`.

3. If you built for x86, a folder called `Release` will have been created which contains the program. The program is called `src2xhtml.exe`.

    If you built for x64, a folder called `x64` will have been created which contains another folder called `Release`, which contains the program.

## Usage
src2xhtml takes the following ordered, command line arguments where the theme file is optional:

### *nix
```
./src2xhtml <input_file> <output_file> [theme_file]
```

### Windows
```
src2xhtml.exe <input_file> <output_file> [theme_file]
```

## Example
An easy example to test your executable and understanding is to create a HTML document of the source code included, such as:

### *nix
```
./src2xhtml src/main.c main.html themes/monokai.txt
```

### Windows
```
src2xhtml.exe src/main.c main.html themes/monokai.txt
```

The result will be a HTML document in the called `main.html`, which contains the source code of src/main.c, with formatting and syntax highlighting.

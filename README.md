# cplox

A scripting language built in C++

## What is this?

cplox is a C++17 port of Bob Nystrom's programming language [Lox](https://github.com/munificent/craftinginterpreters), from the book [Crafting Interpreters](http://craftinginterpreters.com/).

The aim of this project is to learn how programming languages are built from the ground up.

## How does it work?

cplox consists of a handwritten compiler and virtual machine. The compiler compiles Lox source code to bytecode and the virtual machine interprets the bytecode instructions in order to run the Lox program.

To build the interpreter, you'll need [make](https://www.gnu.org/software/make/). Once you have that set up, run:

```sh
$ make cplox
```

This will copy the built interpreter into the root of the repository. You can run the REPL with this command:

```sh
$ ./cplox
```

And you can run a program with this command:

```sh
$ ./cplox [program].lox
```

To run the tests, you'll need [Dart](https://dart.dev/get-dart). Once you've installed it, run the following command to get the dependencies:

```sh
$ make get
```

If you develop on a non-Windows OS, you might need to change the command in the Makefile to use `pub` instead of `pub.bat`.

Run this command to compile a debug build of cplox and run all tests against it:

```sh
$ make test
```

## How can I contribute?

Contributions are welcome! To contribute to Lox, see the [repo](https://github.com/munificent/craftinginterpreters). To contribute to cplox, please follow the process below:

* Create a personal fork of this GitHub repo.
* In your fork, create a branch off the **master** branch.
* Make and commit your changes to your branch, keeping the following in mind:
    * Follow the current coding style.
    * Include tests when adding a new feature.
* Build the repo with your changes, and ensure that all tests pass.
* Create a pull request (PR) against the repo's **master** branch:
    * Add a title that summarises the changes.
    * In the description, list the main changes.
* Wait for feedback or approval of your changes.
* When your changes have been approved, the PR will be merged.

# License

The project is licensed under the [MIT license](LICENSE.txt).
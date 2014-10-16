TAM (Test-suite Analysis and Manipulation)
========
TAM (Test-suite Analysis and Manipulation) is a GUI application, written in Qt, that is created for researchers and developers to help investigating and manipulating test suites or creating assesment reposrts. It is built on top of the [SoDA](http://github.com/sed-szeged/soda) library.
It provides a graphical user interface for most of the algorithms implemented in the library.

### What's next?

* Support handling multiple binaries simultaneously.
 * To be able to load multiple versions of coverage and results binaries into TAM and be able to switch between them.
* Review User Interface
 * Help developers, test planners to identify bad smells in test suite and provide an interface where they can mark parts of the test suite that can be improved by developers.
 
Send us your improvement ideas or help us to implement these features. Open an issue to start a discussion or send your pull request.

## Getting started

### Dependencies

* cmake 2.8.0 or higher
* Boost 1.49 or higher
* [SoDA](http://github.com/sed-szeged/soda) (develop branch)
* Qt 5.3

### Authors and Contributors

SoDA is created and maintained by the [Department of Software Engineering](http://www.sed.hu), [University of Szeged](http://www.u-szeged.hu). 

#### Contribution
If you would like to contribute to the project, create an issue at our GitHub page or send a pull request. Every contribution is welcome!

We are using [git flow](http://danielkummer.github.io/git-flow-cheatsheet/) to manage our repository.
The *origin/develop* is our main development branch. *origin/master* is always in production ready state.

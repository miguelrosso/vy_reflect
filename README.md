# VY_REFLECT
## a tiny reflection library
vy_reflect is a c++17 reflection library written for educational purposes while developing a larger application development framework, **very**.

## a message from the developer
vy_reflect is by no means a 'battle tested' library and has many shortcomings as well as unhandled cases.
that said, the basic functionality _is_ there. just don't expect it to be adopted by your FAANG-likes anytime soon.

## building
vy_reflect makes extensive use of `inline static`s which were introduced in c++17. it is perfectly possible to back-port the code to work in older standards but that would require manual intervention.

there is a code example under `test/`. in linux, you can build by running `test/build_test.sh`.
there are a bunch of warnings still in need of addressing but the thing does compile and run.
the test build products will be generated in `test/build/`.

## very
very is an in-development general purpose application development framework. vy_reflect represents a vertical slice taken out of that, and published as it's own repo. this was done as very itself is unfinished, and i've decided to start again from scratch, with a radically different approach.
if you're interested to know why: very was originally developed as a C++ framework and had it's own build system written in C. it was structured as a collection of 'modules' that could compile into .lib, .dll or executable files. it was written in a very 'modern' c++ game-engine code style, similar to the Unreal Engine. after a good while i realized this was what i actually wanted to keep away from, so now i'm rewriting the whole thing as a single-header C library. keeping things as simple as possible is the new name of the new game.
if you wanna know more about very, keep an eye on my github page as well as `yvm.ch`. i'll post stuff there eventually, some time.

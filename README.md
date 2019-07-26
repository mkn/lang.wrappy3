| Operating system  | Build Status |
| ------------- | ------------- |
| Linux/Mac  | [![Linux/Mac Build](https://travis-ci.org/mkn/lang.wrappy3.svg?branch=master)](https://travis-ci.org/mkn/lang.wrappy3)  |
| Windows  | [![Windows Build](https://ci.appveyor.com/api/projects/status/abjfv0rawliv0qqn/branch/master?svg=true)](https://ci.appveyor.com/project/dekken/lang-wrappy3)  |

# lang.wrappy3

**Cross platform Python3 Wrapping in C++**

Fork of: https://github.com/lava/wrappy

Supported Architectures/Operating Systems:

    x86_64 / Linux
    x86_64 / BSD
    x86_64 / Windows


##Original README.md

# I wanna send an email!

    #include <wrappy/wrappy.h>

    int main() {
         auto s = wrappy::call("smtplib.SMTP", "localhost");
         s.call("sendmail", "python@localhost", "wrappy@example.org",
             "Subject: Norwegian blue parrot for sale\n"
             "Amazing offer, only while supplies last!\n"
             "Product may show signs of usage.");
    }

Requires an SMTP daemon to be running on localhost, e.g.
`sudo aptitude install sendmail`. Note that most webmail providers will
classify the mail as spam and put it in the thrash or reject it outright.

# I wanna be a turtle!

    #include <wrappy/wrappy.h>

    void drawTree(double len, double angle, int lvl) {
        if(!lvl) return;

        wrappy::call("turtle.left", angle);
        wrappy::call("turtle.forward", len);
        drawTree(0.5*len,  60, lvl-1);
        drawTree(0.7*len,  0,  lvl-1);
        drawTree(0.5*len, -60, lvl-1);
        wrappy::call("turtle.backward", len);
        wrappy::call("turtle.right", angle);
    }

    int main() {
        drawTree(100, 90, 6);
    }


# I wanna be a scientist!

    #include <wrappy/wrappy.h>
    #include <algorithm>

    int main() {
        std::vector<double> x {1.0, 2.0, 3.0, 4.0}, y {1.5, 1.0, 1.3, 2.0};
        std::vector<wrappy::PythonObject> pyx, pyy;

        std::transform(x.begin(), x.end(), std::back_inserter(pyx),
            [](double d) { return wrappy::construct(d); });
        std::transform(y.begin(), y.end(), std::back_inserter(pyy),
            [](double d) { return wrappy::construct(d); });

        wrappy::call("matplotlib.pyplot.plot", pyx, pyy);
        wrappy::call("matplotlib.pyplot.show");
    }

Requires the matplotlib package to be installed.
This example was the original motivation for this library, but if you actually
*just* want to plot something from C++, there is a header-only alternative: [matplotlib-cpp](http://github.com/lava/matplotlib-cpp)

# I wanna use this library!

#### ...and I don't need no fancy extras

    cmake .
    make
    sudo make install  # or: sudo checkinstall

#### ...and I like to do things properly

    git checkout ubuntu/xenial
    dch -i  # Skip this step and the next if you don't have a valid gpg key anyways
    git commit -am "Changes for local 1.0.0 release"
    gbp buildpackage --git-debian-branch=ubuntu/xenial --git-upstream-tag='v%(version)s'
    sudo dpkg -i ../libwrappy*.deb

Requires `git-buildpackage` and Ubuntu 16.04, although this should work on almost any other Debian-like distribution as well. (`--git-upstream-tag` may have been called `--git-upstream-version` in previous versions)

# I wanna be educated!
The idea of this library is to make it easy to call out to python code from C++.

Why would you want to do this? If there is a python-only library that
offers some needed functionality and no equivalent python library exists. In
that case, it is often more practical and far easier to do it this way than
to wrap your C++ code in python. As the examples above show, just a few lines
of code can be enough.

The heart of this library is the `PythonObject` class, which is a RAII-wrapper around python's `PyObject*` that handles
the reference counting. Together with the `call()`- and `construct()`-families of functions seen in the
examples above, these form a compact API that enables a very "natural" style of calling out to python code.

Right now, this library only works with the 2.x versions of python, since the C API changed with version 3. It's
quite similar, so the effort to port it to Python 3 shouldn't be too big. However, given that most people seem to be
happy with 2.7, the chances that someone will bother to do it seem to be low.

## Debugging

You can use the python pretty printing facilities for debugging:

    (gdb) p _PyObject_Dump(iter_.get())
    object  : <listiterator object at 0x7ffff7e97390>
    type    : listiterator
    refcount: 1
    address : 0x7ffff7e97390


# API reference

(work in progress)

## Free functions

* `wrappy::load(const std::string& name)`
Load the python object identified by the given name. It should be either
builtin function (`isinstance`, `eval`, ...), or the fully qualified name of a callable
in some python module (`os.getpid`, `f`)

Names are resolved in the following way: The given name is split at dots, and the longest
prefix that is a valid import name is used as the module name. Afterwards, all components
are resolved as attributes of the previous object.

* `wrappy::call(name, Args...)`
Call the function with the given args.

* `wrappy::call(PythonObject from, const std::string& name, Args...)

* `PythonObject wrappy::construct(const std::string&)`
* `PythonObject wrappy::construct(int)`
* `PythonObject wrappy::construct(long long)`
* `PythonObject wrappy::construct(float)`
* `PythonObject wrappy::construct(double)`
* `PythonObject wrappy::construct(bool)`
* `PythonObject wrappy::construct(PythonObject)`
Construct a python primitive (PyString, PyNumber, ...) from the corresponding C++ type.
The overload that takes a PythonObject as argument is the identity function.

## struct PythonObject
* PythonObject PythonObject::attr(const std::string& name)
Returns the result of executing x.attr in python.

* long long PythonObject::num()
* double PythonObject::floating()
* std::string PythonObject::str()
Convert the python object to the corresponding C++ type.

* `PyObject* PythonObject::get()`
Return the underlying `PyObject*`. Remember to `Py_INCREF()` if you intend to use it
independently of the PythonObject it came from.



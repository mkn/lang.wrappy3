#pragma once

#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include <wrappy/detail/sys.hpp>

struct _object;
typedef _object PyObject;

/**
 * Note that this library is *NOT* thread-safe.
 *
 * This is basically unfixable since the reference count of a python object
 * is stored in a non-atomic ssize_t and modified without any locking,
 * making any increases and decreases inherently racy.
 *
 * If you want to use this in a multi-threaded environment, please make sure
 * to protect all concurrent calls that are not read-only by mutexes.
 */

namespace wrappy {

class WrappyError : public std::runtime_error {
public:
    WrappyError(const std::string& str)
    : std::runtime_error(str)
    { }
};

// A Raii-wrapper around PyObject* that transparently handles
// the necessary reference-counting
class PythonObject {
public:
    // Accessors
    // Python doesn't care about const-ness, so having a separate acccessor
    // for a const PyObject* would be useless. We have to trust the caller
    // not to do non-const things with it.
    long long num() const;
    double floating() const;
    const char* str() const;
    PyObject* get() const;
    PythonObject attr(const std::string& x) const; // returns self.x

    // Give up ownership of the underlying object
    PyObject* release();

    // Constructors
    struct owning {};
    struct borrowed {};
    PythonObject(); // Note that this does not default to "None"
    PythonObject(owning, PyObject*);
    PythonObject(borrowed, PyObject*);

    // Syntactic sugar
    operator bool() const;

    PythonObject operator()() const; // forwards to self.__call__()

    template<typename... Args>
    PythonObject call(const std::string& f, Args... args);

    // Rule-of-five plumbing
    ~PythonObject();
    PythonObject(const PythonObject&);
    PythonObject& operator=(const PythonObject&);
    PythonObject(PythonObject&&);
    PythonObject& operator=(PythonObject&&);

private:
    PyObject* obj_;
};

// Note that this is an input iterator, iterators cannot
// be stored, rewound, or compared to anything but "end"
struct PythonIterator {
    PythonIterator& operator++(); // pre-increment
    PythonObject operator*();     // dereference
    // *only* for comparison to "end", python iterators have
    // no concept of position or comparability
    bool operator!=(const PythonIterator&);

private:
    PythonIterator(bool, PythonObject);
    friend PythonIterator begin(PythonObject);
    friend PythonIterator end(PythonObject);

    bool stopped_;
    PythonObject iter_;
    PythonObject obj_;
};

PythonIterator begin(PythonObject);
PythonIterator end(PythonObject);

// Providing access to pythons global objects
extern PythonObject None;
extern PythonObject True;
extern PythonObject False;

void addModuleSearchPath(const std::string& path);

// There is one quirk of call() for the case of member methods:
//
//     call("module.A.foo")
//
// calls the unbound method "foo", so it is necessary to provide an instance
// of A as the first argument, while
//
//     auto a = call("module.A"); call(a, "foo");
//
// calls the method "foo" that is already bound to a, so providing an explicit
// self argument in that case is an error.

template<typename... Args>
PythonObject call(const std::string& f, Args... args);

template<typename... Args>
PythonObject call(PythonObject from, const std::string& f, Args... args);


// The template-magic in call() constructs a series of appropriate calls
// to these functions, but of course they can also be used directly:

PythonObject construct(long long);
PythonObject construct(int);
PythonObject construct(double);
PythonObject construct(const std::string&);
PythonObject construct(PythonObject); // identity
PythonObject construct(const std::vector<PythonObject>&); // python list

// TODO there is no good way to actually call these constructed functions
typedef PythonObject (*Lambda)(const std::vector<PythonObject>& args, const std::map<const char*, PythonObject>& kwargs);
typedef PythonObject (*LambdaWithData)(const std::vector<PythonObject>& args, const std::map<const char*, PythonObject>& kwargs, void* userdata);
PythonObject construct(Lambda);
PythonObject construct(LambdaWithData, void*);

PythonObject callWithArgs(
    const std::string& function,
    const std::vector<PythonObject>& args
        = std::vector<PythonObject>(),
    const std::vector<std::pair<std::string, PythonObject>>& kwargs
        = std::vector<std::pair<std::string, PythonObject>>());

PythonObject callWithArgs(
    PythonObject from,
    const std::string& function,
    const std::vector<PythonObject>& args
        = std::vector<PythonObject>(),
    const std::vector<std::pair<std::string, PythonObject>>& kwargs
        = std::vector<std::pair<std::string, PythonObject>>());


// Just get an object, without calling a function
PythonObject load(const std::string& name);


// Will call x.__enter__() in constructor and x.__exit__() in destructor
class ContextManager {
public:
	ContextManager(PythonObject x);
	~ContextManager();
};

} // end namespace wrappy

#include <wrappy/detail/call.hpp>

#if        !defined(_WRAPPY_AS_LIB_)
#include <wrappy/detail/wrappy.ipp>
#endif  // !defined(_WRAPPY_AS_LIB_)

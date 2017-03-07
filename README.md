# Overview

"In general, high-level languages make complex programming simpler, while
low-level languages tend to produce more efficient code.",
[http://en.wikipedia.org/wiki/High\_level\_language](http://en.wikipedia.org/wiki/High_level_language)

There is often a need both high-level and low-level programming to solve
problems, but little use for what is in between.  Specifically either the
performance of a particular aspect of a project matters or it does not.  When 90
percent of the code that goes into a project is not performance critical, there
can be significant gains by using a much higher-level language.  Not only does
this decrease development time but it also lowers the potential for bugs.
Unfortunately languages are either high-level or low-level, and a multi-language
solution means having to use language coupling mechanisms and type conversions.
While applications like Swig help to bridge the gap, it is still significant
effort and increases project complexity.

The purpose of the Asuka Type Library is to alleviate unnecessary complexity,
but it requires a change in methodology.  At the core of the Asuka Type Library 
is a type abstraction layer called `AT::Field` and a container called 
`AT::Collection`.  The combination of dynamic typing with containers allows for
complex data hierarchies to be built with ease, very similar to lua tables or
perl hashes.  While less efficient than static typed C++ containers, this
functionality simplifies many tasks that can take up significant development
time for a project.  Providing this framework in standard C++ gives the benefit
of being able to use customized code for handling performance intensive aspects
of projects and use generic code for the less performance critical, both within
the same language.

# Fundamental types

## AT::Collection

The `AT::Collection` type provides two internal data structures, an ordered
vector and an associative array.  Direct access to the `AT::Collection`
interface is not necessary since all functionality is exposed through the
`AT::Field` class.

### Indexing

Access to a node is provided using the index operator `[]`.  Which of the two
types of containers accessed is determined from the type provided as an index.
If the incoming parameter is an integer type, including `char`, then the vector
portion is accessed.  If the incoming parameter is non-integer like `char *` or
`float` then the hash is accessed.

    AT::Field list;
    list[0] = 1;   //set vector index 0 to 1
    list["0"] = 2; //set hash key "0" to 2

The following example creates a 3 tier structure:

    AT::Field foo;
    foo["user"][0]["name"] = "fred";
    foo["user"][1]["name"] = "bob";

### Methods

- push - `AT::Field &instance.push(AT::Field)`

    Pushes a new `AT::Field` onto the tail of the vector.  Defaults to index 0 if
    pushing onto an empty vector.

- pop - `AT::Field instance.pop()`

    Returns a copy of the tail `AT::Field` and decrements the AT::Collection top.
    Returns undef if the vector is empty.

- shift - `AT::Field &instance.shift(AT::Field)`

    Pushes a new `AT::Field` onto the front of the vector.

- unshift - `AT::Field instance.unshift()`

    Returns a copy of the front `AT::Field` and increments the `AT::Collection`
    offset.  Returns undef if the vector is empty.

- offset - `AT::Integer32 instance.offset()`

    Returns the lowest index used by the vector.

- top - `AT::Integer32 instance.top()`

    Returns the highest index used by the vector.

- size - `AT::Integer32 instance.size()`

    Returns the number of items allocated for in the vector.  Does not take into
    account the vector offset.

- count - `AT::Integer32 instance.count()`

    Returns the number of items in the vector (top+1).

- keys - `AT::Collection instance.keys()`

    Returns a list of hash and vector keys that exist in the collection instance.

### Iteration

List iteration is done directly against the list itself using member methods.  A
standalone iteration object is not provided.

- foreach - `foreach(AT::Field, AT::Collection)`

    If you have a dislike of macros, you'll probably want to not use this.  This
    macro is simply a shortcut for a loop using the standard iteration methods.  If
    you want to modify the values of the list, make sure to use a reference to the
    list like the example below.  Unfortunately it is not possible to define the
    `AT::Field` value receiver inline.

        AT::Field users, user;
        users.push("Bob");
        users.push("Fred");
        foreach(user, &users) {
          user.lc(); //make all usernames lower case
          std::cout<<user<<std::endl;
        }

- current - `AT::Field &instance.current()`

    Returns the instance where the iterator is currently positioned.

- iterate - `AT::Field &instance.iterate()`

    Returns the current instance, and increments the iterator.

- first - `AT::Field &instance.first()`

    Returns the first instance in the vector.

- last - `AT::Field &instance.last()`

    Returns the last instance in the vector.

- next - `AT::Field &instance.next()`

    Returns the next instance in the vector.

- prior - `AT::Field &instance.prior()`

    Returns the prior instance in the vector.

## AT::Field

### Supported types

The Asuka Type Library provides abstract type definitions that are more
representative of the true datatype storage requirements.  This is common
practice in a number of libraries, so doing it again here shouldn't be a
surprise.  It is not recommended for external clients to use these.  They are
just provided here so developers are aware of what the function declarations
actually require.

    AT::Integer     int
    AT::Integer08   char
    AT::Integer16   short
    AT::Integer32   long
    AT::Integer64   long long
    AT::Unsigned    unsigned int
    AT::Unsigned08  unsigned char
    AT::Unsigned16  unsigned short
    AT::Unsigned32  unsigned long
    AT::Unsigned64  unsigned long long
    AT::Float32     float
    AT::Float64     double
    AT::Character   char
    AT::Byte        unsigned char
    AT::Boolean     bool
    AT::Unknown     void

### Type casting

Implicit type casting usually works, but on occasion it may be necessary to
explicitly cast from `AT::Field` instances.  Such as calling standard C/C++
routines or doing arithmetic.  The implicit type casting also does not support
user defined types.  Casting to a 'void \*' will allow safe containment of user
instantiated objects as long as the referenced object doesn't leave scope.
Dynamic memory allocation is recommended for user type containment, just make
sure to do garbage collection.  One case of needing to do this is with binary
data.  An example interface that uses this method is the resource loader.

    AT::Resource::Manager loader;
    loader.accept("c:\\asuka\\modules\\resource");
    AT::Field image = loader.input("c:\\asuka\\resources\\misc\\256x256_24b.tga");
    unsigned char *pixels = (unsigned char *)(void *)image["buffer"][0];
    loader.release(image);

### Lua

Type support for `lua_State*` is directly supported by the `AT::Field` class.
Both in and out of lua calls are supported.  For arguments being passed to lua
functions a `lua_setfield` routine is provided and for rvals direct type
casting is used.

- lua\_setfield - `lua_setfield(lua_State *, AT::Field)`

    Pushes onto the state stack the values in the `AT::Field` instance.  This only
    supports list context.  Any arbitrary keys may be used, they do not have to be
    sequential.

The following is a complete example of a C++ application which calls a
`example.lua` script.

    #include <asuka.hpp>
    #include <iostream>
    
    static const luaL_reg lualibs[] = {
      {"base", luaopen_base},
      {"table", luaopen_table},
      {"io", luaopen_io},
      {"string", luaopen_string},
      {"math", luaopen_math},
      {"debug", luaopen_debug},
      {"loadlib", luaopen_loadlib},
      {NULL, NULL}
    };
    
    static void lua_loadlibs(lua_State *state) {
      const luaL_reg *lib = lualibs;
      for(; lib->func; lib++) {
        lib->func(state);
        lua_settop(state, 0);
      }
    }
    
    int main() {
      try {
        lua_State* state = lua_open();
        lua_loadlibs(state);
        if(luaL_loadfile(state, "example.lua")) {
          throw("failed to load example.lua");
        }
        AT::Field arguments;
        arguments.push(20);
        arguments["nothing"] = "something";
        lua_setfield(state, arguments);
        lua_setglobal(state,"arg");
        if(lua_pcall(state, 0, LUA_MULTRET, 0)) {
          throw("script failure");
        }
        AT::Field value;
        foreach(value, state) {
          std::cout<<"rval '"<<value<<"'"<<std::endl;
        }
      }
      catch(...) {
        std::cout<<"-exception-"<<std::endl;
      };
      std::cout<<"-finished-"<<std::endl;
      return 0;
    }

### JSON

The JSON [http://www.json.org](http://www.json.org) syntax is simply a data transport syntax native
to javascript.  This makes it especially useful for DHTML implementations that do
not want to deal with issues that come from using an XML DOM and closures.  That
and it is much faster...

- encode - `AT::Field AT::JSON::encode(AT::Field &)`

    Returns a string that contains the contents of the provided AT::Field in JSON
    format.

- decode - `AT::Field AT::JSON::decode(AT::Field)`

    Parses the input string, expected in JSON format, and returns an AT::Field
    heirarchy which mirrors the contents of the string.  Root level variables will
    be the first nodes of the AT::Field collection.

        AT::Field data = AT::JSON::decode("var v1=1; var v2={\"label\": \"value\"};");
        std::cout<<data["v1"]<<std::endl;
        std::cout<<data["v2"]["label"]<<std::endl;

# Referencing

Being able to create references is fundamental to the functionality of the Asuka
Type Library.  The `AT::Field` type utilizes reference counting to allow for
multiple instances to point at the same data.  Creating a reference to a
`AT::Field` is done by assigning a pointer to an existing instance.  In the
following example any assignment to `foo` or `bar` will result in the other
being changed as well, since they both reference the same data storage instance.
The data storage will not be released until both `foo` and `bar` have gone out
of scope.

    AT::Field foo;
    AT::Field bar = &foo;

The referencing functionality also extends to `AT::Collection` indexes.  In the
following example, an internal reference to a node was created.  This
relationship is preserved in the copy of `foo` to `bar`.  Any modification of
`bar[0]` will affect `bar[1]`, unless assigning an AT::Field instance.  Note
the assignment from `foo` to `bar` does an actual copy.

    AT::Field foo;
    foo[0] = &foo[1];
    AT::Field bar = foo;

Another example, showing an intermediate `data` variable.  If `data` is
assigned a value, that will affect both `foo[0]` and `foo[1]`.  The `bar`
collection instance will reference an anonymous AT::Field instance for both
indexes 0 and 1.

    AT::Field data;
    AT::Field foo;
    foo[0] = &data;
    foo[1] = &data;
    AT::Field bar = foo;

Another important aspect of references is their use as function call arguments.
The following pushes the `data` field onto the list.  Any modification of that
element in the list will also change the `data` instance.

    AT::Field data;
    AT::Field list;
    list.push(&data);

# Manipulators

The manipulation routines provide two ways to perform the operations, either as
member methods or external methods.  The member methods make modifications to
the instance, while the external methods provide the results in a new
`AT::Field` instance.  The only exception being the `base` method, which
behaves like an external method.

## Member methods

- base - `AT::Field instance.base(AT::Integer32 target_ordinal, AT::Integer32 current_ordinal=10)`

    This method provides an easy way to convert a numeric value to an `AT::Field`
    in the specified ordinal.

        AT::Field number = 256;
        std::cout<<number.base(16)<<std::endl;

- append - `void instance.append(AT::Character *in)`

    Appends the specified string to the `AT::Field` instance.

- uc - `void instance.uc()`

    Changes all alphabetical characters to upper case.

- lc - `void instance.lc()`

    Changes all alphabetical characters to lower case.

- chomp - `void instance.chomp(AT::Boolean whitespace=false)`

    Removes all \\n or \\r characters from the end of the string.

## External methods

- substitute - `AT::Field substitute(AT::Field in_buffer, const AT::Character *expression, const AT::Character *replacement, AT::Boolean global=false)`

    The substitute routine allows for one or all of the `expression` matches to be
    replaced with the replacement string.

- substitute - `AT::Field substitute(AT::Field in_buffer, AT::Field &replacement)`

    This is a less generic substitute method that takes a buffer and list of
    replacement values.  The replacements are put in place of enumerated strings
    like %0, %1, %2... %n (where n is the index into the replacement list).

        AT::Field values;
        values[0] = "Asuka";
        values[1] = "Type";
        values[10] = "Library";
        std::cout<<AT::substitute("%0 %1 %10", values)<<std::endl;

- split - `AT::Field split(const char *pattern, AT::Field, AT::Integer32)`

    Returns a list of items delimited by the pattern, for as many as specified by
    the limit (default is to split on all matches).

- join - `AT::Field join(const char *, AT::Field)`

    Provides a single scalar value of the list items provided separated by the
    delimiter.

- uc - `AT::Field uc(const AT::Character *string)`

    Changes all alphabetical characters to upper case.

- lc - `AT::Field lc(const AT::Character *string)`

    Changes all alphabetical characters to lower case.

- chomp - `AT::Field chomp(const AT::Character *string, AT::Boolean whitespace=false)`

    Removes all \\n or \\r characters from the end of the string.

# Regular Expressions

This regular expression implementation supports most of the regex functionality
that is defined in the extended posix set.  Like PCRE, the behavior deviates with
a preference to perl functionality and rules.

    Fundamentals
    [] character set
    () grouping
    |  alternation
    .  any single character
    ^  anchor front
    $  anchor back
    ?  greedy

    Multiplicity
    {} specified
    *  0 or more
    +  1 or more
    ?  0 or 1

    Escape sequences
    \s  whitespace
    \S  non-whitespace
    \w  word characters (alphanumeric and underscore)
    \W  non-word characters
    \d  digits

    Escape characters
    \   backslash
    `   backtick

- rx - `AT::Field rx(const AT::Character *pattern, const AT::Character *)`

    Returns the results of the compile/match.  The results are determined from the
    grouping provided by the compiled pattern.  Counting is done as expected,
    incremental for each open paren encountered '('.  For example
    `"AB" =~ m/(A(B))/` results with AB=0 and B=1.  A Regular Expression cache
    is kept where expressions are compiled once and reused when duplicate patterns
    are specified.  Alternately an `AT::RegEx` instance can be created using the 
    compile/execute or compile/match member methods.

    Also remember that C/C++ literal strings need to escape backslashes, so to
    specify a digit escape sequence two backslashes are required, `"\\d"`.  Or to 
    specify a literal backslash `"\\\\"`.  As an alternative, the backtick character
    can be used for escape sequences so `"\\d"` becomes `` "`d" `` and `"\\\\"`
    becomes `` "`\" ``.  This can be really handy when working with regular expressions
    for windows directory paths.

    The following example shows the list context results for matching.  If a match
    is undefined, the prior value is not unset.

        AT::Field protocol, host;
        AT::Field port = 80;
        (protocol, host, port) = AT::rx("([^:]+)://([^:]+)(:(`d+))?/", url);

- compile - `AT::Integer32 &instance.compile(const AT::Character *)`

    Parses the pattern and builds the expression state tree.

- match - `AT::Field instance.match(const AT::Character *)`

    Returns a list of matched items, same as the AT::rx method.

# Filesystem

## Miscellaneous

The following routines are OS independent methods for performing typical
filesystem operations.

- rm - `AT::Boolean rm(const AT::Character *filename)`

    Removes the specified file.

- mv - `AT::Boolean mv(const AT::Character *src, const AT::Character *dest)`

    Moves the specified file from the source to the destination.  This should be
    an atomic operation, but it is dependant on the OS.

- cp - `AT::Boolean cp(const AT::Character *src, const AT::Character *dest)`

    Copy the specified file to the target.  This routine only supports making file
    copies where the source and destination are on the same physical partition.

- mkdir - `AT::Boolean mkdir(const AT::Character *path, AT::Field mask)`

    Creates the specified path.  Currently does not create recursively.

- rmdir - `AT::Boolean rmdir(const AT::Character *path)`

    Removes the specified directory.  The directory must be empty.

- pwd - `AT::Field pwd(const AT::Character *path=NULL)`

    Returns the present working directory.  If a path is provided, the present
    working directory will be changed if possible.

- stat - `AT::Field stat(const AT::Character *path)`

    Returns a list of stats for the specified path provided.

        0,  inode (non-windows)
        1,  dev
        2,  mode
        3,  nlink (non-windows)
        4,  uid (non-windows)
        5,  gid (non-windows)
        6,  rdev (non-windows)
        7,  size
        8,  access
        9,  modified
        10, creation
        11, block size (non-windows)
        12, blocks (non-windows)

- lstat - `AT::Field lstat(const AT::Character *path)`

    Same as stat, except it will not traverse links but instead return information
    about the links themselves.

- filetype - `AT::Field filetype(const AT::Character *path)`

    Returns a string value of one of the following, describing the type of file
    specified.

        regular
        directory
        character
        block
        fifo
        link
        socket

## AT::File

- constructor - `AT::File([const AT::Character *}])`

    Creates a new AT::File instance.

- cast - `(AT::Field)instance`

    This is the easiest method for file processing.  An `AT::Field` is provided
    with a list of all the contents the file, one entry per line (\\n or \\r\\n
    delimited).

        AT::Field line;
        AT::Field counter = 0;
        foreach(line, AT::File("/home/codeneko/foo.txt")) {
          line.chomp();
          std::cout<<"line ("<<++counter<<"): '"<<line<<"'"<<std::endl;
        }

- extract - `AT::Unsigned32 instance.extract(AT::Unknown *, size_t)`

    Takes a pointer to a pre-allocated buffer and extracts the specified number of
    bytes either from the memory cache or off disk.

- extract - `AT::Field instance.extract(AT::Byte)`

    Returns an AT::Field instance with data starting at the current file pointer to
    the specified delimiter character.

- extract - `AT::Field instance.extract(enum AT::storage_types)`

    Returns an AT::Field instance with data starting at the current file pointer to
    the size of the type enumeration provided.

- size - `AT::Unsigned32 instance.size()`

    Returns the size of the file in bytes.

- getpos - `AT::Unsigned32 instance.getpos()`

    Returns the current position of the file pointer.

- setpos - `AT::Unsigned32 instance.setpos(AT::Unsigned32)`

    Sets the file pointer to the position specified.

## AT::Directory

- constructor - `AT::Directory([const AT::Character *])`

    Creates a new AT::Directory instance.

- cast - `(AT::Field)instance`

    This is the easiest method for directory listing.  An `AT::Field` is provided
    with a list of all the contents the directory instance references.

        AT::Field contents = AT::Directory("/home/codeneko");

- next - `AT::Field instance.next()`

    An alternate directory iteration method, doesn't require a full directory read.

## AT::Archive

The zip archive abstraction is only provided if the zlib library is available
at build time.  Some of the AT::Path functionality is dependant on
`AT::Archive`.

- source - `AT::Boolean instance.source(AT::Character *)`

    Sets the source zip archive file.

- list - `AT::Field instance.list()`

    Returns a list of file headers for the contents of the archive file.

- locator - `AT::Field instance.locator(AT::Character *)`

    Returns the header for the specified file in the archive.

        AT::Archive temporary(archive);
        AT::Field header = temporary.locator(relative);
        results = temporary.extract(header);

- extract - `AT::File instance.extract(AT::Field)`

    Returns an `AT::File` instance that contains the contents of the uncompressed
    file from the archive.

## AT::Path

- relative - `AT::Boolean Path::relative(const AT::Character *)`

    Returns true if the provided path is a relative path, and false if it is
    absolute.

- open - `AT::File Path::open(const AT::Character *)`

    This routine performs a search for the file specified.  If a zip archive
    is present with the same name as a node in the directory hierarchy, it will be
    examined for a file with a similar relative path from that point.  Assuming a
    zip archive exists call /home/codeneko/test.zip and it contains a file named
    foo/bar.txt, then the following will open it (unless the file
    /home/codeneko/test/foo/bar.txt actually exists, then it will be opened
    instead).

        AT::File text = AT::Path::open("/home/codeneko/test/foo/bar.txt");

    This allows packaged resources to be provided while still allowing user
    overrides for individual components.

- split - `AT::Field Path::split(const AT::Character *)`

    Specialized path splitting routine.  Returns the device as the first element in
    the list.

- delimiter - `AT::Field Path::delimiter(const AT::Character *)`

    Returns the OS specific delimiter for the path provided.

- basename - `AT::Field Path::basename(const AT::Character *)`

    Returns just the last element of an OS independent split operation on the path
    provided.

- dirname - `AT::Field Path::dirname(const AT::Character *)`

    Returns the first portion of the provided path, minus the basename component.

- resolve - `AT::Field Path::resolve(AT::Field relative, AT::Field current)`

    Resolves relative paths.

# Document Object Model

The DOM implementation provided is functional, but not complete.
Internally the document is just an `AT::Field` tree.  The following
field members might be available depending on the node type.

    node["nodeType"]      - An enumerated value for the type of node.
    node["nodeName"]      - The name of the node.
    node["nodeId"]        - A unique node ID.
    node["nodeValue"]     - The value for the node (CDATA, Comment, Text)
    node["ownerDocument"] - A reference to the owner document

A Document node will also have the following to indicate the root
document node.

    m_node["documentElement"] - The root document node (Document)

## AT::DOM::Document

- createAttribute - `AT::DOM::Node createAttribute(AT::Field name)`

    Creates an attribute node.

- createCDATASection - `AT::DOM::Node createCDATASection(AT::Field data)`

    Creates a CDATA node.

- createComment - `AT::DOM::Node createComment(AT::Field data)`

    Creates a comment node.

- createElement - `AT::DOM::Node createElement(AT::Field name)`

    Creates an element node.

- createTextNode - `AT::DOM::Node createTextNode(AT::Field data)`

    Creates an text node.

- loadXML - `loadXML(AT::Field xml)`

    Loads the document with the XML specified via `AT::Field`.

- serialize - `AT::Field serialize(AT::Boolean format, AT::Field options`

    Returns back a single text field with the contents of the document.  If the format
    option is used, indentation will be used for children and newlines for each node.
    Some logic is used to determine if text nodes will be left on a single line when
    using the format option.  If the text block does not contain newlines, it will
    colapse onto a single line.

## AT::DOM::Node

- appendChild - `AT::DOM::Node &node.appendChild(AT::DOM::Node &)`

    Appends a child node.

- setAttribute - `AT::DOM::Node &node.setAttribute(AT::Field name, AT::Field value)`

    Creates an attribute node and sets the value.  The attribute node is appended
    to the node.

- setAttributeNode - `AT::DOM::Node &node.setAttributeNode(AT::DOM::Node &)`

    Appends the provided attribute node.

- hasChildNodes - `AT::Boolean node.hasChildNodes()`

    Returns true if the node has children.

- selectSingleNode - `AT::Field node.selectSingleNode(AT::Field xpath)`

    Returns a single node specified by the xpath provided.

- selectNodes - `AT::Field node.selectNodes(AT::Field xpath)`

    Returns a list of nodes specified by the xpath provided.

# No documentation

The following are functional components of the Asuka Type Library, but have yet
to be documented.  Please refer to the doxygen dump for the supported APIs.

## AT::Configuration

## AT::DB

## AT::Resource

# Future Enhancements

## Documentation

The documentation provided for Asuka is lacking in many details.  There are
many subtle behaviors that should be presented.  As an alternative it may
be sufficient to look over code in the library that makes heavy use of the 
existing features to see how things can be done.

## Wide Character Support

There are several places in Asuka that should support Wide Characters, either
UTF8 or otherwise.  Some of the specifications for functionality provided by
Asuka even mandate it.  Probably the most significant would be `AT::Field`
and `AT::DOM`.

# Author

Dee E. Abbott

# References

The following are links to specifications, rfcs, and dependencies of the
asuka library.  It is not complete.

- JSON - [http://www.json.org](http://www.json.org)
- DOM - [http://www.w3.org/TR/REC-DOM-Level-1](http://www.w3.org/TR/REC-DOM-Level-1)
- XPath - [http://www.w3.org/TR/xpath](http://www.w3.org/TR/xpath)
- Lua - [http://www.lua.org](http://www.lua.org)
- zlib - [http://www.zlib.net](http://www.zlib.net)
- freetype - [http://www.freetype.org](http://www.freetype.org)
- regex - [http://en.wikipedia.org/wiki/Regular\_expression](http://en.wikipedia.org/wiki/Regular_expression)

# License

    Copyright (c) 2005, Dee E. Abbott
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the organization nor the names of its contributors may
      be used to endorse or promote products derived from this software without
      specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <gras/block.hpp>
#include <gras/hier_block.hpp>
#include <gras/top_block.hpp>
%}

//======================================================================
//== bring in the dependency headers
//======================================================================
%include <grextras/config.hpp>
%include <gras/exception.i>
%include <gras/element.i>
%import <gras/block.i>
%import <gras/hier_block.i>

//======================================================================
//== a simple definition for a boost shared pointer
//======================================================================
namespace boost{template<class T>struct shared_ptr{T*operator->();};}

%extend boost::shared_ptr<gras::Block>
{
    %insert("python")
    %{
        def __str__(self):
            return self.to_string()

        def set(self, key, value):
            from PMC import PMC_M
            self._set_property(key, PMC_M(value))

        def get(self, key):
            return self._get_property(key)()
    %}
}

//======================================================================
//== GREXTRAS_SWIG_FOO - a simple macro to export shared ptr of class
//== And to create a pythonic interface for the factory functions
//======================================================================
%define GREXTRAS_SWIG_FOO(MyClass)

%template(grextras_ ## MyClass) boost::shared_ptr<grextras::MyClass>;

%extend boost::shared_ptr<grextras::MyClass>
{
    %insert("python")
    %{
        def __str__(self):
            return self.to_string()
    %}
}

%pythoncode %{

#remove the make_ from the factory functions, it feels more pythonic
for attr in filter(lambda x: x.startswith('make_'), dir(MyClass)):
    make = getattr(MyClass, attr)
    setattr(MyClass, attr[5:], make)
if hasattr(MyClass, 'make'):
    globals()[#MyClass] = MyClass.make

try: len(__all__)
except: __all__ = list()

__all__.append(#MyClass)

%}

%enddef

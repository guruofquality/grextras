// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%module "grextras"
%newobject *::make;

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
%import <gras/element.i>
%import <gras/block.i>
%import <gras/hier_block.i>
%import <gras/top_block.i>

//======================================================================
//== a simple definition for a boost shared pointer
//======================================================================
namespace boost{template<class T>struct shared_ptr{T*operator->();};}

%template(grextras_Block) boost::shared_ptr<gras::Block>;
%pythoncode %{
from PMC import *
%}
%extend boost::shared_ptr<gras::Block>
{
    %insert("python")
    %{
        def __str__(self):
            return self.to_string()

        def x(self, key, *args):
            pmcargs = PMC_M(list(args))
            pmcret = self._handle_call(key, pmcargs)
            return pmcret()
    %}
}

//======================================================================
//== GREXTRAS_SWIG_FOO - a simple macro to export shared ptr of class
//== And to create a pythonic interface for the factory functions
//======================================================================
%define GREXTRAS_SWIG_FOO(MyClass)

%template(grextras_ ## MyClass) boost::shared_ptr<grextras::MyClass>;
%pythoncode %{
from PMC import *
%}
%extend boost::shared_ptr<grextras::MyClass>
{
    %insert("python")
    %{
        def __str__(self):
            return self.to_string()

        def x(self, key, *args):
            pmcargs = PMC_M(list(args))
            pmcret = self._handle_call(key, pmcargs)
            return pmcret()

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

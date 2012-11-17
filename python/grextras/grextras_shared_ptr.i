// Note: I really want to use boost_shared_ptr.i and %shared_ptr
// But, while it works great in general, it messes up the type
// identification such that connect doesnt recongnize Element.
// http://www.swig.org/Doc2.0/SWIGDocumentation.html#Library_std_shared_ptr
// And this maybe could have made it better, but no idea...
// http://www.swig.org/Doc2.0/SWIGDocumentation.html#Modules

//
//  shared_ptr
//
//  An enhanced relative of scoped_ptr with reference counted copy semantics.
//  The object pointed to is deleted when the last shared_ptr pointing to it
//  is destroyed or reset.
//

//
// This is highly hacked up version of boost::shared_ptr
// We just need enough to get SWIG to "do the right thing" and
// generate "Smart Pointer" code.
//

namespace boost {

template<class T> class shared_ptr
{
public:

    shared_ptr()
    {
    }

    shared_ptr (T * p)
    {
    }


    T * operator-> () // never throws
    {
        return px;
    }


private:

    T * px;                     // contained pointer
    int pn;

};  // shared_ptr

};

///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************(
*** \file    singleton.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the singleton code.
***
*** This class permits to turn a normal class into a singleton
*** ***************************************************************************/

#ifndef __SINGLETON_HEADER__
#define __SINGLETON_HEADER__

//! Contains utility code used across the entire source code
namespace vt_utils
{

/** ****************************************************************************
*** \brief Used for transforming a standard class into a singleton class
***
*** This is a templated abstract class which classes may derive from to become
*** singleton classes. To create a new singleton type class, follow the steps below.
*** It is assumed that the desired class is called "ClassName".
***
*** -# In the header file, define the class as follows: class ClassName : public vt_utils::Singleton<ClassName>
*** -# Make vt_utils::Singleton<ClassName> a friend of ClassName in the header file
*** -# Put the ClassName() constructor in the private section of the class, and the destructor in the public section
*** -# Define the following function in the public section of the class and implement it: bool SingletonInitialize()
*** -# In the source file, set the static template member like so: template<> ClassName* Singleton<ClassName>::_singleton_reference = nullptr
***
*** With this done, your new class should be ready to go. To create and use a singleton class, do the following:
***
*** -# Call ClassName* SingletonCreate() to get a pointer to the new singleton class instance
*** -# After the singleton object has been created, next call bool SingletonInitialize() to initialize the class
*** -# If you ever need to retreive a pointer for a singleton you've created, just call the static method const ClassName* SingletonGetReference()
*** -# Call SingletonDestroy to destroy the class and its underlying singleton. You can then create and initialize the singleton once more if you'd like.
***
*** \note The creation steps listed above are the only way to properly construct a singleton class object. Don't try to circumvent it, and never attempt to
*** modify the protected static member Singleton::_singleton_reference directly, except as instructed above.
***
*** \note Sometimes singleton classes need to refer to each other to initialize themselves, particularly with game engine components. That is the purpose
*** of the SingletonInitialize() method, so that all the singleton objects can be created and then reference each other when this method is invoked. It
*** can be viewed as a helper function to the class constructor.
***
*** \note For engine singleton classes, SingletonCreate(), SingletonDestroy(), and SingletonInitialize()
*** should only be called in main.cpp before the main game loop. There may be qualified exceptions to this
*** practice, however.
***
*** \note Most of our singleton classes also define a pointer to their singleton object inside the
*** source file of the class. For example, the AudioEngine singleton contains the AudioManager class object
*** name inside the vt_audio namespace. Therefore you do not need to call the SingletonGetReference()
*** function when this object is made available.
*** ***************************************************************************/
template<typename T> class Singleton
{
protected:
    //! \brief A reference to the singleton class instance itself
    static T *_singleton_reference;

    Singleton()
    {}

    virtual ~Singleton()
    {}

public:
    //! \brief Creates and returns an instance of the singleton class
    static T *SingletonCreate() {
        if(_singleton_reference == nullptr)
            _singleton_reference = new T();
        return _singleton_reference;
    }

    //! \brief Destroys the singleton class instance
    static void SingletonDestroy() {
        if(_singleton_reference != nullptr)
            delete _singleton_reference;
        _singleton_reference = nullptr;
    }

    //! \brief Returns a pointer to the singleton class instance (or nullptr if the class is not instantiated)
    static const T *SingletonGetReference() {
        return _singleton_reference;
    }

    /** \brief A method for the inheriting class to implement, which initializes the class
    *** \return True if initialization was successful, false if it was not
    **/
    virtual bool SingletonInitialize() = 0;

private:
    Singleton(const Singleton &s);
    Singleton &operator=(const Singleton &s);
}; // template<typename T> class Singleton

template<typename T>
T* Singleton<T>::_singleton_reference = nullptr;

} // namespace vt_utils

#endif // __SINGLETON_HEADER__

/*
 * Singleton.hpp
 *
 *  Created on: 2013-07-05
 *      Author: Jonathan Dumaresq
 *      Update: Rémi Boucher
 */

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

/**
 * @class TSingleton singleton.hpp
 * @brief Singleton template class.
 *
 * To use a class as a singleton, it should inherit this class
 * as shown in the example below. Doing so will make sure only
 * one instance of the specified class is ever created.
 *
 *
 * Usage example:
 * @code
 * #include "singleton.hpp"
 *
 * class Child : public TSingleton<Child>
 * {
 * 		friend class TSingleton<Child>;
 * 	private:
 * 		Child();
 * 		~Child();
 * 		Child(const Child&);
 * 		void operator=(const Child&);
 *
 * 	public:
 * 		//...//
 * }
 * @endcode
 *
 * @note Singleton should be avoided when possible. Only use one when
 * you MUST have access to a single instance of a class anywhere
 * in the program. If you only need access to unique members of
 * a class, consider using static variables.
 */
template <typename T>
class TSingleton
{
protected:
    /**
     * @brief Constructor.
     */
	TSingleton(void)
	{
	}

	/**
	 * @brief Destructor.
	 */
	~TSingleton()
	{
	}

public:
	/**
	 * @brief Returns a pointer to a single instance of the class.
	 *
	 */
	static T *getInstance(void)
	{
		static T _singleton;

		return &_singleton;
	}
};

#endif // SINGLETON_HPP

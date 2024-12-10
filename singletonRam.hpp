/*
 * singletonRam.hpp
 *
 *  Created on: 2018-11-23
 *      Author: sg
 */

#ifndef SINGLETONRAM_HPP_
#define SINGLETONRAM_HPP_

/**
 * @class TSingletonRam singletonRam.hpp
 * @brief SingletonRam template class.
 *
 * To use a class as a singleton, it should inherit this class
 * as shown in the example below. Doing so will make sure only
 * one instance of the specified class is ever created.
 *
 *
 * Usage example:
 * @code
 * #include "singletonRam.hpp"
 *
 * class TRam : public TSingletonRam<TRam>
 * {
 * 		friend class TSingleton<TRam>;
 * 	private:
 * 		TRam(TScreen *);
 * 		~TRam();
 * 		TRam(const TRam&);
 * 		void operator=(const TRam&);
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

#include "screen.hpp"

template <typename T>
class TSingletonRam
{
protected:
    /**
     * @brief Constructor.
     */
	TSingletonRam(void)
	{
	}

	/**
	 * @brief Destructor.
	 */
	~TSingletonRam()
	{
	}

public:
	/**
	 * @brief Returns a pointer to a single instance of the class.
	 *
	 */
	static T *getInstance(TScreen *screen)
	{
		static T _singleton(screen);

		return &_singleton;
	}
};

#endif /* SINGLETONRAM_HPP_ */

/*!	\file Exception.h
*
*	\brief Header file for the Exception class.
*
*	\author Gregory Diamos
*
*
*
*
*/

#ifndef EXCEPTION_H_INCLUDED
#define EXCEPTION_H_INCLUDED

#include <exception>
#include <sstream>
#include <assert.h>


/*!
	\brief a namespace for common classes and functions
*/


namespace common
{

	/*!
	
		\brief An Exception with a variable message
	
	*/
	class Exception : public std::exception
	{
	
		public:
		
			std::string message;
			int code;
		
		public:
	
			Exception( const std::string& message, int code = 0 );
			~Exception() throw();
			const char* what() const throw();
			
	};

	
}//ext

#endif
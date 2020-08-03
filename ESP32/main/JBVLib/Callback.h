/*
 * Callback.h
 *
 *  Created on: Nov 27, 2019
 *      Author: Bas
 */

#ifndef CALLBACK_H_
#define CALLBACK_H_


template<typename R, typename ...Args>
class CallbackHandler
{
public:
	CallbackHandler(){}
	virtual ~CallbackHandler(){}
	virtual R Invoke(Args... args) = 0;
};


template<typename T, typename R, typename ...Args>
class CallbackHandlerMethod : public CallbackHandler<R, Args...>
{
private:
	R(T::*method)(Args...);
	T* methodInstance;

public:

	CallbackHandlerMethod<T, R, Args...>(T* instance, R(T::*memberFunctionToCall)(Args...))
	{
		method = memberFunctionToCall;
		methodInstance = instance;
	}

	R Invoke(Args... args)
	{
		return (methodInstance->*method)(args...);
	}
};


template<typename R, typename ...Args>
class CallbackHandlerFunction : public CallbackHandler<R, Args...>
{
private:
	R(*func)(Args...);

public:
	CallbackHandlerFunction<R, Args...>( R(*functionCall)(Args...))
	{
		func = functionCall;
	}

	R Invoke(Args... args)
	{
		return (*func)(args...);
	}
};



template<typename R, typename ...Args>
class Callback
{
	CallbackHandler<R, Args...> *callback = 0;

public:

	template<typename T>
	Callback(T* instance, R(T::*method)(Args...))
	{
		callback = new CallbackHandlerMethod<T, R, Args...>(instance, method);
	}


	Callback(R(*func)(Args...))
	{
		callback = new CallbackHandlerFunction<R, Args...>(func);
	}

	R operator()(Args... args)
	{
		//todo figure out a way to do a null check. (maybe return a pointer of the result that could be NULL)
		return callback->Invoke(args...);
	}


};




#endif

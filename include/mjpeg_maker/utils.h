/*
 * utils.h
 *
 *  Created on: Apr 24, 2020
 *      Author: hasan
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <string>
#include <exception>
#include <sstream>
#include <string.h>
#include <iostream>

#include <map>
#include <memory>

using namespace std;

#define THROW(exceptionClass, message) throw exceptionClass(__FILE__, \
__LINE__, (message) )

namespace mjpeg_maker {



namespace mjpeg_maker {

	class RobotException : public std::exception
	{
		// Disable copy constructors
		RobotException& operator=(const RobotException&);
		std::string file_;
		int line_;
		std::string e_what_;
		int errno_;
	public:
		explicit RobotException(std::string file, int line, int errnum)
			: file_(file), line_(line), errno_(errnum) {
			std::stringstream ss;
#if defined(_WIN32) && !defined(__MINGW32__)
			char error_str[1024];
			strerror_s(error_str, 1024, errnum);
#else
			char * error_str = strerror(errnum);
#endif
			ss << "Robot Exception (" << errno_ << "): " << error_str;
			ss << ", file " << file_ << ", line " << line_ << ".";
			e_what_ = ss.str();
		}
		explicit RobotException(std::string file, int line, const char * description)
			: file_(file), line_(line), errno_(0) {
			std::stringstream ss;
			ss << "Robot Exception: " << description;
			ss << ", file " << file_ << ", line " << line_ << ".";
			e_what_ = ss.str();
		}
		virtual ~RobotException() throw() {}
		RobotException(const RobotException& other) : line_(other.line_), e_what_(other.e_what_), errno_(other.errno_) {}

		int getErrorNumber() const { return errno_; }

		virtual const char* what() const throw () {
			return e_what_.c_str();
		}

	};

}

class AbstractInstanceContainer
{
public:
    AbstractInstanceContainer() = default;
    virtual ~AbstractInstanceContainer() = default;

    AbstractInstanceContainer& operator=(AbstractInstanceContainer&& other) noexcept
    {
        mRawPointer = other.mRawPointer;
        other.mRawPointer = nullptr;

        return *this;
    }

    void* get() { return mRawPointer; }

protected:
    explicit AbstractInstanceContainer(void* ptr) : mRawPointer(ptr) {}

private:
    void* mRawPointer = nullptr;
};


template <class T>
class InstanceContainer : public AbstractInstanceContainer
{
public:
    explicit InstanceContainer(std::unique_ptr<T> ptr) : AbstractInstanceContainer(ptr.get()), mPointer(std::move(ptr)) {}
    ~InstanceContainer() override = default;

private:
    std::unique_ptr<T> mPointer;
};


class ServiceContainerException
{
public:
    explicit ServiceContainerException(std::string&& message) : message(message) {}

    std::string message;
};

class ServiceContainer
{
public:

    template <typename T, typename... Deps, typename... Args>
    void set(Args... args)
    {
        auto instance = std::make_unique<T>(get<typename std::remove_const<Deps>::type>()..., args...);
        std::unique_ptr<InstanceContainer<T>> service = std::make_unique<InstanceContainer<T>>(std::move(instance));
        mContainer[typeId<T>()] = std::move(service);
    }

    template <typename T>
    T* get()
    {
        auto it = mContainer.find(typeId<T>());
        if (it == mContainer.end()) {
            throw ServiceContainerException(std::string("Service '" + std::string(typeid(T).name()) + "' not registered in container."));
        }
//        assert (it != mContainer.end());

        return static_cast<T*>(it->second->get());
    }

private:
    template <typename T>
    int typeId()
    {
        static int id = ++mLastTypeId;

        return id;
    }

private:
    static int mLastTypeId;
    std::map<int, std::unique_ptr<AbstractInstanceContainer>> mContainer;
};




/***********************************************************************************************************/

#include <iostream>

struct Service1
{
    int alpha = 0;

    ~Service1() {
        std::cout << "Service 1 deleted" << std::endl;
    }
};


struct Service2
{
    explicit Service2(Service1* s1) : s1(s1)
    {
    	//std::cout << "get param " << k << std::endl;
    }

    int value() { return s1->alpha; }

    Service1* s1;

    ~Service2() {
        std::cout << "Service 2 deleted" << std::endl;
    }
};

struct Service3
{
    Service3(Service2* s2, Service1* s1)
    {

    }
};

}
#endif /* SRC_UTILS_H_ */

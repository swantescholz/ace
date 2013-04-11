#ifndef ACE_SINGLETON_H_
#define ACE_SINGLETON_H_

#include <memory>

namespace ace {

template <typename T>
class Singleton {
protected:
	static std::shared_ptr<T> instance;
	Singleton() {}
private:
	Singleton(const Singleton& other) = delete;
	Singleton& operator= (const Singleton& other) = delete;
public:
	static T& getInstance() {
		if (instance.get() == nullptr)
			instance.reset(new T());
		return *(instance.get());
	}
		
};

template<typename T> std::shared_ptr<T> Singleton<T>::instance;

} // namespace ace




#endif

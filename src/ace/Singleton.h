#ifndef ACE_SINGLETON_H_
#define ACE_SINGLETON_H_

#include <memory>

namespace ace {

template <typename T>
class Singleton {
protected:
	Singleton() {}
	virtual ~Singleton() {}
private:
	Singleton(const Singleton& other) = delete;
	Singleton& operator= (const Singleton& other) = delete;
public:
	static T& getInstance() {
		static T instance;
		return instance;
	}
		
};

} // namespace ace




#endif

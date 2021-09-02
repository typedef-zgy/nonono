#ifndef DMNONCOPYABLE_H
#define DMNONCOPYABLE_H

class DMNonCopyable {
protected:
	DMNonCopyable() = default;
	~DMNonCopyable() = default;

	DMNonCopyable(const DMNonCopyable& obj) = delete;
	DMNonCopyable& operator=(const DMNonCopyable& obj) = delete;
};

#endif	// DMNONCOPYABLE_H

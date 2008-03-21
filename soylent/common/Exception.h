#ifndef Exception_h
#define Exception_h

namespace bburdette
{

class Exception
{
private:
	unsigned int mUi32ExceptionCode;
public:
	Exception(unsigned int aUi32Code = MULTICHAR('e','x','c','p'))
		:mUi32ExceptionCode(aUi32Code)
	{
	}

	inline const unsigned int& GetCode() { return mUi32ExceptionCode; }

	virtual const char* GetMessage() const { return ""; }
};

class MessageException : public Exception
{
public:
	enum { MaxLen = 255 };
private:
	char mCMessage[MaxLen];
	size_t mILen;
public:
	MessageException(const char *aC, unsigned int aUi32Code = MULTICHAR('e','x','c','p'))
		:Exception(aUi32Code)
	{
		strncpy(mCMessage, aC, MaxLen);
		mILen = strlen(mCMessage);
	}

	MessageException& operator+=(const char *aC)
	{
		strncat(mCMessage, aC, MaxLen-mILen);
		mILen = strlen(mCMessage);

		return *this;
	}

	virtual const char* GetMessage() const { return mCMessage; }
};

}	// namespace bburdette

#endif

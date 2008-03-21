#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include "config.h"
#include "vec.h"
#include <list>

class Message : public gc
{
public:
    Message(const tvec& ref) : ref(ref) { }
    virtual ~Message() { }

    tvec ref;
};


class MessageNode;


class MessageTracker : public gc
{
    friend class MessageList;
public:
    Message* poll();
private:
    MessageTracker(MessageNode* tail) : cur_(tail) { }
    MessageNode* cur_;
};


class MessageList : public gc
{
public:
    MessageList();
    void send(Message* msg);
    MessageTracker* tracker() const;
private:
    MessageNode* tail_;
};


class MessagePendingQueue : public gc
{
public:
    MessagePendingQueue(MessageTracker* tracker) : tracker_(tracker) { }
    Message* poll(const tvec& ref);
private:
    MessageTracker* tracker_;
    typedef std::list< Message*, gc_allocator<Message*> > messages_t;
    messages_t messages_;
};



namespace Messages
{

struct ChangeDirection : public Message {
    ChangeDirection(const tvec& ref, const vec& direction)
        : Message(ref), direction(direction)
    { }

    vec direction;
};

}


#endif

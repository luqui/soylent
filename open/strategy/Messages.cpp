#include "Messages.h"

struct MessageNode
{
    MessageNode(MessageNode* next, Message* msg) 
        : next(next), msg(msg)
    { }
    
    MessageNode* next;
    Message* msg;
};


MessageList::MessageList()
    : tail_(new MessageNode(0, 0))
{ }

void MessageList::send(Message* msg)
{
    MessageNode* node = new MessageNode (0, msg);
    tail_->next = node;
    tail_ = node;
}

MessageTracker* MessageList::tracker() const
{
    return new MessageTracker(tail_);
}


Message* MessageTracker::poll()
{
    if (cur_->next) { 
        cur_ = cur_->next;
        return cur_->msg;
    }
    else {
        return 0;
    }
}


Message* MessagePendingQueue::poll(const tvec& ref) 
{
    while (Message* msg = tracker_->poll()) {
        messages_.push_back(msg);
    }
    
    for (messages_t::iterator i = messages_.begin(); 
            i != messages_.end(); ++i) {
        num interval = ((*i)->ref - ref).norm2();
        if (interval >= 0) {
            Message* msg = *i;
            messages_.erase(i);
            return msg;
        }
    }
    return 0;
}

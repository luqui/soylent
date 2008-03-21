#ifndef __BOARD_H__
#define __BOARD_H__

#include "config.h"
#include "Actor.h"
#include "Draw.h"
#include <list>

class Board : public gc {
    typedef std::list< Actor*, gc_allocator<Actor*> > actors_t;
public:
    typedef actors_t::const_iterator const_iterator;

    void add_actor(Actor* actor) { actors_.push_back(actor); }
    void step();
    
    const_iterator actors_begin() const { return actors_.begin(); }
    const_iterator actors_end()   const { return actors_.end(); }
private:
    actors_t actors_;
};

class BoardViewIterator : public gc {
public:
    BoardViewIterator(const Board::const_iterator& end,
                      const Board::const_iterator& in,
                      const tvec& ref) 
            : end_(end), in_(in), ref_(ref) { 
        advance();
    }
    BoardViewIterator& operator ++ ()    { 
        ++in_;
        advance();
        return *this; 
    }
    BoardViewIterator  operator ++ (int) { 
        BoardViewIterator me = *this; ++*this; return me; 
    }

    const ActorState* operator*  () const { return cur_; }

    bool operator == (const BoardViewIterator& o) const { 
        return in_ == o.in_;
    }
    bool operator != (const BoardViewIterator& o) const {
        return !(*this == o);
    }
private:
    const ActorState* cur_;
    Board::const_iterator end_, in_;
    tvec ref_;

    void advance() {
        while (in_ != end_ && !(cur_ = (*in_)->find_state(ref_))) ++in_;
    }
};

class BoardView : public gc {
public:
    typedef BoardViewIterator const_iterator;
    BoardView(Board* board, const tvec& ref) : board_(board), ref_(ref) { }

    const_iterator units_begin() const { 
        return BoardViewIterator(board_->actors_end(), board_->actors_begin(), ref_); 
    }
    const_iterator units_end()   const { 
        return BoardViewIterator(board_->actors_end(), board_->actors_end(), ref_); 
    }

    void draw(Artist* artist);
    
private:
    Board* board_;
    tvec ref_;
};

#endif

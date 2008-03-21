#include "Board.h"

void Board::step() {
    for (const_iterator i = actors_begin(); i != actors_end(); ++i) {
        (*i)->step();
    }
}

void BoardView::draw(Artist* artist) {
    for (const_iterator i = units_begin(); i != units_end(); ++i) {
        (*i)->draw(artist, ref_);
    }
}

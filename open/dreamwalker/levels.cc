#include "levels.h"
#include "Level.h"
#include "Dude.h"
#include <GL/gl.h>
#include <GL/glu.h>

class Level_1 : public Level {
public:
	void step() {
		cells_->step();
		float g = 0.02 * DUDE->pos().y;
		glClearColor(g/5, g/4, g/3, 1);
	}

	vec2 gravity() const {
		float g = 3 - 0.02 * DUDE->pos().y;
		if (g < 0) g = 0;
		return vec2(0, -g);
	}

	float jump_power() const {
		return 0.55;
	}
	
	CellLevel* cells() const {
		return cells_;
	}

	Level_1() {
		CellBoard* area1 = new CellBoard(16,12);
		{
			const int nblocks = 27;
			int blocks[nblocks][2] = {
				{ 6, 0 }, { 7, 0 }, { 8, 0 }, { 9, 0 },
				{ 6, 1 },                     { 9, 1 },
				{ 6, 2 },                     { 9, 2 },
				{ 6, 3 },					  { 10, 3 },
				{ 6, 4 },                     { 10, 4 },
				{ 5, 5 },                     { 10, 5 },
				{ 3, 6 }, { 4, 6 },           { 10, 6 },
				{ 0, 8 }, { 1, 8 },           { 10, 7 }, { 10, 8 },
				{ 0, 9 },                     { 10, 9 }, { 10, 10 },
				{ 2, 11 }, { 3, 11 }, { 4, 11 }
			};
			
			for (int i = 0; i < nblocks; i++) {
				area1->set(blocks[i][0], blocks[i][1], new BoxCell);
			}
		}

		CellBoard* area2 = new CellBoard(16,12);
		{
			const int nblocks = 23;
			int blocks[nblocks][2] = {
				{ 5, 0 }, { 6, 0 }, { 7, 0 },
				{ 9, 1 }, { 10, 1 },
				{ 12, 2 },
				{ 14, 3 }, { 15, 4 },
				{ 10, 5 }, { 11, 5 },
				{ 8, 6 }, { 9, 6 }, { 12, 6 }, { 13, 6 },
				{ 6, 7 }, { 7, 7 }, { 14, 7 },
				{ 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 3 },
				{ 4, 4 }, /* { 5, 5 }, */ { 6, 6 }
			};
			
			for (int i = 0; i < nblocks; i++) {
				area2->set(blocks[i][0], blocks[i][1], new BoxCell);
			}
			area2->set(5,5, new BoxCell(1, Color(1,0.9,0.9)));
			area2->set(15,2, new InvisiBox);
		}

		CellBoard* area2invisible = new CellBoard(16,12);
		{
			const int nblocks = 6;
			int blocks[nblocks][2] = {
				{ 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 },
				{ 0, 7 }, { 1, 7 }
			};
			for (int i = 0; i < nblocks; i++) {
				area2invisible->set(blocks[i][0], blocks[i][1], new InvisiBox);
			}
		}

		CellBoard* area3 = new CellBoard(16,12);
		{
			const int nblocks = 18;
			int blocks[nblocks][2] = {
				{ 12, 0 }, { 13, 0 }, { 14, 0 }, { 15, 0 },
				{ 8, 2 }, { 9, 2 }, { 10, 2 },
				{ 8, 3 },
				{ 12, 4 }, { 13, 4 }, { 14, 4 },
				{ 14, 5 },
				{ 8, 6 }, { 9, 6 },
				{ 12, 8 }, { 13, 8 },
				{ 9, 11 }, 
			};
			
			for (int i = 0; i < nblocks; i++) {
				area3->set(blocks[i][0], blocks[i][1], new BoxCell);
			}
		}

		CellBoard* area4 = new CellBoard(16,12);
		{
			const int nblocks = 19;
			int blocks[nblocks][2] = {
				{ 11, 0 }, { 12, 0 }, { 13, 0 }, { 14, 0 }, { 15, 0 },
				{ 8, 1 }, { 9, 1 },
				{ 8, 2 },
				{ 7, 3 }, { 8, 3 },
				{ 6, 4 },
				{ 6, 5 },
				{ 6, 6 }, { 5, 6 },
				{ 4, 7 },
				{ 4, 8 },
				{ 4, 9 },  /* { 7, 9 }, { 8, 9 } */
				{ 4, 10 }, 
				{ 2, 11 }, /* { 3, 11 }, { 4, 11 }, */
			};

			for (int i = 0; i < nblocks; i++) {
				area4->set(blocks[i][0], blocks[i][1], new BoxCell);
			}

			area4->set(7, 9, new InvisiBox);
			area4->set(8, 9, new InvisiBox);
			area4->set(3, 11, new HighJumpBox(3));
			area4->set(4, 11, new HighJumpBox(3));
		}

		CellBoard* area5 = new CellBoard(16, 12);
		{
			const int nblocks = 12;
			int blocks[nblocks][2] = {
				{ 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 }, 
				{ 2, 4 }, { 2, 5 }, { 2, 6 }, { 2, 7 }, 
				{ 2, 8 }, { 2, 9 }, { 2, 10 }, { 2, 11 }
				/* , { 1, 11 }, { 0, 11 } */
			};
			
			for (int i = 0; i < nblocks; i++) {
				area5->set(blocks[i][0], blocks[i][1], new BoxCell);
			}

			area5->set(1, 10, new InvisiBox);
			area5->set(0, 11, new InvisiBox);
		}

		CellBoard* area6 = new CellBoard(16, 12);
		{
			const int num_links = 24;
			HintChain* links[num_links];
			for (int i = 0; i < num_links; i++) {
				links[i] = new HintChain;
			}
			for (int i = 0; i < num_links-1; i++) {
				links[i]->set_next(links[i+1]);
			}
			
			area6->set(15, 0, links[0]);
			area6->set(14, 1, links[1]);
			area6->set(13, 2, links[2]);
			area6->set(12, 3, links[3]);
			area6->set(11, 4, links[4]);
			area6->set(10, 5, links[5]);
			area6->set(7,  5, links[6]);
			area6->set(6,  5, links[7]);
			area6->set(5,  5, links[8]);
			area6->set(4,  5, links[9]);
			area6->set(3,  5, links[10]);
			area6->set(2,  5, links[11]);
			area6->set(1,  5, links[12]);
			area6->set(0,  5, links[13]);
			area6->set(0,  10, links[14]);
			area6->set(1,  10, links[15]);
			area6->set(10, 4, links[16]);
			area6->set(9,  3, links[17]);
			area6->set(8,  3, links[18]);
			area6->set(7,  4, links[19]);
			area6->set(0,  6, links[20]);
			area6->set(0,  7, links[21]);
			area6->set(0,  8, links[22]);
			area6->set(0,  9, links[23]);
		}
		
		CellBoard* area7 = new CellBoard(16, 12);
		{
			const int nblocks = 4;
			int blocks[nblocks][2] = {
				{ 7, 4 }, { 8, 4 },
				{ 11, 10 }, { 12, 10 }
			};
			
			for (int i = 0; i < nblocks; i++) {
				area7->set(blocks[i][0], blocks[i][1], new BoxCell);
			}
		}
		
		CellBoard* area8 = new CellBoard(16, 12);
		{
			const int nblocks = 4;
			int blocks[nblocks][2] = {
				{ 11, 3 }, { 12, 3 },
				{ 11, 9 }, { 12, 9 }
			};
			
			for (int i = 0; i < nblocks; i++) {
				area8->set(blocks[i][0], blocks[i][1], new BoxCell);
			}
		}

		CellBoard* area9 = new CellBoard(16, 12);
		{
			area9->set(7, 7, new WinBox);
		}
		
		cells_ = new CellLevel(16, 12, 4, 13);
		cells_->set_board(2,0,area1);
		cells_->set_board(2,1,area2);
		cells_->set_board(3,1,area2invisible);
		cells_->set_board(1,1,area3);
		cells_->set_board(1,2,area4);
		cells_->set_board(1,3,area5);
		cells_->set_board(0,4,area6);
		cells_->set_board(0,5,area7);
		cells_->set_board(0,6,area8);
		cells_->set_board(0,7,area8);
		cells_->set_board(0,8,area8);
		cells_->set_board(0,9,area8);
		cells_->set_board(0,10,area9);
		cells_->set_board(0,12,area1);
	}

private:
	CellLevel* cells_;
};

Level* make_level(int n) {
	switch (n) {
		case 1: return new Level_1;
		default: abort();
	}
}

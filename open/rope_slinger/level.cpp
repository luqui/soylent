#include "level.h"
#include "objects.h"
#include "player.h"
#include <fstream>

Level* LEVEL = 0;

static void literal(istream& fin, string lit) {
    string tmp;
    fin >> tmp;
    if (tmp != lit) DIE("Syntax error: expecting " + lit + "; got " + tmp);
}

struct BoxDef {
    vec ll, ur;
};    

Level::~Level() {
    delete manager;
}

void Level::load_level(string file)
{
    ifstream fin(file.c_str());

    cout << "Reading " << file << endl;
    
    literal(fin, "left");
    fin >> left;
    literal(fin, "bottom");
    fin >> bottom;
    literal(fin, "right");
    fin >> right;
    literal(fin, "top");
    fin >> top;

    vec p1pos, p2pos;
    literal(fin, "p1");
    fin >> p1pos.x >> p1pos.y;
    literal(fin, "p2");
    fin >> p2pos.x >> p2pos.y;

    num gravity;
    literal(fin, "gravity");
    fin >> gravity;

    literal(fin, "ropes");
    fin >> max_ropes;

    literal(fin, "gunspeed");
    fin >> fire_velocity;
    
    int boxes;
    literal(fin, "boxes");
    fin >> boxes;
    
    list<BoxDef> boxen;
    for (int i = 0; i < boxes; i++) {
        BoxDef bx;
        literal(fin, "box");
        fin >> bx.ll.x >> bx.ll.y >> bx.ur.x >> bx.ur.y;
        boxen.push_back(bx);
    }

    cout << "Building level\n";
    
    world = dWorldCreate();
    dWorldSetGravity(world, 0, -gravity, 0);
    collide_space = dSimpleSpaceCreate(0);
    contact_joints = dJointGroupCreate(0);
    
    manager = new ObjectManager;
    p1 = new Player(p1pos, Color(0,0,1));
    manager->add(p1);
    p2 = new Player(p2pos, Color(1,0,0));
    manager->add(p2);

    for (list<BoxDef>::iterator i = boxen.begin(); i != boxen.end(); ++i) {
        manager->add(new Wall(i->ll, i->ur));
    }

    set_view();

    player = P1_SCORE <= P2_SCORE ? p1 : p2;
    stepct_ = 5*int(1/STEP);
}

void Level::set_view()
{
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(left, right, bottom, top);
    glMatrixMode(GL_MODELVIEW);
}

void Level::freeze() 
{
    frozen_ = true;
    freeze_timer_ = 0;
    wait_timer_ = WAIT_TIMER;
    if (player == p1) next_player_ = p2; else next_player_ = p1;
    player = 0;
    MOUSE_FOCUS = 0;
}

void Level::unfreeze()
{
    if (frozen_) {
        frozen_ = false;
        freeze_timer_ = TURN_TIMER;
    }
}

void Level::step()
{
    if (--stepct_ == 0) {
        manager->sweep();
        stepct_ = 5*int(1/STEP);
    }

    if (!frozen_) {
        manager->step();
        collide();
        dWorldQuickStep(world, STEP);
        dJointGroupEmpty(contact_joints);

        if ((freeze_timer_ -= STEP) <= 0) {
            freeze();
        }
    }
    else if (!player) {
        if ((wait_timer_ -= STEP) <= 0) {
            player = next_player_;
            MOUSE_FOCUS = next_player_;
            wait_timer_ = 0;
        }
    }
}

void Level::draw()
{
    manager->draw();
    if (player && !frozen_) {
        num rpoint = freeze_timer_ / TURN_TIMER * (right - left);
        Color color = player->color();
        glColor3d(color.r, color.g, color.b);
        glBegin(GL_QUADS);
            glVertex2d(left, bottom);
            glVertex2d(left, bottom+0.5);
            glVertex2d(left + rpoint, bottom+0.5);
            glVertex2d(left + rpoint, bottom);
        glEnd();
    }

    Color p1col = p1->color();
    glColor3d(p1col.r, p1col.g, p1col.b);
    for (int i = 0; i < P1_SCORE; i++) {
        num x = left + 0.75 * i + 0.25;
        draw_box(vec(x, top - 0.75), vec(x + 0.5, top - 0.25));
    }
    
    Color p2col = p2->color();
    glColor3d(p2col.r, p2col.g, p2col.b);
    for (int i = 0; i < P2_SCORE; i++) {
        num x = right - 0.75 * i - 0.25;
        draw_box(vec(x - 0.5, top - 0.75), vec(x, top - 0.25));
    }
}

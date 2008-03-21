#include "Soldier.h"
#include "Draw.h"
#include "Relativity.h"

PikemanState::PikemanState(const tvec& ref, const vec& vel)
    : SoldierState(ref, vel), tex_(load_texture("media/red/pikeman.png"))
{ }

void PikemanState::render() const
{
    TextureBinding b = tex_->bind();
    Draw::texture_square();
}

const PikemanState* PikemanState::handle(Message* msg, const tvec& rcvdp) const
{
    tvec r = ref();
    vec v = vel();
    num time = light_point_to_line(r, v, msg->ref);
    tvec rcvd = tref(time);
    if (Messages::ChangeDirection* dir = dynamic_cast<Messages::ChangeDirection*>(msg)) {
        return new PikemanState(rcvd, dir->direction);
    }
    return 0;
}

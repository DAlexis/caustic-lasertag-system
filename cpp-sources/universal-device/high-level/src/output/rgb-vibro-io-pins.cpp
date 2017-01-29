#include "output/rgb-vibro-io-pins.hpp"

RBGVibroIOPins::RBGVibroIOPins(UintParameter id, IIOPin* r, IIOPin* g, IIOPin* b, IIOPin* v) :
	RGBVibroLocalBase(id), m_r(r), m_g(g), m_b(b), m_v(v)
{
}

void RBGVibroIOPins::update(uint8_t r, uint8_t g, uint8_t b, uint8_t v)
{
	if (r > 128)
		m_r->set();
	else
		m_r->reset();

	if (g > 128)
		m_g->set();
	else
		m_g->reset();

	if (b > 128)
		m_b->set();
	else
		m_b->reset();

	if (v > 128)
		m_v->set();
	else
		m_v->reset();
}

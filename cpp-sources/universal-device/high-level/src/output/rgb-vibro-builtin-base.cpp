#include "output/rgb-vibro-builtin-base.hpp"
#include "core/logging.hpp"

RGBVibroLocalBase::RGBVibroLocalBase(UintParameter id) :
	m_id(id)
{
}

void RGBVibroLocalBase::applyIlluminationScheme(const IllumitationScheme* scheme)
{
	m_currentScheme = scheme;
	m_currentIndex = 0;
	m_lastAnimationTime = systemClock->getTime();
}

UintParameter RGBVibroLocalBase::getId()
{
	return m_id;
}

void RGBVibroLocalBase::interrogate()
{
	updateState();
	update(m_currentState.r, m_currentState.g, m_currentState.b, m_currentState.vibro);
}

void RGBVibroLocalBase::updateState()
{
	if (m_currentScheme == nullptr)
		return;

	if (m_currentIndex == m_currentScheme->tasks.size()-1)
		return;

	const IllumitationScheme::Task &current = m_currentScheme->tasks[m_currentIndex];
	const IllumitationScheme::Task &next = m_currentScheme->tasks[m_currentIndex+1];

	Time dt = (systemClock->getTime() - m_lastAnimationTime) / 1000;
	uint32_t period = next.delayFromPrev;

	if (dt >= period)
	{
		m_currentIndex++;
		dt = 0;
		m_currentState = m_currentScheme->tasks[m_currentIndex].state;
		m_lastAnimationTime = systemClock->getTime();
		if (m_currentIndex == m_currentScheme->tasks.size()-1)
			return;
	}

	float p = float(dt) / next.delayFromPrev;

	m_currentState.r = current.state.r * (1.0-p) + next.state.r*p;
	m_currentState.g = current.state.g * (1.0-p) + next.state.g*p;
	m_currentState.b = current.state.b * (1.0-p) + next.state.b*p;
	m_currentState.vibro = current.state.vibro * (1.0-p) + next.state.vibro*p;
}

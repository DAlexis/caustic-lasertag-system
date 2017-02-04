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
	m_lastAnimationTime = 0;
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

	Time dt = (systemClock->getTime() - m_lastAnimationTime) / 1000;
	if (dt >= m_currentScheme->tasks[m_currentIndex].delayFromPrev)
	{
		m_currentIndex++;
		dt = 0;
		m_currentState = m_currentScheme->tasks[m_currentIndex].state;
		m_lastAnimationTime = systemClock->getTime();
		if (m_currentIndex == m_currentScheme->tasks.size()-1)
			return;
	}
	float p = float(dt) / m_currentScheme->tasks[m_currentIndex].delayFromPrev;
	const IllumitationScheme::State &current = m_currentScheme->tasks[m_currentIndex].state;
	const IllumitationScheme::State &next = m_currentScheme->tasks[m_currentIndex+1].state;
	m_currentState.r = current.r * (1.0-p) + next.r*p;
	m_currentState.g = current.g * (1.0-p) + next.g*p;
	m_currentState.b = current.b * (1.0-p) + next.b*p;
	m_currentState.vibro = current.vibro * (1.0-p) + next.vibro*p;
}

#include "Particle.hpp"

NoteParticle::NoteParticle(Level* level, const Vec3& pos, const Vec3& dir, float scale) :
	Particle(level, pos, Vec3::ZERO)
{
      m_vel.x *= 0.009999999776482582;
      m_vel.y *= 0.009999999776482582;
      m_vel.z *= 0.009999999776482582;
      m_vel.y += 0.2;
      m_rCol = Mth::sin((dir.x + 0.0F) * M_PI * 2.0F) * 0.65F + 0.35F;
      m_gCol = Mth::sin((dir.x + 0.33333334F) * M_PI * 2.0F) * 0.65F + 0.35F;
      m_bCol = Mth::sin((dir.x + 0.6666667F) * M_PI * 2.0F) * 0.65F + 0.35F;
      m_size *= 0.75F * scale;
      m_oSize = m_size;
      m_lifetime = 6;
      m_bNoPhysics = false;
	  m_tex = PTI_NOTE;
}

void NoteParticle::tick()
{
	m_oPos = m_pos;

	m_age++;
	if (m_age > m_lifetime)
		remove();

	move(m_vel);
	if (m_pos.y == m_oPos.y)
	{
		m_vel.x *= 1.1;
		m_vel.z *= 1.1;
	}

	m_vel *= 0.66f;

	if (m_bOnGround)
	{
		m_vel.x *= 0.7f;
		m_vel.z *= 0.7f;
	}
}

void NoteParticle::render(Tesselator& t, float f, float a, float b, float c, float d, float e)
{
	float mult = float(m_age + f) / float(m_lifetime) * 32;
	m_size = m_oSize * Mth::clamp(mult, 0.0f, 1.0f);
	Particle::render(t, f, a, b, c, d, e);
}

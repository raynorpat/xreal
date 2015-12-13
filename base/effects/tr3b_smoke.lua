
function tr3b_smoke_Smoke1(origin, dir)
	
	shader = cgame.RegisterShader("particles/steam");
	
	-- spawn particles
	for i = 1, 10, 1 do
		tr3b_smoke_SpawnSmokeParticle(origin, shader, dir)
	end
end

function tr3b_smoke_SpawnSmokeParticle(origin, shader, dir)
	
	p = particle.Spawn()
	p:SetType(particle.SMOKE)
	p:SetShader(shader)
	p:SetDuration(4900 + qmath.random() * 1000)
	
	dir2 = vector.New()
	vector.Scale(dir, 20 * qmath.random(), dir2)
	org = origin + dir2
	p:SetOrigin(org)
	
	-- poke smoke into the direction
	vel = vector.New()
	vector.Scale(dir, 20 + 10 * qmath.random(), vel)
	
	vel[0] = vel[0] + qmath.random() * 7
	vel[1] = vel[1] + qmath.random() * 7
	vel[2] = vel[2] + qmath.random() * 7
	
	p:SetVelocity(vel)

	accel = vector.Construct(0, 0, 20)
	--vector.Scale(dir, 1.5, accel)
	p:SetAcceleration(accel)
	
	p:SetColor(1, 1, 1, 0.7)

	-- size it
	dim = qmath.random() * 1
	p:SetWidth(dim)
	p:SetHeight(dim)
	
	dim = 70 + dim * 30
	p:SetEndWidth(dim)
	p:SetEndHeight(dim)
	
	p:SetRotation(8 + (qmath.crandom() * 4))
end



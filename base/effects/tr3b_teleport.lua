
--
-- TARGET_FX FUNCTIONS
--

function TestParticleSpawn(origin, dir)
	tr3b_teleport_ParticleCircle1(origin, dir)
end

function tr3b_teleport_ParticleCircle1(origin, dir)
	
	shader = cgame.RegisterShader("particles/flare2");
	
	-- spawn particles in a small circle
	vector.Set(dir, 0, 0, 1)
	
	forward = vector.Construct(20, 0, 0)
	dst = vector.New()
	vel = vector.New()
	for angle = 0, 360, 30 do
		
		vector.RotatePointAround(dst, dir, forward, angle);
		
		-- move into center
		vel[0] = -dst[0] * 0.4
		vel[1] = -dst[1] * 0.4
		vel[2] = -dst[2] * 0.4
		
		-- add circle origin to world origin
		dst = dst + origin
		
		tr3b_teleport_SpawnTeleportParticle(dst, shader, vel, dir)
	end
end

function tr3b_teleport_ParticleCircle2(origin, dir)
	
	shader = cgame.RegisterShader("particles/flare2");
	
	-- create orthogonal vector to main direction
	forward = vector.New()
	vector.Perpendicular(forward, dir);
	vector.Scale(forward, 40, forward);
	
	dst = vector.New()
	vel = vector.New()
	for angle = 0, 360, 30 do
		
		vector.RotatePointAround(dst, dir, forward, angle);
		
		-- move into center
		vel[0] = -dst[0] * 0.4
		vel[1] = -dst[1] * 0.4
		vel[2] = -dst[2] * 0.4
		
		-- add circle origin to world origin
		dst = dst + origin
		
		tr3b_teleport_SpawnTeleportParticle(dst, shader, vel, dir)
	end
end


--
-- HELPER FUNCTIONS
--

function tr3b_teleport_SpawnTeleportParticle(origin, shader, vel, dir)
	
	p = particle.Spawn()

	if not p then
		return
	end
	
	p:SetType(particle.SMOKE)
	p:SetShader(shader)
	p:SetDuration(1900 + qmath.random() * 100)
	p:SetOrigin(origin)
	
	-- give random velocity
	--randVec = vector.New()
	--randVec[0] = qmath.crandom()		-- between 1 and -1
	--randVec[1] = qmath.crandom()
	--randVec[2] = qmath.crandom()
	--vector.Normalize(randVec)
	--tmpVec = vector.New()
	--vector.Scale(randVec, 16, tmpVec)
	-- tmpVec[2] += 30     -- nudge the particles up a bit
	p:SetVelocity(vel)

	-- add some gravity/randomness
	--tmpVec = vector.New()
	--tmpVec[0] = 0 --qmath.crandom() * 3
	--tmpVec[1] = 0 --qmath.crandom() * 3
	--tmpVec[2] = 30
	--p:SetAcceleration(tmpVec)
	
	accel = vector.New()
	vector.Scale(dir, 30, accel)
	p:SetAcceleration(accel)
	
	-- set color
	p:SetColor(1.0, 1.0, 1.0, 1.0)

	-- size it
	dim = 3 + qmath.random() * 2
	p:SetWidth(dim)
	p:SetHeight(dim)
	
	dim = dim * 0.2
	p:SetEndWidth(dim)
	p:SetEndHeight(dim)
	
	-- rotate it by some degrees
	p:SetRotation(qmath.rand() % 179)
	
end

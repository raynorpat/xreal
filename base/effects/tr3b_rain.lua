
--
-- TARGET_FX FUNCTIONS
--
function tr3b_rain_RainGrid512(origin, dir)
	-- spawn particles in a 512x512 grid
	tr3b_rain_RainGrid(origin, dir, 512, 16)
end

function tr3b_rain_RainGrid256(origin, dir)
	-- spawn particles in a 256x256 grid
	tr3b_rain_RainGrid(origin, dir, 256, 16)
end

function tr3b_rain_RainGrid128(origin, dir)
	-- spawn particles in a 128x128 grid
	tr3b_rain_RainGrid(origin, dir, 128, 16)
end

function tr3b_rain_RainGrid64(origin, dir)
	-- spawn particles in a 64x64 grid
	tr3b_rain_RainGrid(origin, dir, 64, 16)
end

function tr3b_rain_RainGrid32(origin, dir)
	-- spawn particles in a 32x32 grid
	tr3b_rain_RainGrid(origin, dir, 32, 16)
end


--
-- HELPER FUNCTIONS
--
function tr3b_rain_RainGrid(origin, dir, gridSize, stepSize)
	
	shader = cgame.RegisterShader("particles/rain01");
	
	org = vector.New()
	gridSize = gridSize / 2
	for i = -gridSize, gridSize, stepSize do
		for j = -gridSize, gridSize, stepSize do
		
			org[0] = origin[0] + i
			org[1] = origin[1] + j
			org[2] = origin[2]
		
			if qmath.random() > 0.7 then
				tr3b_rain_SpawnRainParticle(org, shader)
			end
		end
	end
end

function tr3b_rain_SpawnRainParticle(origin, shader)
	
	p = particle.Spawn()

	if not p then
		return
	end
	
	p:SetType(particle.SPARK)
	p:SetShader(shader)
	p:SetDuration(10000)
	p:SetOrigin(origin)
	
	-- add some randomess to avoid parallel rain drops
	vel = vector.New()
	vel[0] = qmath.random() * 5
	vel[1] = qmath.random() * 5
	vel[2] = -40
	p:SetVelocity(vel)

	-- add some gravity/randomness
	accel = vector.New()
	accel[0] = 0 --qmath.crandom() * 3
	accel[1] = 0 --qmath.crandom() * 3
	accel[2] = -70 - qmath.random() * 50
	p:SetAcceleration(accel)
	
	-- set color
	p:SetColor(1.0, 1.0, 1.0, 1.0)

	-- size it
	p:SetWidth(5)
	p:SetHeight(0.3)
	
	p:SetEndWidth(10 + qmath.random() * 10)
	p:SetEndHeight(0.4 + qmath.random() * 0.2)
	
end

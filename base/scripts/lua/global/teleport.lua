------------------------------
-- Teleport functions--
--			       --
-- 	by otty 2007    --
------------------------------

-- Teleport everything (player, rockets and grenades)
function TeleportTouch(self, other)

		target = entity.Target(self);
		
		if target then
			other:Teleport(target);
		end	
end

-- Teleport only players
function TeleportPlayerTouch(self, other)

	if other:IsClient() then
		
		target = entity.Target(self);
		
		if target then
			other:Teleport(target);
		end	
	end
end

-- Teleport only rockets
function TeleportRocketTouch(self, other)

	target = entity.Target(self);
	
	if other:IsRocket() then
		if target then
			other:Teleport(target);
		end	
	end
end

-- Teleport only grenades
function TeleportGrenadeTouch(self, other)

	target = entity.Target(self);
	
	if other:IsGrenade() then
		if target then
			other:Teleport(target);
		end	
	end
end

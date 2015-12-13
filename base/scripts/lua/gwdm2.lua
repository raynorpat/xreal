-- TODO

function jumppadTouch(self, other)
	--game.Print("jumppadTouch called from ", self:GetClassName())
	
	if other:IsClient() then
		--game.Broadcast(other:GetClientName(), " touched jumppad ", self:GetTargetName())
		other:CenterPrint("You touched jumppad ", self:GetTargetName())
		--other:Print("You touched jumppad ", self:GetTargetName())
	end
end

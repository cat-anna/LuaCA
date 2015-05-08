
function RandomColors(blender, count)
	blender:AddColor(0, 0, 0);
	for i=1,count,1 do
		local r = xmath.RandRange(0, 256) / 256
		local g = xmath.RandRange(0, 256) / 256
		local b = xmath.RandRange(0, 256) / 256
		blender:AddColor(r, g, b);
	end
end
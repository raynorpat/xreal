
function TestVectors(inputVector)
	cgame.Print("lets do some lua vector tests ...")

	a = vector.New()
	a[0] = 15
	a.y = 13.7
	--veca.z = 97

	b = vector.Construct(10, 20, 30)
	b = inputVector;

	cgame.Print("a = ", a)
	cgame.Print("b = ", b)
	
	c = a + b
	cgame.Print(c, " = a + b")

	c = a - b
	cgame.Print(c, " = a - b")
	
	c = a * b
	cgame.Print(c, " = a dot b")

	--c = -a
	--cgame.Print(c, " = -a")
	
	return c;
end

a = vector.New()
a[0] = 15
a.y = 13.7
--veca.z = 97

b = vector.Construct(10, 20, 30)

game.Print("a = ", a)
game.Print("b = ", b)

c = a + b
game.Print(c, " = a + b")

c = a - b
game.Print(c, " = a - b")

c = -a
game.Print(c, " = -a")
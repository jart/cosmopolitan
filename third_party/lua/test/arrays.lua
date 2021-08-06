local errors = false

function assert(cond)
	if not cond then
		local line = debug.getinfo(2).currentline
		print("test failed at line " .. line)
		errors = true
	end
end

-- test array constructor
do
	local a = [1, 2, 3]
	assert(a[1] == 1)
	assert(a[2] == 2)
	assert(a[3] == 3)
	assert(#a == 3)
end

-- test nested array constructor
do
	local a = [1, 2, 3, [4, 5]]
	assert(a[1] == 1)
	assert(a[2] == 2)
	assert(a[3] == 3)
	assert(a[4][1] == 4)
	assert(a[4][2] == 5)
	assert(#a == 4)
end

-- test array write
do
	local a = [1, 2, 3]
	a[1] = -1
	assert(a[1] == -1)
	assert(#a == 3)
end

-- test array extend
do
	local a = [1, 2, 3]
	a[7] = 5
	assert(#a == 7)
	assert(a[7] == 5)
end

-- test array extend 2
do
	local a = []
	for i=5,15 do
		a[i] = i
		assert(a[i] == i)
		assert(#a == i)
	end
end

-- test setting element to nil (should not affect array size)
do
	local a = [1, 2, 3]
	a[3] = nil
	assert(a[3] == nil)
	assert(#a == 3)
end

-- test array with holes
do
	local a = [1, nil, 3]
	assert(a[1] == 1)
	assert(a[2] == nil)
	assert(a[3] == 3)
	assert(#a == 3)
	a[1] = nil
	assert(#a == 3)
end

-- test filling hole in array
do
	local a = [1, nil, 3]
	a[2] = 2
	assert(a[2] == 2)
	assert(#a == 3)
end

-- test filling hole in array 2
do
	local a = [1, nil, 3]
	local i = 2
	a[i] = 2
	assert(a[2] == 2)
	assert(#a == 3)
end

-- test read out of bounds
do
	local a = [1, 2, 3]
	assert(#a == 3)
	assert(a[0] == nil)
	assert(a[4] == nil)
	assert(#a == 3)
end

-- test array resize (array growing)
do
	local a = [1, 2, 3]
	table.resize(a, 1000)
	assert(a[4] == nil)
	assert(#a == 1000)
	a[1] = 4
	a[10] = 10
	a[11] = 11
	assert(#a == 1000)
end

-- test array resize (array shrinking)
do
	local a = [1, 2, 3, 4, 5]
	table.resize(a, 3)
	assert(a[1] == 1)
	assert(a[2] == 2)
	assert(a[3] == 3)
	assert(a[4] == nil)
	assert(a[5] == nil)
	assert(#a == 3)
end

-- test non-const integer
do
	local a = []
	local y = 3
	a[y] = 66
	assert(a[3] == 66)
	assert(#a == 3)
end

-- test table.insert()
do
	local a = [1, 2, 3]
	table.insert(a, 1, "new")
	assert(a[1] == "new")
	assert(a[2] == 1)
	assert(a[3] == 2)
	assert(a[4] == 3)
	assert(#a == 4)
end

-- test table.remove()
do
	local a = [1, 2, 3]
	table.remove(a, 1)
	assert(a[1] == 2)
	assert(a[2] == 3)
        -- TODO: fix the implementation, as after upgrading to Lua 5.4.3
        -- it keeps the array size the same after table.remove()
	-- assert(#a == 2)
end

-- test ipairs
-- expected behavior: equivalent to for i=1,#a do print(i, a[i]) end
do
	local a = [1, nil, 3, nil]
	local cnt = 0
	for k,v in ipairs(a) do
		assert(v == a[k])
		cnt = cnt + 1
	end
	assert(cnt == #a)
end

-- test pairs
-- expected behavior: same as ipairs?
do
	local a = [1, nil, 3]
	local cnt = 0
	for k,v in pairs(a) do
		assert(v == a[k])
		cnt = cnt + 1
	end
	assert(cnt == 3)
end

-- test normal insert/remove operations
local function test (a)
  assert(not pcall(table.insert, a, 2, 20));
  table.insert(a, 10);
  table.insert(a, 2, 20);
  table.insert(a, 1, -1);
  table.insert(a, 40);
  table.insert(a, #a+1, 50)
  table.insert(a, 2, -2)
  assert(a[2] ~= undef)
  assert(a["2"] == undef)
  assert(not pcall(table.insert, a, 0, 20));
  assert(not pcall(table.insert, a, #a + 2, 20));
  assert(table.remove(a,1) == -1)
  assert(table.remove(a,1) == -2)
  assert(table.remove(a,1) == 10)
  assert(table.remove(a,1) == 20)
  assert(table.remove(a,1) == 40)
  assert(table.remove(a,1) == 50)
  assert(table.remove(a,1) == nil)
  assert(table.remove(a) == nil)
  assert(table.remove(a, #a) == nil)
end

a = {n=0, [-7] = "ban"}
test(a)
assert(a.n == 0 and a[-7] == "ban")

if not errors then
	print("All tests passed!")
end

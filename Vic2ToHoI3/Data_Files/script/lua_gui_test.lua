
function run_me_from_lua()
	local f = io.open("lua_output.txt", "a")
	f:write("I like cake!\n")
	f:close()
end

--run_me_from_lua()
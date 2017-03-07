io.write( "[lua] These args were passed into the script from C\n" );

for i=0,table.getn(arg) do
   print(i,arg[i])
end

io.write("nothing is: " .. arg["nothing"] .. "\n")

io.write("[lua] Script returning data back to C\n")

local temp = {}
temp[1]=9
temp[2]=8
temp[3]=7
temp[4]=6
temp[5]=5

temp[6] = {10, 11}

temp["something"] = "nothing";

return temp,9,1

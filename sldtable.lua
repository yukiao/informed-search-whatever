
local function trim(str)
	return str:match("%s*(.+)"):reverse():match("%s*(.+)"):reverse()
end

local function split(text, d)
	local last = 1
	local r = {}

	while last <= #text do
		local s = text:find(d, last, true) or (#text + 1)
		r[#r + 1] = text:sub(last, s - 1)
		last = s + #d
	end

	return r
end

local function parseGraph(data)
	assert(data:sub(1, 19) == "Teyvat Graph Format")

	local vertex = {}
	local adjacents = {}
	local result = {vertex = vertex, adjacents = adjacents}
	local mode = 0 -- 1 = info, 2 = vertex, 3 = edges
	local iterator = data:gsub("\r\n", "\n"):gmatch("[^\n]+")
	iterator() -- skip header

	for line in iterator do
		if #line > 0 and line:sub(1, 1) ~= "#" then
			if line == "Info:" then
				assert(mode == 0, "unexpected Info data")
				mode = 1
			else
				assert(mode ~= 0, "Info data must appear first!")

				if line == "Vertex:" then
					mode = 2
				elseif line == "Edges:" then
					mode = 3
				elseif mode == 1 then
					-- Info data
					local key, value = line:match("^(%w+):%s*(.+)$")

					if key == "Resolution" and result.resolution == nil then
						local width, height = value:match("^(%d+)x(%d+)$")
						width, height = tonumber(width), tonumber(height)
						assert(width and height and width > 0 and height > 0, "invalid resolution")
						result.resolution = {width, height}
					elseif key == "Parameter" and result.nparam == nil then
						local nparam = tonumber(value:match("^(%d+)$"))
						assert(nparam and nparam >= 0, "invalid parameters")
						result.nparam = nparam
					else
						error("Info: unexpected "..key)
					end
				elseif mode == 2 then
					-- Vertex data
					local x, y, name = line:match("^(%d+)x(%d+):%s*(.+)$")
					x, y = tonumber(x), tonumber(y)

					if x and y and name then
						name = trim(name)

						if not vertex[name] then
							local v = {name, x, y}
							vertex[#vertex + 1] = v
							vertex[name] = v
						else
							error("duplicate vertex definition "..name)
						end
					else
						error("unknown vertex data: "..line)
					end
				elseif mode == 3 then
					-- Edges data
					local splitted = split(line, " - ")

					if #splitted >= 2 then
						local param = {}
						local data1, data2 = {0, param}, {0, param}

						for _, num in ipairs(split(splitted[3] or "", " ")) do
							if #num > 0 then
								num = tonumber(num)

								if not num then
									error("invalid parameter value "..num.." at: "..line)
								end

								param[#param + 1] = num
							end
						end

						if #param < result.nparam then
							error("insufficient parameter: "..line)
						end

						local a, b = trim(splitted[1]), trim(splitted[2])

						local va = vertex[a]
						if not va then
							error("unknown vertex: "..a)
						end

						local vb = vertex[b]
						if not vb then
							error("unknown vertex: "..b)
						end

						data1[1] = vb
						data2[1] = va

						local adjva = adjacents[va] or {}
						adjacents[va] = adjva
						local adjvb = adjacents[vb] or {}
						adjacents[vb] = adjvb
						adjva[#adjva + 1] = data1
						adjvb[#adjvb + 1] = data2
					else
						error("unknown edges data: "..line)
					end
				end
			end
		end
	end

	return result
end

local teyvatFile = assert(io.open("teyvat.txt", "rb"))
local teyvat = parseGraph(teyvatFile:read("*a"))
teyvatFile:close()

local function printSLDTable(dest)
	local destVert = teyvat.vertex[dest]
	local dx, dy = destVert[2], destVert[3]
	io.write("Wilayah,h_SLD(n)\n")
	for _, v in ipairs(teyvat.vertex) do
		io.write(v[1]:gsub("\\n", " "), ",", math.floor(math.sqrt((v[2] - dx)^2 + (v[3] - dy)^2)), "\n")
	end
	io.write("\n")
end

local function printCustomHeuristicTable(dest)
	local destVert = teyvat.vertex[dest]
	local dx, dy = destVert[2], destVert[3]
	local bsafety, bdist = 0, 0

	for _, adj in ipairs(teyvat.adjacents[destVert]) do
		bdist = bdist + math.sqrt((adj[1][2] - dx) ^ 2 + (adj[1][3] - dy) ^ 2)
		bsafety = bsafety + adj[2][2]
	end

	bdist = bdist / #teyvat.adjacents[destVert]
	bsafety = bsafety / #teyvat.adjacents[destVert]

	io.write("Wilayah,h_Danger(n)\n")
	for _, v in ipairs(teyvat.vertex) do
		local dist = math.sqrt((v[2] - dx)^2 + (v[3] - dy)^2)
		local asafety, adist = 0, 0

		for _, adj in ipairs(teyvat.adjacents[v]) do
			adist = adist + math.sqrt((adj[1][2] - v[2]) ^ 2 + (adj[1][3] - v[3]) ^ 2)
			asafety = asafety + adj[2][2]
		end

		adist = adist / #teyvat.adjacents[v]
		asafety = asafety / #teyvat.adjacents[v]
		io.write(v[1]:gsub("\\n", " "), ",", math.floor((asafety + bsafety) * dist / (adist + bdist)), "\n")
	end
	io.write("\n")
end

printSLDTable("Dawn Winery")
printSLDTable("Stormterror's Lair")
printCustomHeuristicTable("Dawn Winery")
printCustomHeuristicTable("Stormterror's Lair")

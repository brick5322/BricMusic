ThemeColor = {
    Red = 0xE7,
    Green = 0x99,
    Blue = 0xB0
}

AudioPaths = {}

DefaultMenuPath = "&path&"

Extension = {
    "mp3",
    "flac",
    "wav"
}

require("lfs")

local re = {}
for i, v in pairs(Extension) do
    re[i] = ".*%." .. v
end
for file in lfs.dir(DefaultMenuPath) do
    if file ~= "." and file ~= ".." then
        for i, v in pairs(re) do
            if string.find(file, v, 0) then
                local f = DefaultMenuPath .. "/" .. file
                table.insert(AudioPaths, f)
            end
        end
    end
end

-- local file = io.open("./tmp.log","w")
-- io.output(file)
-- for key, value in pairs(AudioPaths) do
--     io.write(value)
--     io.write("\n")
-- end
-- io.close(file)
-- io.output(io.stdout)

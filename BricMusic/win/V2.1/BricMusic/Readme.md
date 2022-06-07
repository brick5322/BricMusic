# BricMusic——一款轻便的音乐播放器

就这么个软件，想用的话，要么把它设置成mp3什么的默认打开工具，或者进`settings.lua`，设置一下默认打开的音乐列表。

解释下`settings.lua`吧

```lua
ThemeColor = {
    Red = 0x9A,
    Green = 0xC8,
    Blue = 0xE2
}

AudioPaths = {}

DefaultMenuPath = "G:/Music/库"

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

```

`ThemeColor`这个决定了音乐播放器的主色调（值得一提的是9AC8E2，是向晚AVA的应援色，给ASoul的五个姑娘点个关注吧）

`AudioPaths`这个是默认播放列表，示例里面是空的，所以才有了下面的代码，下面的代码是往`AudioPaths`里面填音乐文件路径的，它会从`DefaultMenuPath`找`Extension`所包含格式的音频加到`AudioPaths`里面。

当然，你也大可以写个像这样的`AudioPaths`，然后把后面的代码都删掉。

```lua
AudioPaths = {
	"G:/music/库/111.mp3",
	"G:/music/库/222.mp3",
    "G:/music/库/333.mp3",
}
```

最后的注释，那些是调试用的，会把播放列表存到`tmp.log`里面，要是觉得不对劲，可以解开这个看看这里输出的对不对。

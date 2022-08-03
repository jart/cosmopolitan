local top = [[
    <!DOCTYPE html>
    <html lang=en>
    
    <head>
        <link rel="icon" href="data:;base64,iVBORw0KGgo=">
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
        <meta name="description" content="description">
        <style>
        * {margin: 0;padding: 0;box-sizing: border-box;}
        body {margin: 0 auto;font-family: Roboto, Helvetica, Arial, sans-serif;color: #444444;line-height: 1;max-width: 960px;padding: 30px;}
        h1, h2, h3, h4 {color: #111111;font-weight: 400;}
        h1, h2, h3, h4, h5, p {margin-bottom: 24px;padding: 0;}
        h1 {font-size: 48px;}
        h2 {font-size: 36px;margin: 24px 0 6px;}
        .button {min-height:1rem;border: none;border-radius: 0.25rem;background: #1E88E5;color: white;font-family: sans-serif;font-size: 1rem;line-height: 1.2;white-space: nowrap;text-decoration: none;padding: 0.25rem 0.5rem;margin: 0.25rem;cursor: pointer;}
        body {height: 100vh;display: grid;place-items: center;background: hsl(0, 0%, 93%);}
        .canvas {width: 75vmin;height: 75vmin;background: hsl(350, 70%, 80%);}
        .shadow {filter: drop-shadow(1px 1px 0.5px rgba(0, 0, 0, .25));}
        </style>
    </head>
    
    <body>
]]

local bottom = [[
    <a href="https://github.com/jart/cosmopolitan/tree/master/tool/net/demo/gensvg/2/index.lua">Show The Code</a>
    </body>
</html>
]]

local nrows = 10
local ncols = 10
local sqsize = 20
local seed = math.random()
local function render()
    local xsize = nrows * sqsize
    local ysize = ncols * sqsize
    local style = string.format('style="background-color: hsl(%s,90%%,90%%);"',
                                seed * 360)
    Write('<svg class="canvas" ' .. style .. ' viewBox="0 0 ' .. xsize .. ' ' ..
              ysize .. '">')
    Write('</svg>')
end

Write(top)
Write("<h2>Add Random Color</h2>")
Write("<h4>Refresh for a new random seed</h4>")
render()

Write("seed: " .. seed)
Write('<code>')
Write(EscapeHtml(
          [[local style = string.format('style="background-color: hsl(%s,90%%,90%%);"', seed * 360)]]))
Write('</br>')
Write(EscapeHtml(
          [[Write('<svg class="canvas" ' .. style .. ' viewBox="0 0 ' .. xsize .. ' ' .. ysize .. '">')]]))
Write('</code>')
Write('<div>')
Write('<a class="button" href="/gensvg/1">Back</a>')
Write('<a class="button" href="/gensvg/3">Next</a>')
Write('</div>')
Write(bottom)

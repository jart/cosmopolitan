-- redbean maxmind demo
-- by justine tunney

local maxmind = require "maxmind"

local kMetroCodes = {
   [500] = "Portland-Auburn ME",
   [501] = "New York NY",
   [502] = "Binghamton NY",
   [503] = "Macon GA",
   [504] = "Philadelphia PA",
   [505] = "Detroit MI",
   [506] = "Boston MA-Manchester NH",
   [507] = "Savannah GA",
   [508] = "Pittsburgh PA",
   [509] = "Ft. Wayne IN",
   [510] = "Cleveland-Akron (Canton) OH",
   [511] = "Washington DC (Hagerstown MD)",
   [512] = "Baltimore MD",
   [513] = "Flint-Saginaw-Bay City MI",
   [514] = "Buffalo NY",
   [515] = "Cincinnati OH",
   [516] = "Erie PA",
   [517] = "Charlotte NC",
   [518] = "Greensboro-High Point-Winston Salem NC",
   [519] = "Charleston SC",
   [520] = "Augusta GA",
   [521] = "Providence RI-New Bedford MA",
   [522] = "Columbus GA",
   [523] = "Burlington VT-Plattsburgh NY",
   [524] = "Atlanta GA",
   [525] = "Albany GA",
   [526] = "Utica NY",
   [527] = "Indianapolis IN",
   [528] = "Miami-Ft. Lauderdale FL",
   [529] = "Louisville KY",
   [530] = "Tallahassee FL-Thomasville GA",
   [531] = "Tri-Cities TN-VA",
   [532] = "Albany-Schenectady-Troy NY",
   [533] = "Hartford & New Haven CT",
   [534] = "Orlando-Daytona Beach-Melbourne FL",
   [535] = "Columbus OH",
   [536] = "Youngstown OH",
   [537] = "Bangor ME",
   [538] = "Rochester NY",
   [539] = "Tampa-St. Petersburg (Sarasota) FL",
   [540] = "Traverse City-Cadillac MI",
   [541] = "Lexington KY",
   [542] = "Dayton OH",
   [543] = "Springfield-Holyoke MA",
   [544] = "Norfolk-Portsmouth-Newport News VA",
   [545] = "Greenville-New Bern-Washington NC",
   [546] = "Columbia SC",
   [547] = "Toledo OH",
   [548] = "West Palm Beach-Ft. Pierce FL",
   [549] = "Watertown NY",
   [550] = "Wilmington NC",
   [551] = "Lansing MI",
   [552] = "Presque Isle ME",
   [553] = "Marquette MI",
   [554] = "Wheeling WV-Steubenville OH",
   [555] = "Syracuse NY",
   [556] = "Richmond-Petersburg VA",
   [557] = "Knoxville TN",
   [558] = "Lima OH",
   [559] = "Bluefield-Beckley-Oak Hill WV",
   [560] = "Raleigh-Durham (Fayetteville) NC",
   [561] = "Jacksonville FL",
   [563] = "Grand Rapids-Kalamazoo-Battle Creek MI",
   [564] = "Charleston-Huntington WV",
   [565] = "Elmira NY",
   [566] = "Harrisburg-Lancaster-Lebanon-York PA",
   [567] = "Greenville-Spartanburg SC-Asheville NC-Anderson SC",
   [569] = "Harrisonburg VA",
   [570] = "Florence-Myrtle Beach SC",
   [571] = "Ft. Myers-Naples FL",
   [573] = "Roanoke-Lynchburg VA",
   [574] = "Johnstown-Altoona PA",
   [575] = "Chattanooga TN",
   [576] = "Salisbury MD",
   [577] = "Wilkes Barre-Scranton PA",
   [581] = "Terre Haute IN",
   [582] = "Lafayette IN",
   [583] = "Alpena MI",
   [584] = "Charlottesville VA",
   [588] = "South Bend-Elkhart IN",
   [592] = "Gainesville FL",
   [596] = "Zanesville OH",
   [597] = "Parkersburg WV",
   [598] = "Clarksburg-Weston WV",
   [600] = "Corpus Christi TX",
   [602] = "Chicago IL",
   [603] = "Joplin MO-Pittsburg KS",
   [604] = "Columbia-Jefferson City MO",
   [605] = "Topeka KS",
   [606] = "Dothan AL",
   [609] = "St. Louis MO",
   [610] = "Rockford IL",
   [611] = "Rochester MN-Mason City IA-Austin MN",
   [612] = "Shreveport LA",
   [613] = "Minneapolis-St. Paul MN",
   [616] = "Kansas City MO",
   [617] = "Milwaukee WI",
   [618] = "Houston TX",
   [619] = "Springfield MO",
   [622] = "New Orleans LA",
   [623] = "Dallas-Ft. Worth TX",
   [624] = "Sioux City IA",
   [625] = "Waco-Temple-Bryan TX",
   [626] = "Victoria TX",
   [627] = "Wichita Falls TX & Lawton OK",
   [628] = "Monroe LA-El Dorado AR",
   [630] = "Birmingham AL",
   [631] = "Ottumwa IA-Kirksville MO",
   [632] = "Paducah KY-Cape Girardeau MO-Harrisburg-Mount Vernon IL",
   [633] = "Odessa-Midland TX",
   [634] = "Amarillo TX",
   [635] = "Austin TX",
   [636] = "Harlingen-Weslaco-Brownsville-McAllen TX",
   [637] = "Cedar Rapids-Waterloo-Iowa City & Dubuque IA",
   [638] = "St. Joseph MO",
   [639] = "Jackson TN",
   [640] = "Memphis TN",
   [641] = "San Antonio TX",
   [642] = "Lafayette LA",
   [643] = "Lake Charles LA",
   [644] = "Alexandria LA",
   [647] = "Greenwood-Greenville MS",
   [648] = "Champaign & Springfield-Decatur,IL",
   [649] = "Evansville IN",
   [650] = "Oklahoma City OK",
   [651] = "Lubbock TX",
   [652] = "Omaha NE",
   [656] = "Panama City FL",
   [657] = "Sherman TX-Ada OK",
   [658] = "Green Bay-Appleton WI",
   [659] = "Nashville TN",
   [661] = "San Angelo TX",
   [662] = "Abilene-Sweetwater TX",
   [669] = "Madison WI",
   [670] = "Ft. Smith-Fayetteville-Springdale-Rogers AR",
   [671] = "Tulsa OK",
   [673] = "Columbus-Tupelo-West Point MS",
   [675] = "Peoria-Bloomington IL",
   [676] = "Duluth MN-Superior WI",
   [678] = "Wichita-Hutchinson KS",
   [679] = "Des Moines-Ames IA",
   [682] = "Davenport IA-Rock Island-Moline IL",
   [686] = "Mobile AL-Pensacola (Ft. Walton Beach) FL",
   [687] = "Minot-Bismarck-Dickinson(Williston) ND",
   [691] = "Huntsville-Decatur (Florence) AL",
   [692] = "Beaumont-Port Arthur TX",
   [693] = "Little Rock-Pine Bluff AR",
   [698] = "Montgomery (Selma) AL",
   [702] = "La Crosse-Eau Claire WI",
   [705] = "Wausau-Rhinelander WI",
   [709] = "Tyler-Longview(Lufkin & Nacogdoches) TX",
   [710] = "Hattiesburg-Laurel MS",
   [711] = "Meridian MS",
   [716] = "Baton Rouge LA",
   [717] = "Quincy IL-Hannibal MO-Keokuk IA",
   [718] = "Jackson MS",
   [722] = "Lincoln & Hastings-Kearney NE",
   [724] = "Fargo-Valley City ND",
   [725] = "Sioux Falls(Mitchell) SD",
   [734] = "Jonesboro AR",
   [736] = "Bowling Green KY",
   [737] = "Mankato MN",
   [740] = "North Platte NE",
   [743] = "Anchorage AK",
   [744] = "Honolulu HI",
   [745] = "Fairbanks AK",
   [746] = "Biloxi-Gulfport MS",
   [747] = "Juneau AK",
   [749] = "Laredo TX",
   [751] = "Denver CO",
   [752] = "Colorado Springs-Pueblo CO",
   [753] = "Phoenix AZ",
   [754] = "Butte-Bozeman MT",
   [755] = "Great Falls MT",
   [756] = "Billings MT",
   [757] = "Boise ID",
   [758] = "Idaho Falls-Pocatello ID",
   [759] = "Cheyenne WY-Scottsbluff NE",
   [760] = "Twin Falls ID",
   [762] = "Missoula MT",
   [764] = "Rapid City SD",
   [765] = "El Paso TX",
   [766] = "Helena MT",
   [767] = "Casper-Riverton WY",
   [770] = "Salt Lake City UT",
   [771] = "Yuma AZ-El Centro CA",
   [773] = "Grand Junction-Montrose CO",
   [789] = "Tucson (Sierra Vista) AZ",
   [790] = "Albuquerque-Santa Fe NM",
   [798] = "Glendive MT",
   [800] = "Bakersfield CA",
   [801] = "Eugene OR",
   [802] = "Eureka CA",
   [803] = "Los Angeles CA",
   [804] = "Palm Springs CA",
   [807] = "San Francisco-Oakland-San Jose CA",
   [810] = "Yakima-Pasco-Richland-Kennewick WA",
   [811] = "Reno NV",
   [813] = "Medford-Klamath Falls OR",
   [819] = "Seattle-Tacoma WA",
   [820] = "Portland OR",
   [821] = "Bend OR",
   [825] = "San Diego CA",
   [828] = "Monterey-Salinas CA",
   [839] = "Las Vegas NV",
   [855] = "Santa Barbara-Santa Maria-San Luis Obispo CA",
   [862] = "Sacramento-Stockton-Modesto CA",
   [866] = "Fresno-Visalia CA",
   [868] = "Chico-Redding CA",
   [881] = "Spokane WA",
}

local function Dump(p)
   if type(p) == 'table' then
      local a = {}
      for k in pairs(p) do
         table.insert(a, k)
      end
      table.sort(a)
      Write('<dl>\n')
      for i = 1,#a do
         local k = a[i]
         local v = p[k]
         Write('<dt>')
         Dump(k)
         Write('\n')
         Write('<dd>')
         if k == 'iso_code' then
            Write('<img style="max-width:32px" src="//justine.lol/flags/' .. v:lower() .. '.png"> ')
         end
         Dump(v)
         if k == 'metro_code' then
            local metro = kMetroCodes[v]
            if metro then
               Write(' (' .. metro .. ')')
            end
         elseif k == 'accuracy_radius' then
            Write(' km')
         end
         Write('\n')
      end
      Write('</dl>\n')
   else
      Write(EscapeHtml(tostring(p)))
   end
end

local function main()
   if GetMethod() ~= 'GET' and GetMethod() ~= 'HEAD' then
      ServeError(405)
      SetHeader('Allow', 'GET, HEAD')
      return
   end

   local ip = nil
   local geo = nil
   local asn = nil
   local value = '8.8.8.8'
   if HasParam('ip') then
      local geodb = maxmind.open('/usr/local/share/maxmind/GeoLite2-City.mmdb')
      local asndb = maxmind.open('/usr/local/share/maxmind/GeoLite2-ASN.mmdb')
      ip = ParseIp(GetParam('ip'))
      if ip ~= -1 then
         value = FormatIp(ip)
         geo = geodb:lookup(ip)
         if geo then
            geo = geo:get()
         end
         asn = asndb:lookup(ip)
         if asn then
            asn = asn:get()
         end
         if not geo and not asn then
            SetStatus(404)
         end
      end
   end

   Write([[<!doctype html>
     <title>redbean maxmind demo</title>
     <style>
       body { padding: 1em; }
       h1 a { color: inherit; text-decoration: none; }
       h1 img { border: none; vertical-align: middle; }
       input { margin: 1em; padding: .5em; }
       pre { margin-left: 2em; }
       p { word-break: break-word; max-width: 650px; }
       dt { font-weight: bold; }
       dd { margin-top: 1em; margin-bottom: 1em; }
       .hdr { text-indent: -1em; padding-left: 1em; }
     </style>
     <h1>
       <a href="/"><img src="/redbean.png"></a>
       <a href="maxmind.lua">redbean maxmind demo</a>
     </h1>
     <p>
       Your redbean supports MaxMind GeoLite2 which is a free database
       you have to download separately, at their <a
       href="https://www.maxmind.com/en/geolite2/signup">website
       here</a>. It's worth doing because it lets you turn IP addresses
       into geographical coordinates, addresses, name it. Being able to
       Lua script this database is going to help you address things like
       online fraud and abuse.
     </p>
     <p>
       This script is hard coded to assume the database is at the
       following paths:
       <ul>
       <li><code>/usr/local/share/maxmind/GeoLite2-City.mmdb</code>
       <li><code>/usr/local/share/maxmind/GeoLite2-ASN.mmdb</code>
       </ul>
     <p>
       Which on Windows basically means the same thing as:
     </p>
       <ul>
       <li><code>C:\usr\local\share\maxmind\GeoLite2-City.mmdb</code>
       <li><code>C:\usr\local\share\maxmind\GeoLite2-ASN.mmdb</code>
       </ul>
     <p>
       Once you've placed it there, you can fill out the form below to
       have fun crawling all the information it provides!
     </p>
     <form action="maxmind.lua" method="get">
     <input type="text" id="ip" name="ip" placeholder="8.8.8.8"
            value="%s" onfocus="this.select()" autofocus>
     <label for="ip">ip address</label>
     <br>
     <input type="submit" value="Lookup" autofocus>
     </form>
   ]] % {EscapeHtml(value)})

   if ip then
      Write('<h3>Maxmind Geolite DB</h3>')
      if geo then
         Dump(geo)
      else
         Write('<p>Not found</p>\n')
      end

      Write('<h3>Maxmind ASN DB</h3>')
      if asn then
         Dump(asn)
      else
         Write('<p>Not found</p>\n')
      end
   end

end

main()

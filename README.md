# Cosmopolitan

Fast portable static native textmode executable containers.

For an introduction to this project, please read the <a
href="https://justine.storage.googleapis.com/ape.html">αcτµαlly pδrταblε
εxεcµταblε</a> blog post.

Cosmopolitan builds run natively on most platforms without dependencies,
because it implements the subset of the C library, compiler runtimes and
system call interfaces that've achieved near universal consensus amongst
all major platforms, e.g. Linux/BSD/XNU/NT. Containerization is achieved
by having binaries also be ZIP files. Modern static stock GNU toolchains
are provided in a hermetic mono repo for the best historical determinism

Here's how you can get started by printing cat videos inside a terminal:

    make -j8 o//tool/viz/printvideo.com
    wget https://justine.storage.googleapis.com/cats.mpg
    o//tool/viz/printvideo.com cats.mpg
    unzip -vl o//tool/viz/printvideo.com

Cosmopolitan provides a native development environment similar to those
currently being offered by RedHat, Google, Apple, Microsoft, etc. We're
different in two ways: (1) we're not a platform therefore we don't have
any commercial interest in making our tooling work better on one rather
than another; and (2) we cater primarily towards features having gained
cross-platform agreement. Goal is software that stands the test of time
without the costs and restrictions cross-platform distribution entails.

That makes Cosmopolitan an excellent fit for writing small CLI programs
that do things like matrix multiplication relu stdio as a subprocess or
perhaps a web server having the minimum surface area that you require.

## Getting Started

Just clone the repository and put your own folder in it. Please choose a
name that's based on a .com or .org domain name registration you control
which is scout's honor but could change to verify TXT records in future.

We provide a script that makes it easy to start a new package:

    examples/package/new.sh com/github/user/project
    emacs com/github/user/project/program.c
    make o//com/github/user/project/program.com
    o//com/github/user/project/program.com

Please note GNU Make is awesome. Little known fact: make is a functional
programming language. We improved upon it too! In [tool/build/package.c]
which performs strict dependency checking, to correct Google's published
mistakes c. 2006 which was when they switched from using a GNU Make repo
in favor of an inhouse derivative called Blaze which does graph checking
thereby allowing the repository to grow gracefully with any requirements

## Performance

Your C Standard Library is designed to be competitive with glibc, which
has historically been the best. Routines like [libc/nexgen32e/memcpy.S]
are usually accompanied by microbenchmarks to demonstrate their merits.

Where GNU, LLVM and MSVC got lazy is intrinsics. Cosmopolitan does that
better. Your [ansitrinsics library] makes a 10x speedup so much easier,
using Intel's new instructions, in such a way that only allows Intel to
leverage their patents which have knowable expiry; rather than compiler
intrinsics API copyrights, which might never expire like Walt Disney IP

See [dsp/scale/cdecimate2xuint8x8.c] and [tool/viz/lib/ycbcr2rgb3.c] as
an example, of how 4k video convolutions needed to print cat videos can
be done from a single core without threads on a cheap computer, without
sacrificing backwards compatibility due to excellent microarchitectural
dispatch like [libc/nexgen32e/kcpuids.S], [libc/nexgen32e/x86feature.h]

Furthermore Cosmopolitan provides you with Intel's official instruction
length decoder Xed ravaged down to 3kb in size using Tim Patterson code
stunts. So you can absolutely code high-performance lightweight fabless
x86 microprocessors using any hobbyist board without royalties, because
Xed tells us which parts of the encoding space now belong to the people.
Please see [third_party/xed/x86ild.greg.c] to learn more.

## Integrated Development Environment

Your Cosmopolitan IDE is based on whichever editor you love most. Emacs
configs are provided, showing how `make tags` can be used to automate
certain toil, such as adding include lines by typing `C-c C-h` over a
symbol. We recommend trying the following:

    sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
    sudo apt install gdb ragel ctags clang-format-10
    git clone git@github.com:jart/cosmopolitan.git && cd cosmopolitan
    tool/scripts/install-emacs.sh    # Emacs 26.1 has a bug
    tool/scripts/configure-emacs.sh  # adds load statements
    make tags                        # index all the symbol
    emacs                            # for power and glory!

See [tool/emacs/cosmo-stuff.el] for further details. Further note that
this codebase might seem to have unusual numbers of include statements
quoted and vendored code, but that's actually why the tools work since
it gives them easy perfect knowledge of what exists, thus performance.

## Specimen

Cosmopolitan encodes binaries respecting the intersection of reasonable
platform requirements. RADIX-256 disassembly of [examples/life.c] below
should explain how easy it is to do pe+sh+elf+macho+bootloader overlays
enabling gnu ld.bfd to produce a simple portable 12kb .com program file

Please send feedback and concerns to <jtunney@gmail.com> since we would
love to do an even better job. Please also let us know if some goofball
is distributing without bundling their source code, so we can reach out
and ask if they would like to purchase a commercial license.

    make -j10 -O MODE=tiny CPPFLAGS=-DIM_FEELING_NAUGHTY
    o/tiny/tool/viz/bing.com <o/tiny/examples/life.com | 
    o/tiny/tool/viz/fold.com

    MZqFpD=‘◙  ► °       ☺ ◘@           JT                      p♂  
    ▓@δ δ◄ÉÉδ♥R1╥╜  δ♣Θ⌐☼  ⁿ┐ p1╔·Ä╫ë╠√♫XΦ  ^üεh ▒♦╤εâΘ☺u∙╣ ☻┐ ♦Ä▐Ä╟
    1÷1λ≤ñΩï@  ╣ ■≤ñ1φÄ▌Φ  Uëσ╣ 0╕ ☺Ä└1└1λ≤¬Ç·@t#Φ. ╕ ♦Ä└┐↑ 0÷1╔ë°Φ]
     î╞â╞ Ot•Hu≈Ä╞δ∞ë♫@2ë▬B2Ω3E  ◙SR┤◘═‼r,ê╧Çτ⁇Çß└╨┴╨┴å═▲♠▼1÷Ä╞╛ 2ç≈
    ÑÑÑÑÑñ▼ô½æ½Æ½X¬Æ[├ZÇ≥Ç1└═‼r≈δ┴S1█è▲♀29├w☻ë╪QP╗ ►î└)├üπλ☼╣♣ ╤δâΘ☺
    u∙XYà█t♠9├w☻ë╪PQå═╨╔╨╔Ç╔☺1█┤☻═‼Y[r‼┤ 9├|♪A;♫♪2~♦1╔■╞[├P1└═‼Xδ╨  
                                                                    
                                                                  U¬
    ‘◙#‘“◙o=“$(command -v “$0“)“◙set -- “$o“ “$@“◙if [ -d /Applicati
    ons ]; then◙dd if=“$o“ of=“$o“ bs=8 skip=“     410“ count=“     
     87“ conv=notrunc 2>/dev/null◙elif exec 7<> “$o“; then◙printf ‘╲
    177ELF╲2╲1╲1╲011╲0╲0╲0╲0╲0╲0╲0╲0╲2╲0╲076╲0╲1╲0╲0╲0╲134╲022╲100╲0
    00╲000╲000╲000╲000╲150╲012╲000╲000╲000╲000╲000╲000╲000╲000╲000╲0
    00╲000╲000╲000╲000╲0╲0╲0╲0╲100╲0╲070╲0╲004╲000╲0╲0╲000╲000╲000╲0
    00‘ >&7◙exec 7<&-◙fi◙exec “$@“◙R=$⁇◙if [ $R -eq 126 ] && [ “$(un
    ame -m)“ != x86_64 ]; then◙if Q=“$(command -v qemu-x86_64)“; the
    n◙exec “$Q“ “$@“◙else◙echo error: need qemu-x86_64 >&2◙fi◙fi◙exi
    t $R◙αcτμαlly pδrταblε εxεcμταblε♪◙ error:  ♪◙ cpuid oldskool ds
    knfo e820 nomem nolong hello◙ ♀  Cf☼▼D  8 ╕D      f☼▼D          
    λλ   Ü☼ λλ   Æ☼ λλ   Ü╧ λλ   Æ╧ λλ   ¢» λλ   ô» ☻░¡←  ☺ ■OQΣ≡♦  
      ►  0►  @► ►♣  =☻░¡+☼à↑♪  j ¥▓@☼ └âα■☼“└fΩW@  ÉÉÉÉUëσΦ§♪Φ↓ ┐• î
    ♠▬2ú↑2Φ(☺┐EDΦÑ Φ╨♀Φ→☻Uëσ╣♦ ╛ 0¼ê┬¼ê╞à╥t♀QVë╫╛€DΦ○ ^YâΘ☺uσ]├Uëσë·
    à╥t↑RV1╔▒♥☺╩¼^♀ÇεZ¼εâ┬☺âΘ☺y÷]├UëσΦ↕ Φ  Uëσ┐ 0╛♦ ΦÇ ΦÉ☺Uëσï╖☻0à÷u
    ♦ï6 01└PV╕lDPVWV╕dDP_àλt♠^Φ♦ δ⌡]├UëσWVΦ♂ ^_à÷t♥Φ↨ ]├Uëσë■─>▬2Φ╕ 
    î♠▬2ú↑2]├UëσSë√ç▐¼ç▐ä└t↑ë╟VP╕  δ◘XΦ.   δ♦XΦ( ^δ▀[]├ë±ë■1╥¼ê┬¼ê╞à
    └t◄â┬♣┤@∞ α≤Ét∙âΘ☺uσ├PQRë≥â┬♣┤ ∞ αu♦≤Éδ≈ë°ë≥εZYX├UëσS┤╕░ Ä└ë°<♥t
    ☻0φ1λj X═►┤☺░ ╡ ▒ ═►┤►░♥╖ │ ═►1└[]├Uëσâ∞►ûë∙ëμëτ¬ë╧j☺Zδ♫V¼ä└u√ë≥
    ^)≥UëσSë╤ë·ÇΓ ÇμÇÇ┬áÇ╓☼╖ │á9·t)w↕¼<◙t↕<♪t↕¬░•¬âΘ☺uΦë°[]├☺▀δ≥ë°R1
    ╥≈√)╫ZδμPQRVë▐P╕  δ◘XΦ♫   δ♦XΦ◘ ë╟^ZYXδ╞≈╥ë∙Çß ÇσÇç╧)±Q)∙ë■☺╬╕  
    ▲•≤ñX├Uëσ╕ S1█═§r→ü√MPu¶╕☺S1█═§1█1╔╕•S│☺▒♥═§Φ╢◙UëσΦ↔ Φy☺┐ ►╛  Φz
     r○Φ╟ Φ∞ Φ‘☻╕àDΦ♀■Uëσ£X% puMf£fXfë┴f╗    f1╪fPf¥f£fXf9┴t:f!╪fPf¥
    f┐   Çfë°fG☼óf9°|∟fë°☼óf┐    f!·f9·u○1└]├╕uDδ◙╕ÉDδ♣╕oDδ Φú²Uëσfh
    PAMSS┴∩♦Ä╟f1λf1█f╕ Φ  f╣↑   fïVⁿ═§r&f;Fⁿu à╔t♫â∙§r♠÷E¶☺u♥â╟↑fà█t
    ♦9≈r╦ë°[╔├∙δ·Uëσ·☼☺▬¿D☼ └♀☺☼“└δ ╣  Äß$■☼“└fΩrH    √]├UëσSf╛ @  f
    ╗  ► f╣↑   f┴ß○fâ╩ⁿfâ┬♦f9╤t♫dgfï♦▬dgfë♦‼δΘgfì∟‼┐  ╕ @Ä└ï♫@2ï▬B2à
    λt+ë°S1█ΦS°[)╟Që┴┴ß○1÷&fï♦dgfë♥fâ├♦â╞♦âΘ♦uδYδ╤[]├·▲1└Ä└HÄ╪┐ ♣╛►♣
    &è♣Pè♦P&╞♣ ╞♦λ&Ç=λXê♦X&ê♣▼u@╕☺ Φ, ░¡μdΦ% ░╨μdΦ% Σ`PΦ↑ ░╤μdΦ◄ X♀☻
    μ`Φ○ ░«μdΦ☻ δúΣd¿☻u·├Σd¿☺t·├√├Uëσ▲╕ lÄ╪f╟♠ 0♥α♠ f╟♠  ♥╨♠ f╟♠ ►♥└
    ♠ ╣ ☺f╕♥   1÷fë♦f♣ ►  â╞◘âΘ☺u∩▼f╟♠ 2 └♠ f╕ ≡♠ ☼“╪]├·☼☺▲02Φóλ☼ αf
    ♪á☻  ☼“αf╣Ç  └☼2f♪☺☺  ☼0☼☺▬¿D☼ └f♪♥  Çfâα√☼“└Ω≥I( ╕0   Ä╪ÄαÄΦ1╥δ
     HâΣ≡1φ1λΦ]•  ┐ ►  Φ{•  ┐ ►  ╛ ≡♠ ║ 2  Φ≡♣  ┐ùD  ï4% 0  Φ╠√λλ┐ 0
      ╛♦   Φτ√λλΘΓ•  Hì§█•  Φλ•  Θ╤•  É☼▼D  ☺   ♣             @     
      ►                      ►      ☺   ♠             @       ►     
     ►               ►      Qσtd♠                                   
            ►       ♦   ♦   P♂      P♂@     P♂►     ↑       ↑       
    ◘       É☼▼Ç    ◘   ♦   ☺   OpenBSD     É☼▼Ç    PE  då☻ k↕d╲    
        ≡ #☻♂☻♫☼            ╞¶        @      ►   ►  ♠       ♠       
     @   ►      ♥  ☺  ►       ♥       ◘      ►          ►           
    ä←  (                                                           
                            ►   @                           .text   
     ►   ►   ►   ►              `  p.data            ►              
        └  └É☼▼Ç    ╧·φ■•  ☺♥   ☻   ♣   ÿ☻  ☺       ↓   H   __PAGEZE
    RO                                                      ↓   ÿ   
    __TEXT            @                             •   ♣   ☺       
    __text          __TEXT           ►@      ►       ►  ♀           
     ♦              ↓   Φ   __DATA            @                     
     ►      •   ♥   ☻       __data          __DATA            @     
     ►          ♀                           __bss           __DATA  
             0@      ►          ♀           ☺               ←   ↑   
    B)→☺&¿◘ºB)→☺&¿◘º♣   ╕   ♦   *             @                     
                                                                    
                                    S↕@                             
            Éf.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä
         f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f☼▼ä     
    ZAR╕    Hà└t◙Hà└t♣Θ»♦  Θ@☻  UHë╤HëσAWIë≈AVA╛  @ AUIë²ATA╝  ► SHë
    √Hâ∞↑Hâ{◘ ☼ä╕   ïS►Hë╪Hâ├↑λ╩t○Hâ{◘ uδδ♥Hë├L9#L☼C#Iü─λ☼  IüΣ ≡λλH
    ïC◘H♥♥H% ≡λλI9─snIïU◘Hà╥t↓Aâ}►☺u♠Iâ┼↑δΩIïE H☺╨L9αrεIïE L9αw◄H☺╨L
    9αv○Lìáλ☼  δ½Lë·Lë≈╛♥   HëM╚Φ6   LëΓHïM╚Iü╞ ►  Hâ╩♥Iü─ ►  Hë►δÇH
    â├↑Θ=λλλHâ─↑[A╲A]A^A_]├I╣ ≡λλλλ♥ Ië╚╣‘   Hë°λ╬H╙Φ%λ☺  â■λu♣Hì♦┬├
    Hì¶┬Hâ: u‼Iï H- ►  Ië Hâ╚♥Hë☻Hï☻âΘ○L!╚Hë┬δ╛U╣ 2  ║ ≡♠ ╛♥   HëσSH
    ë√PΦÅλλλHâ ■☼☺;Z[]├1÷ë≡Hk└↑Hâ|•◘ t♦λ╞δεA╕☺   HìW↑A9≡r☺├UHëσATSLï
    ◙HïZ◘Hë╤Dë└DïZ►DïR¶Aë─λ╚L;IΦt→☼►AΦLïa°HâΘ↑☼◄A↑Lëa(à└u▌δ♥DëαHk└↑A
    λ└Hâ┬↑H☺°Lë◘HëX◘DëX►DëP¶A9≡rí[A╲]├1└├UëσSç█[fÉ╠·⌠δ²Uëσâ∞◘U^ì~°j◘
    Y1└≤¬·☼☺^°☼♂δ≈Uëσ]├╞♣ª↔  ◘δ►HàλH☼Eτt•╞♣ö↔   Lc$$Lìl$◘NìtΣ↑╕“↕@ ┐
    ► @ ╣P @ H)∙┴Θ♥≤H½1└â╔λLë≈≥H»IëλΘ╧   UHëσ┐ 0@ ╛°←@ ╣00@ HìA►Hë☺╟
    A◘ ♥  SV÷•◘u<÷• u!÷•♦u◄Iâ⁇ t!j j☺╛ⁿ←@ δ*j“j♦╛☻∟@ δ▼j0j ╛∙←@ δ¶j*
    j►╛λ←@ δ○jEj◘╛♦∟@ ╣q↓@ XH½XH☺╚H½W┐↑0@ Iâ╔λ╗(0@ H9▀s51╔1╥¼Ië└Aâα⌂
    I╙αâ┴•L○┬ä└xδ¿@t○Lë╚H╙αH○┬Hë╨Hâ⁇ H☼E•H½δ╞_^[╔├╕*   ├HâΣ≡1φ╗  @ Φ
    !λλλâ♪┴▼  ☺╕◘ @ ╣◘ @ H9┴t♀PQλ►YXHâ└◘δ∩ÉLëτLëεLë≥Φ╣λλλë╟Φ    UHëσ
    ATAëⁿ1λPΦr   DëτΦö♠  1÷1╥UHëσAW╣00@ Ië╧Iï•IïO◘Hà╔t∟HâΘ↑Hë¶◘Hët◘◘
    Hë|◘►IëO◘1└A_]├╣    Hà╔t≥PWVj!_j►^λ╤^_YHà└t▀I╟└ ♥  Ië•Jë♀ Lë┴δ┤U
    HëσAVAUATHï5α←  Ië■Ië⌠j YH¡HÆH¡HùH¡Hà└t↑Mà÷δ♣L9≥u♫VQQλ╨YY^1└HëF°
    âΘ☺u╘H¡Hà└t◄Mà÷t○PLëτΦfδ┐λ^δ╢A╲Mà÷u←╕► @ ╣► @ HâΦ◘H9╚|◘PQλ►YXδ∩A
    ]A^]├╠U╣ ◘  1└HëσAWAVAUATSHü∞Hα  Hëà└▼λλHëà╚▼λλλ§C♂  Hâ─ à└uGHâ∞
     H╟┴⌠λλλλ§“♂  Hâ─↑A╕♠   Lìì╨⌂λλj Hë┴║⌐→@ Hâ∞ λ§♪♂  XZ╣☺   λ§╨◙  
    Hâ─ Hâ∞ j♦Xë♣»→  λ§╔◙  Ië─λ§╚◙  LëτHâ─ A╕ ☻  Hìì╨/λλ║λ⁇  Hì╡╤⁇λλ
    Ië┼Φ╙☺  Aë─1└Hïì╨/λλH☺┴è◄ä╥t♪Ç·╲u♥╞☺/Hλ└δπA☼╖E f=λ╫w♪ëà╝▼λλ╕☺   
    δ☼Hì╡╝▼λλLë∩Φ0♦  ë└Hìì╨⌂λλE1└1█H☺└Ië╦LìU╬A╛²⌂  IìT♣ Hì╡╝▼λλâ╜╝▼λ
    λ ☼äÅ   Iλ└Iü°λ☺  w↕L9╤Hë╪H☼B┴Jëä┼╚▼λλDïì╝▼λλIc┴Aâ∙⌂v◘Dë╧ΦÉ♥  L9
    ╤s♀Hλ┴êAλH┴Φ◘u∩☼╖☻f=λ╫w♪ëà╝▼λλ╕☺   δ◘Hë╫Φö♥  ë└H☺└H☺┬Eà╔u¼Hë╚L)╪
    H=²⌂  I☼G╞╞ä♣╨⌂λλ ΘdλλλL9╤s♠╞☺ Hλ┴L)┘╕■⌂  Lì╡╨▼λλHü∙■⌂  Lì╜└▼λλH
    ☼G╚Iü°λ☺  ╕λ☺  L☼G└Hâ∞ ╞ä♪╨⌂λλ LëΘLì¡╨/λλJ╟ä┼╨▼λλ    λ§↔○  Θtⁿλλ
    Hë·Hï⁇☼╖•f=λ╫w◙ëB↑╕☺   δ○Hìr↑Φ▀☻  ë└H☺└H☺☻├Hë·Hc╞ë≈LïB►â■⌂v♣Φì☻ 
     HïJ◘I9╚v►Hìq☺Hër◘ê☺H┴Φ◘uτ├UHì♦▬Ië╙Më┬HëσAWI┐ &  ☺   AVAUIë⌡ATS1
    █Hâ∞8Hë}░Hì}░HëM¿Hëu╕HëE└Φbλλλâ}╚ t↔ïE╚à└t▬â° w╲I☼ú╟sVHì}░ΦAλλλδ
    π1÷Hì}░Φ_λλλMà█t↨HïE╕Iλ╦L)ΦL9╪I☼G├A╞D♣  Mà╥☼äf☺  Iλ╩HïE¿I9┌L☼G╙J
    ╟♦╨    ΘK☺  Hλ├L9╙s§HïE╕H;E└r☻1└HïU¿HëD┌°E1÷ïu╚à÷☼ä►☺  Eä÷u¶â■ w
    ☼I☼ú≈☼âσ   Θ≈   â■“t○â■╲☼à╥   E1Σâ}╚╲u♫Hì}░Iλ─Φì■λλδ∞E1╔â}╚“u♫Hì
    }░Φy■λλIλ┴δ∞LëαMà╔u↓Iλ╠IâⁿλtÅ╛╲   Hì}░ΦÇ■λλδτHâ°☺v∟╛╲   Hì}░HëEá
    Φf■λλHïEáHâΦ☻δ▐AÇΣ☺t↨╛“   Hì}░ΦH■λλIλ╔☼ä@λλλAÇ■☺A╛♥   Iâ┘ Mìa☺M9
    ⌠r¶╛“   Hì}░Iâ╞♥Φ▬■λλδτ1╥╣♥   Lë╚H≈±Hà╥A☼ö╞Θⁿ■λλHì}░Φ≥²λλHì}░Φ╛²
    λλΘσ■λλ1÷Hì}░Φ┘²λλΘG■λλHâ─8ë╪[A╲A]A^A_]├╠Hì♣╪↓  ├☼┐└à└x←Ië╩☼♣H=☺
    ≡λλs☺├≈╪ë♣║↓  jλX∙├ï♣⌂▬  δφH┴Φ0δ•H┴Φ ☼╖└f=λ☼sσIë╩☼♣r╙├Aë├┴Φ►%λ☼ 
     A┴δ∟A┴π↑D○╪δ┌QRëλ1└âλ⌂v“☼╜╧║┐→@ ïLJ≥ë·┴∩♠ÇΓ⁇♀Ç◘╨H┴α◘■╔u∞◘ΦH○°ZY
    ├U1└HëσWVSQRë┬ë├λ└☼╖¶Wë╤füß ⁿfü∙ ▄tΦfü∙ ╪t♦ë▬δ,☼╖♦Gë┴füß ⁿfü∙ ▄t
    ♂╟♠²λ  â╚λδ☼┴Γ◙ìä☻ $áⁿë♠ìC☻ZY[^_]├f☼▼D  Éâ♪▄↑  ♦÷♣ë§  ♦t♪@☼╢╧λ§ì
    ♣  δ°╠ï♣ö§  ☼♣·☼☺∟%Ü→@ ⌠δ²        f☼▼D  Énodll◙ KernelBase.dll ☺
    └☺└☺└☺└☻α☻α☻α☻α☻α♥≡♥≡♥≡♥≡♥≡♦°♦°♦°♦°♦°♣ⁿ♣ⁿ♣ⁿ♣ⁿ♣ⁿ♣ⁿÉÉ☼▼D  ⁇♣ExitPr
    ocess æ☺FreeEnvironmentStringsW ╜☺GetCommandLineW ‼☻GetEnvironme
    ntStringsW  ╘☻GetStdHandle  ╫♣SetDefaultDllDirectories  ▼•WriteF
    ile ░←          ░→  ►                       ☼▼@ °→      ♠←      
     ←      2←      L←      ╲←      x←              É☼▼Ç    É╬ ☺&τ☺╬
     ☺☺⌂╬ üÇÇ►f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼
    ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä   
      f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.
    ☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä 
        f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     
    f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼
    ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä   
      f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.
    ☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä 
        f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     
    f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼
    ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä   
      f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.
    ☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä 
        f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     
    f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     f.☼▼ä     ☼▼@ 
      @             °→      ♠←       ←      2←      L←      ╲←      
    x←                                                              

See also `build/objdump -xd o/tiny/examples/life.com.dbg | less` to view
assembly and the linker glue in [ape/ape.lds] that makes it all possible

## Licensing

Cosmopolitan is Free Software licensed under the GPLv2. The build config
**will embed all linked sources inside your binaries** so the compliance
is easy while facilitating trust and transparency similar to JavaScript.
You can audit your source filesystem using ZIP GUIs e.g. Win10, InfoZip.
That works easiest by changing the filename extension from .com to .zip.

### Commercial Support

If you want to be able to distribute binaries in binary only form, then
please send $1,000 to Justine Tunney <jtunney@gmail.com> on PayPal, for
a license lasting 1 year. This README will be updated in the event that
this needs to change. Please reach out if there's anything you need.

## Contributing

We'd love to accept your patches! Before we can take them, we have to
jump through one legal hurdle. Please write an email to Justine Tunney
<jtunney@gmail.com> saying you agree to give her copyright ownership to
any changes you contribute to Cosmopolitan. We need to do that in order
to dual license Cosmopolitan. Otherwise we can't create incentives that
encourage corporations to share their source code with the community.

## Volunteering

We also need volunteers who can help us further stabilize System Five's
application binary interface. See our ABI scripts [libc/sysv/consts.sh]
and [libc/sysv/syscalls.sh]. Magic numbers are usually stabler than API
interfaces cf. NPM but we should ideally have fewer of them, similar to
how SI has sought to have fewer defining physics constants.

## About

Cosmopolitan mostly stands on the shoulders of giants and has few novel
ideas, aside from taking care of low-level build system toil, so coding
can become more beautifully pleasant.

Cosmopolitan is maintained by Justine Tunney, who previously worked on
projects like [TensorFlow], [Operation Rosehub], [Nomulus], and Google's
tape backups SRE team. She's also helped activists by operating the
[Occupy Wall Street] website. Justine Tunney currently isn't on the
payroll of any company. She's been focusing on Cosmopolitan because she
wants to give back to Free Software which helped her be successful. See
her [LinkedIn] profile for further details on her professional history.


[LinkedIn]: https://www.linkedin.com/in/jtunney
[Nomulus]: https://github.com/google/nomulus
[Occupy Wall Street]: https://www.newyorker.com/magazine/2011/11/28/pre-occupied
[Operation Rosehub]: https://opensource.googleblog.com/2017/03/operation-rosehub.html
[TensorFlow]: https://github.com/tensorflow/tensorflow
[ansitrinsics library]: libc/intrin
[ape/ape.lds]: ape/ape.lds
[dsp/scale/cdecimate2xuint8x8.c]: dsp/scale/cdecimate2xuint8x8.c
[examples/life.c]: examples/life.c
[libc/nexgen32e/kcpuids.S]: libc/nexgen32e/kcpuids.S
[libc/nexgen32e/memcpy.S]: libc/nexgen32e/memcpy.S
[libc/nexgen32e/x86feature.h]: libc/nexgen32e/x86feature.h
[libc/sysv/consts.sh]: libc/sysv/consts.sh
[libc/sysv/syscalls.sh]: libc/sysv/syscalls.sh
[third_party/xed/x86ild.greg.c]: third_party/xed/x86ild.greg.c
[tool/build/package.c]: tool/build/package.c
[tool/emacs/cosmo-stuff.el]: tool/emacs/cosmo-stuff.el
[tool/viz/lib/ycbcr2rgb3.c]: tool/viz/lib/ycbcr2rgb3.c

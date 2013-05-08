Kube
====

For now, this project serves as a distraction to the things I should
actually be working on.  If I'm going to be distracted, I may as well
try to learn from it. In this codebase I'm trying to use C++11 and
(somewhat) Go/Rust style OO to make a simple, low complexity but
efficient Minecraft-thing. Don't get your hopes up; at the moment, it
is none of these things.

Eventually, I would love for this to become the base of the numerous
Minecraft full-conversion mods, such as Civcraft, Aether mod,
TerraFirmaCraft, and many others. The plan is to give players an
"empty" client, which can connect to any server and play any game by
downloading all the mods from the server which make up the game. Mods
will include custom graphics, as well as client and server code
written in Lua and ran by LuaJIT, which is quite fast. The most
performant common parts of the engine such as chunk rendering and
network code will be baked into the C++ engine, but will be
customizable through Lua as much as possible.

I fully expect this to go dormant for many months at time when
other projects get hectic, so I'm not really looking to collaborate
with anyone yet. I would be interested in hearing from you if you have
ideas or experience in dealing with high performance cube engines.



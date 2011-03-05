Dragonlance TC Editor Pro
Unofficial game file editor/checker/browser for IE (Infinity Engine) games.
Official game file editor for GemRB

Current version: V7.2d

Changes:

V7.2d
- GemRB specific spell and projectile fields

V7.2c
- fixed default saving throws in creature editor
- try to preserve the order of spell indices in spell pages (weidu chokes on an unordered spellbook)
- fixed some iwd effect descriptions
- fixed whole bam importer frame count in cycles

V7.2b
- updated effect targeting types
- added whole bam import from bmp sequences
- slider control unknown fields are not unknown anymore
- window 0x1a field is not unknown anymore

V7.2a
- fixed button disabled/selected fields in the chu editor

V7.2
- more item extended header checks and edited fields
- falling back to load tileset from same directory as .are, if it isn't in the game override
- fixed a crasher bug in the polygon selector

V7.1f
- creature editor knows more pst specific fields

V7.1e
- extended area projectile editor with gemrb specific fields
- fixed tab order in animation/palette editor (a long forgotten request)
- area link flags
- 8 bits heightmap editor

V7.1d
- fixed var.var editor to look up the initial values
- improved projectile finder (now it can look for projectiles in projectiles)
- hopefully removed all references to the old dltc homepage which fell prey to some domain squatter

V7.1c
- spawn.ini checker (pst/iwd/how/iwd2)
- gemrb specific projectile flags

V7.1b
- You can edit the dream movies in areas (see pocket plane in tob)
- At least one crash bugfix i got somewhere smile.gif
- internal script compiler now somewhat works! (I still don't recommend it for real use)
- iwd2 creature levels are now editable
- kit selector combobox is filled (you need a correct kit.ids)

V7.1a
- grid/polygon color is customisable in the .ini file
- window placement improved

V7.1
- implemented .sav repackaging
- fixed cbf compression
- added remove all button to area actor editor (removes all after the selected one)
- fixed tob .gam editor (familiars)

V7.0k
- fixed a problem with bam preview

V7.0j
- fixed problem with pst creature writer (saving corrupted creatures with overlays)
- ability to edit familiar structure in iwd2
- updated iwd2 opcode list

V7.0i
- applied Igi's patch for filedialog (with modification) 

V7.0h
- added more iwd2 specific fields (feats/skills)
- swapped the bg1 area exits
- fixed spell forgetting not to forget multiple spells in some cases

V7.0g
- added some iwd2 specific creature fields
- fixed a small screwup of tutu converted items causing trouble in the item loader

V7.0f
- hot buttons changed from checker to editor because editors are more frequently used
- removed extra debug code from area searcher
- fixed possible memory corruption in area mirror tool

V7.0e
- fixed area editor crasher (removing containers)
- fixed area container/door copy/paste
- added polygon count checker for iwd2 walls

V7.0d
- fixed src crasher

V7.0c
- zoom function enlarges bam by 2 in bam editor

V7.0b
- updated pst missile ids (you have to copy it as missile.ids into your game override)
- fixed chr save
- fixed some effect descriptions

V7.0a
- fixed a crash with iwd2 creature soundsets
- you can disable force new strref on import tbg (this is the default)

V7.0
- TP2 generation for files
- internal script decompiler 

V6.8c
- fixed change detection after .pro file saving
- converts :'s to \\ in the chitin.key (so a chitin.key from mac could be used?)

V6.8b
- fixed PST creature loader when there is no overlay structure

V6.8
- fixed area container inserting when there was no door
- added bam splitter
- fixed a few effect descriptions
- fixed old bug in effect loader causing intermittent problems in effect list
- improved PST effect list
- fixed PST game saver (bestiary offset wasn't recalculated)
- fixed PST creature loader (overlay structure handled)

V6.7m
- effect editor accepts hexadecimal opcode numbers
- fixed number of attacks list in creature editor
- improved BG2 effects list
- added missile.ids for iwd2 (but it is useful for bg1 too)

V6.7l
- modified the saving order of store files to match the original world editor order
- added vertical swapping of bam frames

V6.7k
- fixed iwd2 saving throw types

V6.7j
- fixed a possible crash in the door tile editor
- ignore *.ini CD paths option (autogenerating paths seems more reliable for full install)

V6.7i
- fixed ToB game loading
- improved game structure editing

V6.7h
- fixed PST game saving
- added search values in ids file editor
- improved area editor

V6.7f
- fixed PST creature color selection

V6.7e
- fix: fixed duration/damage tool
- add: added a VVC sequence bit
- updated iesdp format descriptions

V6.7d
- fix: a crasher in tis editor
- fix: palette in multi frame import is now fixed
- fix: fixed some checks

V6.7c
- add: added mass dialog decompile function

V6.7b
- add: two new palette manipulation functions
- add: dltcep recognises the rgb format in sprkclr.2da
- fix: fixed resource searching in effV1 creature effects (crasher), bugged recently
- fix: some effect updates

V6.7a
- add: added dual tlk editing, resync tlks, etc.
- add: added ignore filesize option for large files

V6.7
- add: added support for embedded creatures and trap projectiles in area editor
- add: dialog searcher: outputting which transition contains the action/trigger
- fix: probably fixed creature saving in iwd/pst (iwd2 still unsafe, bg1/bg2 unchanged)
- fix: improved creature checker (dialog, twohanded weapons with shields)

V6.6a
- fix: fixed loading of special gam structures: pst modron maze, bg2 saved locations
- fix: improved door editor
- fix: fixed a bug in the chui editor

V6.6
- fix: overlay handling (minimap, display)
- fix: tile selection
- add: chitin.key unpacker/repacker

V6.5g
- fix: fixed the 'skim savegame' feature so it won't be 'screw savegame' anymore
- add: tavern quality flags (bg1/bg2)
- add: guess dimensions of tileset

V6.5f
- fix: fixed pst explored bitmap editing
- fix: fixed false warning of wed changed when it didn't
- add: drop 10 frames of a cycle function for faster frame removal
- add: added many checks to warn about unsaved changes (not complete)

V6.5e
- fix: if the effect description isn't set yet, try to set it based on the gametype
- add: new setup button
- fix: speed of generating minimap (which was slowed down by the changes in V6.5d)
- add: preview of chui files (shows the background of the first window)

V6.5d
- fix: optimized speed and memory usage of green water removal

V6.5c
- fix: fixed throwing dagger animation

V6.5b
- add: night lightmap editing is simpler

V6.5a
- add: saving/loading polygons in .ply format
- add: region link selection from available triggers
- add: automatically fix height/searchmaps with invalid palette size
- add: improved script checker to check object death variables (may have false reports)
- add: explored area editor
- fix: fixed floodfill freeze for impeded blocks/explored map editor
- fix: fixed night minimap generation (night minimap can be generated from the night wed)

V6.5
- add: option to not center on vertex in polygon editor
- add: travel trigger vs. door matching (used by door flag/region link checks)
- fix: enlarged region link field to 16
- fix: hopefully fixed all polygon corruption in area editor
- fix: hopefully fixed rare crashes due to empty polygons
- fix: fixed a crasher when editing polygons/impeded blocks of small areas
- fix: fixed area corruption when copying items
- fix: fix areas on save when they are corrupted by the previous bug

V6.4b
- fix: shallow water searchmap entry
- add: play all cycles area animation bit
- add: left click on aux map will bring the map editor up centered on that point
- add: align bam frames of a single cycle
- add: importing external .tis/.bmp door tiles

V6.4a
- fix: crasher in game editor (add/remove journal entry)

V6.4 and before
- implemented all the rest...

Features:

- checks most filetypes for possible crashers, bugs or cosmetical errors.
- provides fixes of various errors made by other editors. (MOS, creature, dialog, area, BAM, KEY, BIF )
- extracts most of the filetypes (able to extract all files of a certain type, or all files in a bif or files matching a pattern).
- looks for files matching a specification of field values (for example you can look for all evil creatures).
- able to manipulate or create most of the filetypes with various degree of usefulness.
- able to compress or decompress bifs.

Progress status:

1. Consistency checkers
 They work for BG2, some of them works for IWD2 and other engines.
 They are safe for running, but they may give false reports.

2. Editors
 Item editor - Works for all engines (PST/BG1/BG2/IWD1/IWD2)
 Spell editor - Works for all engines (PST/BG1/BG2/IWD1/IWD2)
 Store editor - Works for all engines (PST/BG1/BG2/IWD1/IWD2)
 Creature editor - Works for all engines, but doesn't fully support IWD2
 Area editor - Works for all engines, fully supports wed/tis editing (door addition/removal, vertex manipulations, overlays), it can convert BG1 areas to BG2 (green water)
 PRO/VVC/EFF editors - Work for BG2 (the other engines might not support these formats)
 Dialog editor - Works for all engines
 Game editor - Works for all engines, it doesn't support full savegame editing. It can be used to add NPC's to a new game.
 BAM editor - Works for all engines. It can compress/decompress BAMs, manipulate palette, add animation cycles
 Worldmap editor - Works for all engines, can edit multi map worldmaps
 MOS editor - Works for all engines, it can compress/decompress MOS files, fix transparency
 TIS editor - Works for all engines
 2DA editor - Works for all engines
 IDS editor - Works for all engines
 MUS editor - Works for all engines
 TLK editor - Works for all engines
 KEY editor - Works for all engines
 Handle the editors with care, make backups before modify items, bams, etc.
 If you find a reproducible bug or you have any idea how could I improve this tool, please notify me via the DLTCEP Support Board. (www.dragonlancetc.com/dltcep)

3. Resource finders
 They work and safe for running. Occasional crashes were reported.

4. Resource extractor
 It works for all engines, but CD extraction may be tricky. Extraction from compressed bifs now works flawlessly, no need to decompress the whole bif.

5. Bif/sav decompressor/compressor
 It works for all engines.

6. TBG import/export
 Supports the TBG3, TBG4, TBGN and IAP fileformats

Known bugs:

The dialog tree recoils when changing any leaf (annoying)
The stop button doesn't work during searching/checking

Known problems:

Problem:
 The script checker reports [ANYONE] as an invalid IDS value.
Reason:
 The symbol isn't in the EA.ids (or other) file. It is working only because the game replaces invalid symbols by 0.
Solution:
 Add the line 0 ANYONE to your EA.ids (or other).

Problem:
 The script checker reports [ANYONE] as an invalid object.ids value.
Reason:
 The symbol isn't in the object.ids file. It is working only because the game replaces invalid symbols by 0.
Solution:
 Add the line 0 ANYONE to your OBJECT.ids.

Problem:
 The consistency checkers report a lot of problems.
Reason:
 It reports different severity problems. Some cause crashes, some cause visual problems, some are unnoticed in the game and some are deliberate tweaks.
Solution:
 Look at your 'Check' menu in the setup and turn off some checks. If you don't care about original game errors, turn the 'skip original files' option on.

Problem:
 The script editor doesn't refresh line numbers when dragging the scrollbar.
Reason:
 When the edit box has no focus Windows doesn't send the same message (and i'm too lazy/inexperienced to find the solution).
Solution:
 Make sure the text edit box has the input focus by clicking into it.

Problem:
 The area editor shows garbled graphics when using graphical placement tools (the placed animation itself seems right, but the area graphics is scrambled).
Reason:
 This has been fixed (i think).
Solution:
 Make sure you use your display in 32 bit color depth.

Problem:
 Effect numbers are displayed twice in the effect names.
Reason:
 The effect descriptions contain the numbers, but DLTCEP adds them too.
Solution:
 Turn on the 'don't number effects' edit option.

Problem:
 DLTCEP is complaining about missing shout.ids, moraleai.ids, color.ids or similar.
Reason:
 The action/trigger ids files contain references to nonexistent files.
Solution:
 Fix your action.ids, trigger.ids files or add an empty file in place of the missing one.

Problem:
 The modified file doesn't contain the modification. This happens only when modifying an original game file. It also happens
when the modification was made by another tool while DLTCEP is running.
Reason:
 DLTCEP loads chitin.key and override information once at startup (or at reload chitin or setup change) to speed up file access.
Solution:
 Load the new file by the 'load external' button. Or reload chitin.key from the main menu.

Credits:

Maltanar - for sharing information and code
Theo - for sharing information and code
Ken Baker - for starting TeamBG (now defunct)
Max/Potencius - for creating TDD and DLTC (dragonlancetc.com, defunct site)
Manveru - for starting IESDP, a great site for IE file formats (now defunct)
Igi - for maintaining IESDP and creating DLTCEP tutorials (iesdp.gibberlings3.net)
Yovaneth - for creating the most comprehensive DLTCEP area editing tutorial (http://www.simpilot.net/~sc/dltcep/index.htm)
Jon Olav Hauglid - author of NI, for clearing up some unknowns (www.idi.ntnu.no/~joh/ni/index.html)
Abel@TeamX - for the ACM handler routines (www.teamx.ru)
Westley Weimer - for WeiDU (weidu.org)
Per Olafsson - for TisPack
Several other people - for providing bits of information and reporting bugs
Ian Ashdown - for the octree algorithm (fast color reduction)
Jean-loup Gailly and Mark Adler - for their static zlib module
Bioware & Black Isle - for the IE engine and the games

Additional information:

Read the accompanying textfiles for instructions on editing various filetypes.

For modding tips, information, tools and other resources visit us at www.gibberlings3.net!
For DLTCEP specific information, visit the DLTCEP homepage at http://forums.gibberlings3.net/index.php?showforum=137

The (somewhat outdated) sources for this program are available on gemrb.git.sourceforge.net under module dltcep.
Modifications to this program may not be misrepresented as the original, otherwise you are free to port it to other systems (i would like to know about it).
DLTCEP is constantly growing, generally I release a new version each week (or month) therefore if you altered the program it is better you supply the patch on the sourceforge site.

If you have created a public mod (or TC) with the help of this tool, please mention that in your mod's readme (if there is any).

Avenger_teambg

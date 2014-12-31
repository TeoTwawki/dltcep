Dragonlance TC Editor Pro
Unofficial game file editor/checker/browser for old IE (Infinity Engine) games.
Official game file editor for GemRB and BGEE

Current version: V7.7j

Changes:

v7.7j
- improved stencil bmp reader
- improved the spell list generator for BGEE
- permanent/allow save projectile flags
- read plts as greyscale bmp
- fixed chitin exploder to handle upper case bif extensions correctly
- added a bunch of experimental opcode descriptions
- added lance projectile width for BGEE
- added a bunch of other projectile flags for BGEE

v7.6g
- added bgee specific flag to area (per area water transparency)
- fixed area editor resizing (again)
- added use_lang flag to weidu parameters
- fixed extracting tileset into bmp

v7.6f
- fixed area editor window resizing
- fixed many off by one errors in the area editor that may have caused memory corruption
- you can change ambient radius via mousewheel in the ambient selection mode
- shift click will relocate the ambient in ambient selection mode
- spell assign key entry generator for BGEE
- unused large files finder for BGEE
- creature loader updates window before small inconsistency popup appears (otherwise it breaks the load)

v7.6e
- don't corrupt area if it has map notes but no rest interruption header
- item bam checker improvements
- correctly allocate wed walls to wallgroup lists
- auto correct missing tileset offset
- some new flags in area and projectile (BGEE specific)

v7.6d
- scanning of variables now handles scripting names in areas
- updated resource checking
- vef editor

V7.6b
- ambush area reference checks (wmp)
- fixed item description checker feedback
- fixed several problems in the wed editor
- added .glsl fileformat to the resource list (textfile)
- fixed last opened bcs/baf logic

V7.6a
- fixed a bug with loading saved games while in the pocket plane (ToB)
- you can add empty overlays in wed editor
- entrance names are generated without space
- tileset checker is aware of new tileset format
- fix zero frames in BAMs
- new functionality: fix linefeed problems in tlk
- proper bam splitting

V7.6
- game and dialog editor asks before quitting after changed
- worldmap editor - now it is possible to select area from worldmap
- fixed handling of four letter extensions (pvrz)
- csv import/export for tlk

V7.5c
- fixed bmp saving
- more item checking functionality
- fixed spawn method names
- area animation height field accepts negative numbers

V7.5b
- improved the dialog editor
- scrollbar ID in editbox (might be BGEE only)
- saving a composite description icon BAM as single BMP (BGEE)
- fixed displaying BMP description icons (BGEE)


V7.5a
- halfway compatible with BGEE compressed tilesets

V7.5
- priority for BMP over BAM when displaying description icons
- key editor won't complain about bifs in lang path
- dialog checker improvements

V7.4i
- text search will highlight the find
- new tool: load all dialogs into a single massive tree view (useful to find orphan states or to browse the whole dialog structure without switches)
- fixed extraction from saved games (filename filter, location)
- bgee minimap creation (new resizing factor)
- bg1 -> bg2 mass conversion of creature effects

V7.4h
- improved creature searching (by dialog/script)
- added GUI for ambient selection in area editor
- improved string reference verifications (length limits)
- implemented add/remove animated tile frame
- night conversion of individual tiles (handle with care!)
- improved handling of corrupt tile references in .wed
- night conversion now works directly from area editor too
- better kit 2da checking (looks into all CLAB)

V7.4g
- night lightmap converter
- ogg/vorbis support

V7.4f
- fixed parsing of globaltimer triggers (hopefully)
- night tileset converter
- handle biffed chr files

V7.4e
- fixed registering of AUX map changes when they first go into override
- language setting in WEIDU options (affects resource location too)
- removed buggy directory changing introduced in 7.4b
- fixed .gam editor to still start in the last savegame dir (you need to turn off 'always start in default folder')


V7.4d
- added composite dialog viewing mode (loads referenced dialogs into the treeview)
- fixed tileset saving broken in v7.4b

V7.4c
- check tags in tlk editor now works
- overlapping ambient check
- colliding region/doorname check
- fixed trigger searching in dialogs
- configurable area editor window size

V7.4b
- more checking/searching abilities for various filetypes
- file picker now starts in the same directory you left it previously (no more resetting to the override)
-

V7.4a
- can automatically correct some item corruptions caused by WeiDU.
- more updated fields from BGEE public information
- improved 2da/effect checking abilities


V7.4
- updated/clarified several unknown fields based on BGEE developer information

V7.3d
- fixed reading of some unterminated 2da's (eg.: speldesc.2da)

V7.3c
- fixed WFX menu
- fixed probability checker (allow equal min and max probability)

V7.3
- Added WFX editor
- show all option in area animation placement
- fixed aux map editor y offset bug
- more editable fields in iwd2 creature files

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

V7.0 and before
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
Max/Potencius - for creating TDD and DLTC (www.dragonlancetc.com, and people who revived the site!)
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
Overhaul Games - for recent upgrades and information on the IE engine

Additional information:

Read the accompanying textfiles for instructions on editing various filetypes.

For modding tips, information, tools and other resources visit us at www.gibberlings3.net!
For DLTCEP specific information, visit the DLTCEP homepage at http://forums.gibberlings3.net/index.php?showforum=137

The (somewhat outdated) sources for this program are available on gemrb.git.sourceforge.net under module dltcep.
Modifications to this program may not be misrepresented as the original, otherwise you are free to port it to other systems (i would like to know about it).
DLTCEP is constantly growing, generally I release a new version each week (or month) therefore if you altered the program it is better you supply the patch on the sourceforge site.

If you have created a public mod (or TC) with the help of this tool, please mention that in your mod's readme (if there is any).

Avenger_teambg

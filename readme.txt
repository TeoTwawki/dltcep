Dragonlance TC Editor Pro
Unofficial game file editor/checker/browser for IE (Infinity Engine) games.

Current version: V6.7j

Changes:

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
Max/Potencius - for creating TDD and DLTC (dragonlancetc.com)
Manveru - for starting IESDP, a great site for IE file formats (now defunct)
Igi - for maintaining IESDP and creating DLTCEP tutorials (iesdp.gibberlings3.net)
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

For modding tips, information, tools and other resources visit us at www.forgottenwars.net!
The DLTCEP support forum is on the following website: www.dragonlancetc.com/forums/index.php?board=9

The (somewhat outdated) sources for this program are available on gemrb.sourceforge.net in the CVS under module Utilities.
Modifications to this program may not be misrepresented as the original, otherwise you are free to port it to other systems (i would like to know about it).
DLTCEP is constantly growing, generally I release a new version each week (or month) therefore if you altered the program it is better you supply the patch on the sourceforge site.

If you have created a public mod (or TC) with the help of this tool, please mention that in your mod's readme (if there is any).

Avenger_teambg

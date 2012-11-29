# Microsoft Developer Studio Project File - Name="chitem" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=chitem - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "chitem.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "chitem.mak" CFG="chitem - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chitem - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "chitem - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "chitem - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Z<none>
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib StaticZlib.lib jpeg.lib ogg_static.lib vorbisfile_static.lib vorbis_static.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /out:"Release/DLTCEP.exe"

!ELSEIF  "$(CFG)" == "chitem - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib StaticZlib.lib jpeg.lib ogg_static_d.lib vorbisfile_static_d.lib vorbis_static_d.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /out:"Debug/DLTCEP.exe" /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "chitem - Win32 Release"
# Name "chitem - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\2da.cpp
# End Source File
# Begin Source File

SOURCE=.\2DAEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\acm2snd.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\area.cpp
# End Source File
# Begin Source File

SOURCE=.\AreaEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\AreaInt.cpp
# End Source File
# Begin Source File

SOURCE=.\AreaProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\AreaSong.cpp
# End Source File
# Begin Source File

SOURCE=.\AreaTriggerString.cpp
# End Source File
# Begin Source File

SOURCE=.\bam.cpp
# End Source File
# Begin Source File

SOURCE=.\BamEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Bif.cpp
# End Source File
# Begin Source File

SOURCE=.\bitstream.cpp
# End Source File
# Begin Source File

SOURCE=.\CFBDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\chitem.cpp
# End Source File
# Begin Source File

SOURCE=.\chitem.rc
# End Source File
# Begin Source File

SOURCE=.\chitemDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\chui.cpp
# End Source File
# Begin Source File

SOURCE=.\ChuiEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorPicker.cpp
# End Source File
# Begin Source File

SOURCE=.\compat.cpp
# End Source File
# Begin Source File

SOURCE=.\Compiler.cpp
# End Source File
# Begin Source File

SOURCE=.\creature.cpp
# End Source File
# Begin Source File

SOURCE=.\CreatureEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\CreatureLevels.cpp
# End Source File
# Begin Source File

SOURCE=.\CreatureOverlay.cpp
# End Source File
# Begin Source File

SOURCE=.\CreatureProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\decoder.cpp
# End Source File
# Begin Source File

SOURCE=.\Decompiler.cpp
# End Source File
# Begin Source File

SOURCE=.\dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogLink.cpp
# End Source File
# Begin Source File

SOURCE=.\Dl1quant.cpp
# End Source File
# Begin Source File

SOURCE=.\effect.cpp
# End Source File
# Begin Source File

SOURCE=.\EffEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\FileExtract.cpp
# End Source File
# Begin Source File

SOURCE=.\FindItem.cpp
# End Source File
# Begin Source File

SOURCE=.\game.cpp
# End Source File
# Begin Source File

SOURCE=.\GameEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\GameGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\GameStatistics.cpp
# End Source File
# Begin Source File

SOURCE=.\IapDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\IcewindCre.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageView.cpp
# End Source File
# Begin Source File

SOURCE=.\ini.cpp
# End Source File
# Begin Source File

SOURCE=.\item.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemPicker.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\IWD2Creature.cpp
# End Source File
# Begin Source File

SOURCE=.\jpegcompress.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\map.cpp
# End Source File
# Begin Source File

SOURCE=.\MapEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\massclear.cpp
# End Source File
# Begin Source File

SOURCE=.\mos.cpp
# End Source File
# Begin Source File

SOURCE=.\MosEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\MyFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\oct_quan.cpp
# End Source File
# Begin Source File

SOURCE=.\options.cpp
# End Source File
# Begin Source File

SOURCE=.\packer.cpp
# End Source File
# Begin Source File

SOURCE=.\PaletteEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\PCInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Polygon.cpp
# End Source File
# Begin Source File

SOURCE=.\progressbar.cpp
# End Source File
# Begin Source File

SOURCE=.\proj.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjGemRB.cpp
# End Source File
# Begin Source File

SOURCE=.\pvr.cpp
# End Source File
# Begin Source File

SOURCE=.\readers.cpp
# End Source File
# Begin Source File

SOURCE=.\riffhdr.cpp
# End Source File
# Begin Source File

SOURCE=.\Schedule.cpp
# End Source File
# Begin Source File

SOURCE=.\script.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchNode.cpp
# End Source File
# Begin Source File

SOURCE=.\seekers.cpp
# End Source File
# Begin Source File

SOURCE=.\snd2acm.cpp
# End Source File
# Begin Source File

SOURCE=.\spell.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src.cpp
# End Source File
# Begin Source File

SOURCE=.\SRCEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\store.cpp
# End Source File
# Begin Source File

SOURCE=.\StoreEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\StoreProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\StrRefDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\subband.cpp
# End Source File
# Begin Source File

SOURCE=.\tbg.cpp
# End Source File
# Begin Source File

SOURCE=.\TextView.cpp
# End Source File
# Begin Source File

SOURCE=.\TisDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\tispack.cpp
# End Source File
# Begin Source File

SOURCE=.\tlkhandler.cpp
# End Source File
# Begin Source File

SOURCE=.\TormentCre.cpp
# End Source File
# Begin Source File

SOURCE=.\unpacker.cpp
# End Source File
# Begin Source File

SOURCE=.\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\variables.cpp
# End Source File
# Begin Source File

SOURCE=.\VVC.cpp
# End Source File
# Begin Source File

SOURCE=.\VVCEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\WedEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\WedPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\WedTile.cpp
# End Source File
# Begin Source File

SOURCE=.\WeiduLog.cpp
# End Source File
# Begin Source File

SOURCE=.\WFX.cpp
# End Source File
# Begin Source File

SOURCE=.\WFXEdit.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\2da.h
# End Source File
# Begin Source File

SOURCE=.\2DAEdit.h
# End Source File
# Begin Source File

SOURCE=.\acmsound.h
# End Source File
# Begin Source File

SOURCE=.\AnimDialog.h
# End Source File
# Begin Source File

SOURCE=.\area.h
# End Source File
# Begin Source File

SOURCE=.\AreaEdit.h
# End Source File
# Begin Source File

SOURCE=.\AreaInt.h
# End Source File
# Begin Source File

SOURCE=.\AreaProperties.h
# End Source File
# Begin Source File

SOURCE=.\AreaSong.h
# End Source File
# Begin Source File

SOURCE=.\AreaTriggerString.h
# End Source File
# Begin Source File

SOURCE=.\bam.h
# End Source File
# Begin Source File

SOURCE=.\BamEdit.h
# End Source File
# Begin Source File

SOURCE=.\Bif.h
# End Source File
# Begin Source File

SOURCE=.\bitstream.h
# End Source File
# Begin Source File

SOURCE=.\CFBDialog.h
# End Source File
# Begin Source File

SOURCE=.\chitem.h
# End Source File
# Begin Source File

SOURCE=.\chitemDlg.h
# End Source File
# Begin Source File

SOURCE=.\chui.h
# End Source File
# Begin Source File

SOURCE=.\ChuiEdit.h
# End Source File
# Begin Source File

SOURCE=.\ColorPicker.h
# End Source File
# Begin Source File

SOURCE=.\compat.h
# End Source File
# Begin Source File

SOURCE=.\Compiler.h
# End Source File
# Begin Source File

SOURCE=.\creature.h
# End Source File
# Begin Source File

SOURCE=.\CreatureEdit.h
# End Source File
# Begin Source File

SOURCE=.\CreatureLevels.h
# End Source File
# Begin Source File

SOURCE=.\CreatureOverlay.h
# End Source File
# Begin Source File

SOURCE=.\CreatureProperties.h
# End Source File
# Begin Source File

SOURCE=.\decoder.h
# End Source File
# Begin Source File

SOURCE=.\Decompiler.h
# End Source File
# Begin Source File

SOURCE=.\dialog.h
# End Source File
# Begin Source File

SOURCE=.\DialogEdit.h
# End Source File
# Begin Source File

SOURCE=.\DialogLink.h
# End Source File
# Begin Source File

SOURCE=.\Dl1quant.h
# End Source File
# Begin Source File

SOURCE=.\effect.h
# End Source File
# Begin Source File

SOURCE=.\EffEdit.h
# End Source File
# Begin Source File

SOURCE=.\FileExtract.h
# End Source File
# Begin Source File

SOURCE=.\FindItem.h
# End Source File
# Begin Source File

SOURCE=.\game.h
# End Source File
# Begin Source File

SOURCE=.\GameEdit.h
# End Source File
# Begin Source File

SOURCE=.\GameGeneral.h
# End Source File
# Begin Source File

SOURCE=.\GameStatistics.h
# End Source File
# Begin Source File

SOURCE=.\general.h
# End Source File
# Begin Source File

SOURCE=.\IapDialog.h
# End Source File
# Begin Source File

SOURCE=.\IcewindCre.h
# End Source File
# Begin Source File

SOURCE=.\ImageView.h
# End Source File
# Begin Source File

SOURCE=.\ini.h
# End Source File
# Begin Source File

SOURCE=.\item.h
# End Source File
# Begin Source File

SOURCE=.\ItemEdit.h
# End Source File
# Begin Source File

SOURCE=.\ItemPicker.h
# End Source File
# Begin Source File

SOURCE=.\ItemProperties.h
# End Source File
# Begin Source File

SOURCE=.\IWD2Creature.h
# End Source File
# Begin Source File

SOURCE=.\jconfig.h
# End Source File
# Begin Source File

SOURCE=.\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=.\jpegcompress.h
# End Source File
# Begin Source File

SOURCE=.\jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\KeyEdit.h
# End Source File
# Begin Source File

SOURCE=.\map.h
# End Source File
# Begin Source File

SOURCE=.\MapEdit.h
# End Source File
# Begin Source File

SOURCE=.\massclear.h
# End Source File
# Begin Source File

SOURCE=.\mos.h
# End Source File
# Begin Source File

SOURCE=.\MosEdit.h
# End Source File
# Begin Source File

SOURCE=.\MyAfxtempl.h
# End Source File
# Begin Source File

SOURCE=.\MyFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\oct_quan.h
# End Source File
# Begin Source File

SOURCE=.\ogg.h
# End Source File
# Begin Source File

SOURCE=.\options.h
# End Source File
# Begin Source File

SOURCE=.\os_types.h
# End Source File
# Begin Source File

SOURCE=.\packer.h
# End Source File
# Begin Source File

SOURCE=.\PaletteEdit.h
# End Source File
# Begin Source File

SOURCE=.\PCInfo.h
# End Source File
# Begin Source File

SOURCE=.\Polygon.h
# End Source File
# Begin Source File

SOURCE=.\progressbar.h
# End Source File
# Begin Source File

SOURCE=.\proj.h
# End Source File
# Begin Source File

SOURCE=.\ProjEdit.h
# End Source File
# Begin Source File

SOURCE=.\ProjGemRB.h
# End Source File
# Begin Source File

SOURCE=.\pvr.h
# End Source File
# Begin Source File

SOURCE=.\readers.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.hm
# End Source File
# Begin Source File

SOURCE=.\riffhdr.h
# End Source File
# Begin Source File

SOURCE=.\Schedule.h
# End Source File
# Begin Source File

SOURCE=.\script.h
# End Source File
# Begin Source File

SOURCE=.\ScriptEdit.h
# End Source File
# Begin Source File

SOURCE=.\SearchNode.h
# End Source File
# Begin Source File

SOURCE=.\spell.h
# End Source File
# Begin Source File

SOURCE=.\SpellEdit.h
# End Source File
# Begin Source File

SOURCE=.\SpellProperties.h
# End Source File
# Begin Source File

SOURCE=.\src.h
# End Source File
# Begin Source File

SOURCE=.\SRCEdit.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\store.h
# End Source File
# Begin Source File

SOURCE=.\StoreEdit.h
# End Source File
# Begin Source File

SOURCE=.\StoreProperties.h
# End Source File
# Begin Source File

SOURCE=.\StrRefDlg.h
# End Source File
# Begin Source File

SOURCE=.\structs.h
# End Source File
# Begin Source File

SOURCE=.\subband.h
# End Source File
# Begin Source File

SOURCE=.\tbg.h
# End Source File
# Begin Source File

SOURCE=.\TextView.h
# End Source File
# Begin Source File

SOURCE=.\TisDialog.h
# End Source File
# Begin Source File

SOURCE=.\tispack.h
# End Source File
# Begin Source File

SOURCE=.\tlkhandler.h
# End Source File
# Begin Source File

SOURCE=.\TormentCre.h
# End Source File
# Begin Source File

SOURCE=.\unpacker.h
# End Source File
# Begin Source File

SOURCE=.\utils.h
# End Source File
# Begin Source File

SOURCE=.\variables.h
# End Source File
# Begin Source File

SOURCE=.\vorbisfile.h
# End Source File
# Begin Source File

SOURCE=.\VVC.h
# End Source File
# Begin Source File

SOURCE=.\VVCEdit.h
# End Source File
# Begin Source File

SOURCE=.\WedEdit.h
# End Source File
# Begin Source File

SOURCE=.\WedPolygon.h
# End Source File
# Begin Source File

SOURCE=.\WedTile.h
# End Source File
# Begin Source File

SOURCE=.\WeiduLog.h
# End Source File
# Begin Source File

SOURCE=.\WFX.h
# End Source File
# Begin Source File

SOURCE=.\WFXEdit.h
# End Source File
# Begin Source File

SOURCE=.\zconf.h
# End Source File
# Begin Source File

SOURCE=.\zlib.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\chitem.ico
# End Source File
# Begin Source File

SOURCE=.\res\chitem.rc2
# End Source File
# Begin Source File

SOURCE=.\res\getfiles.bmp
# End Source File
# End Group
# Begin Group "Documents"

# PROP Default_Filter ".txt"
# Begin Source File

SOURCE=.\areaformat.txt
# End Source File
# Begin Source File

SOURCE=.\bamformat.txt
# End Source File
# Begin Source File

SOURCE=.\biff.txt
# End Source File
# Begin Source File

SOURCE=.\bmpformat.txt
# End Source File
# Begin Source File

SOURCE=.\chrformat.txt
# End Source File
# Begin Source File

SOURCE=.\chuformat.txt
# End Source File
# Begin Source File

SOURCE=.\creformat.txt
# End Source File
# Begin Source File

SOURCE=.\Dialog.txt
# End Source File
# Begin Source File

SOURCE=.\dlgformat.txt
# End Source File
# Begin Source File

SOURCE=.\effects.txt
# End Source File
# Begin Source File

SOURCE=.\effformat.txt
# End Source File
# Begin Source File

SOURCE=.\gamformat.txt
# End Source File
# Begin Source File

SOURCE=.\gradient.txt
# End Source File
# Begin Source File

SOURCE=.\iapformat.txt
# End Source File
# Begin Source File

SOURCE=.\itmformat.txt
# End Source File
# Begin Source File

SOURCE=.\Key.txt
# End Source File
# Begin Source File

SOURCE=.\mosformat.txt
# End Source File
# Begin Source File

SOURCE=.\musformat.txt
# End Source File
# Begin Source File

SOURCE=.\pltformat.txt
# End Source File
# Begin Source File

SOURCE=.\proformat.txt
# End Source File
# Begin Source File

SOURCE=.\restype.txt
# End Source File
# Begin Source File

SOURCE=.\splformat.txt
# End Source File
# Begin Source File

SOURCE=.\stoformat.txt
# End Source File
# Begin Source File

SOURCE=.\tbgformat.txt
# End Source File
# Begin Source File

SOURCE=.\tisformat.txt
# End Source File
# Begin Source File

SOURCE=.\varformat.txt
# End Source File
# Begin Source File

SOURCE=.\vvcformat.txt
# End Source File
# Begin Source File

SOURCE=.\wedformat.txt
# End Source File
# Begin Source File

SOURCE=.\wmapformat.txt
# End Source File
# End Group
# End Target
# End Project

// Visit http://forum.gamedeception.net/
#include <ida.idc>
#include "Util_WoWVersion.idc"

static ExtractPath( sPath )
{
	auto dwIndex;
	for( dwIndex = strlen( sPath ); strstr( substr( sPath, dwIndex, -1 ), "\\" ); dwIndex-- );
	return substr( sPath, 0, dwIndex + 1 );
}

static DumpField( szName, szPrefix, dwPointer, hFile )
{
	auto i, dwEnumId;

	fprintf( hFile, "// Descriptors: 0x%08X\nenum %s\n{\n", dwPointer, szName );
	i = 0;

	dwEnumId = AddEnum( -1, szName, 0x1100000 );

	while( 1 )
	{
		auto pName, pPrevName;

		pName = GetString( Dword( dwPointer ), -1, ASCSTR_C );
		pPrevName = GetString( Dword( dwPointer - 0x14 ), -1, ASCSTR_C );

		if( pName == "" )
			break;

		if( ( strstr( szPrefix, "GAMEOBJECT" ) != -1 && strstr( pName, "OBJECT_FIELD" ) != -1 ) || strstr( pName, szPrefix ) != -1 )
		{
			auto dwIndex;

			dwIndex = Dword( dwPointer + 4 );

			AddConstEx( dwEnumId, pName, dwIndex * 4, -1 );
			fprintf( hFile, "\t%s = 0x%X,\n", pName, dwIndex * 4 );
		}
		else
			break;

		dwPointer = dwPointer + 0x14;
		i++;
	}

	fprintf( hFile, "\tTOTAL_%s_FIELDS = 0x%X\n", szPrefix, i );
	fprintf( hFile, "};\n" );
}

static main()
{
	auto sPath, hFile, dwStartFunc, dwFuncPtrCheck, s_objectDescriptors, s_unitDescriptors, s_itemDescriptors, s_playerDescriptors, s_containerDescriptors, s_gameobjectDescriptors, s_dynamicobjectDescriptors, s_corpseDescriptors;

	sPath = ExtractPath( GetIdbPath() ) + "Objects_Enum.h";
	hFile = fopen( sPath, "w" );
	if( hFile != -1 )
	{
		fprintf( hFile, "#ifndef __OBJECTS_ENUM_H__\n#define __OBJECTS_ENUM_H__\n" );
		fprintf( hFile, "// %s\n", GetWoWVersionString() );
		fprintf( hFile, "/*----------------------------------\n" );
		fprintf( hFile, "WoW Offset Dumper 0.2 - IDC Script\n" );
		fprintf( hFile, "by kynox\n\n" );
		fprintf( hFile, "Credits:\n" );
		fprintf( hFile, "bobbysing, Patrick, Dominik, Azorbix, Tanaris4\n" );
		fprintf( hFile, "-----------------------------------*/\n\n" );

		//dwStartFunc = FindBinary( INF_BASEADDR, SEARCH_DOWN, "56 57 68 ? ? ? ? B8 05" );
		dwStartFunc = FindBinary( INF_BASEADDR, SEARCH_DOWN, "55 89 E5 57 31 FF 56 53 81 EC DC 00 00 00" );

		Message( "dwStartFunc: 0x%08X\n", dwStartFunc );
	
		// updated for 4.0.3 beta
		s_objectDescriptors = Dword( dwStartFunc + 0x36 );
		s_itemDescriptors = Dword( dwStartFunc + 0x135 );
		s_containerDescriptors = Dword( dwStartFunc + 0x23D );
		s_unitDescriptors = Dword( dwStartFunc + 0x338 );
		s_playerDescriptors = Dword( dwStartFunc + 0x44E );
		s_gameobjectDescriptors	= Dword( dwStartFunc + 0x550 );
		s_dynamicobjectDescriptors = Dword( dwStartFunc + 0x64B );
		s_corpseDescriptors = Dword( dwStartFunc + 0x746 );

		MakeName( s_objectDescriptors, "s_objectDescriptors" );
		MakeName( s_unitDescriptors, "s_unitDescriptors" );
		MakeName( s_itemDescriptors, "s_itemDescriptors" );
		MakeName( s_playerDescriptors, "s_playerDescriptors" );
		MakeName( s_containerDescriptors, "s_containerDescriptors" );
		MakeName( s_gameobjectDescriptors, "s_gameobjectDescriptors" );
		MakeName( s_dynamicobjectDescriptors, "s_dynamicobjectDescriptors" );
		MakeName( s_corpseDescriptors, "s_corpseDescriptors" );

		DumpField( "eObjectFields", "OBJECT", s_objectDescriptors, hFile );
		DumpField( "eItemFields", "ITEM", s_itemDescriptors, hFile );
		DumpField( "eContainerFields", "CONTAINER", s_containerDescriptors, hFile );
		DumpField( "eUnitFields", "UNIT", s_unitDescriptors, hFile );
		DumpField( "ePlayerFields", "PLAYER", s_playerDescriptors, hFile );
		DumpField( "eGameObjectFields",	 "GAMEOBJECT", s_gameobjectDescriptors, hFile );
		DumpField( "eDynamicObjectFields", "DYNAMICOBJECT", s_dynamicobjectDescriptors, hFile );
		DumpField( "eCorpseFields", "CORPSE", s_corpseDescriptors, hFile );

		fprintf( hFile, "#endif //__OBJECTS_ENUM_H__" );
		fclose( hFile );
	}
	else
		Message( "Failed to open file %s.\n", sPath );

	Message( "Successfully dumped %s.\n", sPath );
}
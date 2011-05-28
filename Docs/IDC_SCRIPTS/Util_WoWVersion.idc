#ifndef __WOWVERSION_IDC__
#define __WOWVERSION_IDC__

#include <idc.idc>

static GetWoWVersionString()
{
	auto sVersion, sBuild, sDate;
														
	sVersion = FindBinary( INF_BASEADDR, SEARCH_DOWN, "\"=> WoW Version %s (%s) %s\"" );

	if( sVersion == BADADDR )
	{
		Message( "Version format string not found" );
		return 0;
	}

	sVersion = DfirstB( sVersion );

	if( sVersion == BADADDR )
	{
		Message( "Version string unreferences" );
		return 0;
	}
	
	sVersion = PrevHead( sVersion, 0 );
	sBuild = PrevHead( sVersion, 0 );
	sDate = PrevHead( sBuild, 0 );

	sVersion = GetOperandValue( sVersion, 0 );
	sBuild = GetOperandValue( sBuild, 0 );
	sDate = GetOperandValue( sDate, 0 );

	sVersion = GetString( sVersion, -1, ASCSTR_C );
	sBuild = GetString( sBuild, -1, ASCSTR_C );
	sDate = GetString( sDate, -1, ASCSTR_C );

	return form( "Version: %s  Build number: %s  Build date: %s\n", sVersion, sBuild, sDate );
}

#endif // __WOWVERSION_IDC__
//
//  LuaController.m
//  Pocket Gnome
//
//  Created by Josh on 10/14/10.
//  Copyright 2010 Savory Software, LLC. All rights reserved.
//

#import "LuaController.h"
#import "lua.h"
#import "lauxlib.h"
#import "Plugin.h"
#import "Controller.h"

#include "luaconf.h"

#import "wax_http.h"
#import "wax_json.h"

@interface LuaController (Internal)
- (void)addPath:(NSString*)newPath;
@end

@implementation LuaController

static LuaController *sharedController = nil;

+ (LuaController*)sharedController{
	if (sharedController == nil)
		sharedController = [[[self class] alloc] init];
	return sharedController;
}

- (id)init{
    self = [super init];
	if ( sharedController ){
		[self release];
		self = sharedController;
	}
    else if (self != nil) {

		// lets figure out when the app is done launching!
		[[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(applicationDidFinishLaunching:) name: ApplicationLoadedNotification object: nil];
		
		
		
		// fire up wax!
		wax_startWithExtensions(luaopen_wax_http, luaopen_wax_json, nil);		
		
	}
	
	return self;
}

- (void)dealloc{

	// close lua state!
	lua_State *L = wax_currentLuaState();
	lua_close(L);
	
	[super dealloc];
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	//hello
}

- (Plugin *)loadPluginAtPath:(NSString*)path {
	//ge the wax lua state
	lua_State *L = wax_currentLuaState();
	
	if ( L ) {
		
		NSDictionary *pluginDict = [NSDictionary dictionaryWithContentsOfFile:[NSString stringWithFormat:@"%@/Info.plist", path]];
		if(pluginDict == nil)
			return nil;
		
		NSError *error = nil;
		NSArray *pluginFiles =	[[NSFileManager defaultManager] contentsOfDirectoryAtPath:path error:&error];
		if(error != nil)
			return nil;
				
		for ( NSString *file in pluginFiles) {
			
			NSArray *split = [file componentsSeparatedByString:@"."];
			if ( [[split lastObject] isEqualToString:@"lua"] ){
				
				NSString *fullPathToFile = [NSString stringWithFormat:@"%@/%@", path, file];
				
				if (luaL_dofile(L, [fullPathToFile UTF8String]) != 0) {
					PGLog(@"[LUA] Error loading lua file '%@' Error: %s", fullPathToFile, lua_tostring(L,-1));
										
					return nil;	// we could continue here, but I'd rather throw an error + not continue to try to load them
				}
			}				
		}
		
		
		if ( [pluginDict valueForKey:@"Main Class"] == nil ) {
			PGLog(@"plugin main class not defined in Info.plist");
			return nil;
		}
		
		Class pluginClass = NSClassFromString([pluginDict valueForKey:@"Main Class"]);
		Plugin *plugin = [[pluginClass alloc] initWithPath:path];
		
		if ( ![plugin isKindOfClass:[Plugin class]] ) {
			PGLog(@"plugin main class %@ does not inherit from the Plugin class at path: %@", [pluginDict valueForKey:@"Main Class"], path);
			return nil;
		}
		return plugin;	
	}
	
	
	return nil;
}

- (BOOL)unloadPlugin:(Plugin*)plugin{
	
	return NO;
}

@end

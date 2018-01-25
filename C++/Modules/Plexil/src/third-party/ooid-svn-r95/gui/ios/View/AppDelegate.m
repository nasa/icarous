//
//  AppDelegate.m
//  uuid
//
//  Created by Kenneth Laskoski on 3/13/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (BOOL)checkNamespace
{
    /* declare context */
    static UIAlertView *alert;
    static NSRegularExpression *regex;
    static NSString *regexPattern = @"^(([0-9a-fA-F]){8}-([0-9a-fA-F]){4}-([0-9a-fA-F]){4}-([0-9a-fA-F]){4}-([0-9a-fA-F]){12})$";

    /* setup context on first run */
    if (regex == nil)
        regex = [NSRegularExpression
                 regularExpressionWithPattern:regexPattern
                 options:0
                 error:nil];
    if (alert == nil)
        alert = [[UIAlertView alloc]
                 initWithTitle:nil
                 message:@"Namespace must be a valid UUID"
                 delegate:nil
                 cancelButtonTitle:@"OK"
                 otherButtonTitles:nil];

    /* do the check */
    NSString *nameSpace = [[NSUserDefaults standardUserDefaults] stringForKey:@"namespace_key"];
    if ([regex numberOfMatchesInString:nameSpace options:0 range:NSMakeRange(0, [nameSpace length])] != 1)
    {
        [alert show];
        return NO;
    }

    /* if it ain't broke, don't fix it */
    [alert dismissWithClickedButtonIndex:0 animated:YES];
    
    return YES;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // register defaults
    NSDictionary *appDefaults = 
    [NSDictionary dictionaryWithObject:
     @"00000000-0000-0000-0000-000000000000" forKey:@"namespace_key"];
    [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
    
    // listen for changes to our preferences when the Settings app does so,
    // when we are resumed from the backround, this will give us a chance to update our UI
    [[NSNotificationCenter defaultCenter]
     addObserver:self
     selector:@selector(checkNamespace)
     name:NSUserDefaultsDidChangeNotification
     object:nil];
    
    return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

@end

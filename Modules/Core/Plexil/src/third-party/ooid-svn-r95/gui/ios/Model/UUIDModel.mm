//
//  UUIDModel.mm
//  uuid
//
//  Created by Kenneth Laskoski on 01/03/13.
//
//

#import "UUIDModel.h"
#import "AppDelegate.h"

#include "kashmir/uuidgen.h"
#include "kashmir/system/ccmd5.h"
#include "kashmir/system/ccsha1.h"
#include "kashmir/system/iosrandom.h"

#include <sstream>

@implementation UUIDModel
{    
    std::stringstream buffer;
    UITextView *outputView;
}

- (id)initWithTextView:(UITextView *)textView
{
    self = [super init];
    if (self)
    {
        outputView = textView;
        if (outputView == nil)
            return nil;
    }
    
    return self;
}

- (void)generateNIL
{
    static const kashmir::uuid_t null;
    
    /* static memory should be zeroed */
    /* so we don't need the following */
    // assert(!null);
    
    buffer << null << '\n';
    [self synch];
}

- (void)generateNS
{
    NSString *nameSpace = [[NSUserDefaults standardUserDefaults] stringForKey:@"namespace_key"];
    buffer << [nameSpace UTF8String] << '\n';
    [self synch];
}

- (void)generateV3:(NSString *)name
{
    [outputView resignFirstResponder];

    // Check namespace
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    if (![appDelegate checkNamespace])
        return;
    
    // Get namespace from user settings
    const kashmir::uuid_t nameSpace([[[NSUserDefaults standardUserDefaults]
                                      stringForKey:@"namespace_key"] UTF8String]);
    
    kashmir::system::ccmd5 md5engine;
    const kashmir::uuid_t uuid = kashmir::uuid::uuid_gen(md5engine, nameSpace, [name UTF8String]);
    
    buffer << uuid << '\n';
    [self synch];
}

- (void)generateV4:(int)n
{
    static kashmir::system::iOSRandom randstream;
    static kashmir::uuid_t uuid;
    static UIAlertView *alert;
    
    if (alert == nil)
        alert = [[UIAlertView alloc]
                 initWithTitle:@"Sorry!"
                 message:@"There's a limit of 1024 UUIDs per generation cycle"
                 delegate:nil
                 cancelButtonTitle:@"OK"
                 otherButtonTitles:nil];
    
    if (n > 1024)
    {
        [alert show];
        return;
    }
    
    for (int i = 0; i < n; ++i)
    {
        randstream >> uuid;
        buffer << uuid << '\n';
    }

    [self synch];
}

- (void)generateV5:(NSString *)name
{
    [outputView resignFirstResponder];

    // Check namespace
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    if (![appDelegate checkNamespace])
        return;
    
    // Get namespace from user settings
    const kashmir::uuid_t nameSpace([[[NSUserDefaults standardUserDefaults]
                                      stringForKey:@"namespace_key"] UTF8String]);
    
    kashmir::system::ccsha1 sha1engine;
    const kashmir::uuid_t uuid = kashmir::uuid::uuid_gen(sha1engine, nameSpace, [name UTF8String]);
    
    buffer << uuid << '\n';
    [self synch];
}

- (void)clearContent
{
    buffer.str("");
    buffer.clear();
    [self synch];
}

- (void)synch
{
    outputView.text = [NSString stringWithUTF8String:buffer.str().c_str()];
}

@end

//
//  UUIDModel.h
//  uuid
//
//  Created by Kenneth Laskoski on 01/03/13.
//
//

#import <Foundation/Foundation.h>

@interface UUIDModel : NSObject

- (id)initWithTextView:(UITextView *)textView;
- (void)generateNIL;
- (void)generateNS;
- (void)generateV3:(NSString *)name;
- (void)generateV4:(int)n;
- (void)generateV5:(NSString *)name;
- (void)clearContent;

@end

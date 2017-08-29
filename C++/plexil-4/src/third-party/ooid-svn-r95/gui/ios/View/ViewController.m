//
//  ViewController.m
//  uuid
//
//  Created by Kenneth Laskoski on 3/13/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ViewController.h"
#import "UUIDModel.h"

@interface ViewController ()
    @property (weak, nonatomic) IBOutlet UITextField *nameTextField;
    @property (weak, nonatomic) IBOutlet UITextField *nTextField;
    @property (weak, nonatomic) IBOutlet UITextView *textView;

    - (IBAction)generateNIL:(id)sender;
    - (IBAction)generateNS:(id)sender;
    - (IBAction)generateV3:(id)sender;
    - (IBAction)generateV4:(id)sender;
    - (IBAction)generateV5:(id)sender;
    - (IBAction)clearContent:(id)sender;
@end

@implementation ViewController
{
    UUIDModel *_model;
}

- (UUIDModel *)model
{
    return _model;
}

- (void)setModel:(id)model
{
    _model = model;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    [self setModel:[[UUIDModel alloc] initWithTextView:[self textView]]];

    // this is necessary to get the shake event
    [self becomeFirstResponder];
}

- (void)viewDidUnload
{
    [self setModel:nil];   
    [self setTextView:nil];
    [self setNTextField:nil];
    [self setNameTextField:nil];
    
    [super viewDidUnload];
}

// this is necessary to get the shake event
- (BOOL)canBecomeFirstResponder
{
    return YES;
}

// get the shake event
- (void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event
{
    if (motion == UIEventSubtypeMotionShake)
        [self generateV4:nil];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [self.nameTextField resignFirstResponder];
    return YES;
}

- (IBAction)generateNIL:(id)sender
{
    [self.model generateNIL];
}

- (IBAction)generateNS:(id)sender
{
    [self.model generateNS];
}

- (IBAction)generateV3:(id)sender
{
    [self.model generateV3:[self nameTextField].text];
}

- (IBAction)generateV4:(id)sender
{
    [self.model generateV4:[[self nTextField].text intValue]];
}

- (IBAction)generateV5:(id)sender
{
    [self.model generateV5:[self nameTextField].text];
}

- (IBAction)clearContent:(id)sender
{
    [self.model clearContent];
}

@end

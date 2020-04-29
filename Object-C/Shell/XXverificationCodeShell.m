//
//  XXverificationCodeShell.m
//  XXkitProject
//
//  Created by VINSON on 2020/4/29.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXverificationCodeShell.h"

@implementation XXverificationCodeShell
- (void)shellAccount:(UITextField*)account code:(UITextField*)code request:(UIButton*)request{
    _accountShell = [XXtextFieldShell new];
    [_accountShell shell:account];
    
    _codeShell = [XXtextFieldShell new];
    [_codeShell shell:code];
    
    _requestShell = [XXlimitButtonShell new];
    [_requestShell shell:request];
    [request addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}

- (void)onButtonTouchUpInside:(UIButton*)sender{
    if(_onRequest){
        self.onRequest(self, self.accountShell.target.text);
    }
}
@end

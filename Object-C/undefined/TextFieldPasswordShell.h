//
//  PassswordTextField.h
//  jsonApp
//
//  Created by VINSON on 2019/11/12.
//  Copyright © 2019 xxx. All rights reserved.
//

#import "TextFieldExpressShell.h"

NS_ASSUME_NONNULL_BEGIN

@interface TextFieldPasswordShell : TextFieldExpressShell
@property (nonatomic,strong) UIImage *secureONImage;
@property (nonatomic,strong) UIImage *secureOFFImage;
@property (nonatomic,strong) UIImage *clearImage;
@property (nonatomic,assign) BOOL secureON;
@end

NS_ASSUME_NONNULL_END

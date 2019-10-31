//
//  CellProtocol.m
//  XXkitDemo
//
//  Created by VINSON on 2019/10/22.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import "XXheaderCellFooterBase.h"
#define kSigDataChanged @"sigDataChanged"

@implementation XXheaderCellFooterBase
-(void)setJData:(nullable NSDictionary*)jdata{
    
}
-(nullable NSDictionary*)getJData{
    return nil;
}
-(void)postNotificationName:(NSString*)name{
    NSMutableDictionary *userInfo = [[NSMutableDictionary alloc] init];
    userInfo[@"IndexPath"] = self.indexPath;
    NSDictionary *cellData = [self getJData];
    if(nil != cellData){
        userInfo[@"JData"] = cellData;
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:name object:nil userInfo:userInfo];
}
- (void)emitSigDataChanged{
    [self postNotificationName:kSigDataChanged];
}
@end

//
//  XXlogger.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXlogger.h"
#import "../../C/XXtcpSocket.h"

static XXlogger * _instance = nil;

@interface XXlogger()
@property (nonatomic,copy) NSString *serverIP;
@property (nonatomic,assign) NSInteger serverPort;

@property (nonatomic,copy) NSString *localPathAbs;
@property (nonatomic,)
@end

@implementation XXlogger
+(XXlogger*)sharedInstance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [XXlogger new];
    });
    return _instance;
}
+(void)normal:(NSString*)msg{
    [[XXlogger sharedInstance] normal:msg];
}
+(void)raw:(NSString*)msg{
    [[XXlogger sharedInstance] raw:msg];
}
+(void)enableServerIP:(NSString*)ip serverPort:(NSInteger)port{
    [[XXlogger sharedInstance] enableServerIP:ip serverPort:port];
}
+(void)enableLocalPathNodes:(NSArray<NSString*>*)nodes{
    [[XXlogger sharedInstance] enableLocalPathNodes:nodes];
}

-(void)normal:(NSString*)msg{
    
}
-(void)raw:(NSString*)msg{
    
}
-(void)enableServerIP:(NSString*)serverIP serverPort:(NSInteger)port{
    
}
-(void)enableLocalPathNodes:(NSArray<NSString*>*)nodes{
    NSString *home = NSHomeDirectory();
    NSString *path = [home stringsByAppendingPaths:nodes];
}
@end

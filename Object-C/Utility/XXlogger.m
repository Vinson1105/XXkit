//
//  XXlogger.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXlogger.h"
#import "../../Object-C/XXocUtils.h"

static XXlogger * _instance = nil;

@interface XXlogger()
@property (nonatomic,strong) NSMutableDictionary *nameToFifo;
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
+(void)configFifoClass:(Class)cls param:(NSDictionary*)param forName:(NSString *)name{
    [[XXlogger sharedInstance] configFifoClass:cls param:param forName:name];
}
+(void)setFifoEnable:(BOOL)enable forName:(NSString*)name{
    [[XXlogger sharedInstance] setFifoEnable:enable forName:name];
}

-(void)normal:(NSString*)msg{
    NSString *normalMsg = [NSString stringWithFormat:@"[%@] %@", [XXocUtils currentDateString], msg];
    for (XXfifoBase *fifo in self.nameToFifo.allValues) {
        if(fifo.enable){
            [fifo push:[normalMsg dataUsingEncoding:NSUTF8StringEncoding]];
        }
    }
}
-(void)configFifoClass:(Class)cls param:(NSDictionary*)param forName:(NSString *)name{
    XXfifoBase *fifo = self.nameToFifo[name];
    if(fifo){
        
    }
    else{
        fifo = [[cls alloc] initWithParam:param];
        self.nameToFifo[name] = fifo;
        fifo.enable = YES;
    }
}
-(void)setFifoEnable:(BOOL)enable forName:(NSString*)name{
    XXfifoBase *fifo = self.nameToFifo[name];
    if(nil == fifo){
        return;;
    }
    fifo.enable = enable;
}
-(void)pushToEachEnabledFifo:(NSData*)data{
    for (XXfifoBase *fifo in self.nameToFifo.allValues) {
        if(fifo.enable){
            [fifo push:data];
        }
    }
}
@end

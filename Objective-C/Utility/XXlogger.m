//
//  XXlogger.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXlogger.h"
#import "../../Objective-C/XXocUtils.h"
#import "DailyLogFileFifo.h"

@implementation XXfifoBase
- (instancetype)initWithParam:(NSDictionary *)param{return [super init];}
- (void)push:(NSData *)data{}
@end

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
+(void)message:(NSString*)format,...{
    va_list args;
    if (format) {
        va_start(args, format);
        NSString *message = [[NSString alloc] initWithFormat:format arguments:args];
        va_end(args);
        [[XXlogger sharedInstance] message:message];
    }
}
+(void)configFifoClassString:(NSString*)clsString param:(NSDictionary*)param forName:(NSString *)name{
    [[XXlogger sharedInstance] configFifoClassString:clsString param:param forName:name];
}
+(void)setFifoEnable:(BOOL)enable forName:(NSString*)name{
    [[XXlogger sharedInstance] setFifoEnable:enable forName:name];
}

- (instancetype)init{
    self = [super init];
    if (self) {
        [self configFifoClassString:@"DailyLogFileFifo" param:@{@"SavePath":@"XXlogger"} forName:@"XXlogger"];
    }
    return self;
}
-(void)message:(NSString*)msg{
    NSString *normalMsg = [NSString stringWithFormat:@"[%@] %@\n", [XXocUtils currentDateString], msg];
    for (XXfifoBase *fifo in self.nameToFifo.allValues) {
        if(fifo.enable){
            [fifo push:[normalMsg dataUsingEncoding:NSUTF8StringEncoding]];
        }
    }
}
-(void)configFifoClassString:(NSString*)clsString param:(NSDictionary*)param forName:(NSString *)name{
    Class cls = NSClassFromString(clsString);
    if(nil == cls){
        return;
    }
    
//    XXfifoBase *fifo = self.nameToFifo[name];
//    if(fifo){
//
//    }
//    else{
    XXfifoBase *fifo = [[cls alloc] initWithParam:param];
    self.nameToFifo[name] = fifo;
    fifo.enable = YES;
//    }
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
- (NSMutableDictionary *)nameToFifo{
    if(nil == _nameToFifo){
        _nameToFifo = [NSMutableDictionary dictionary];
    }
    return _nameToFifo;
}
@end

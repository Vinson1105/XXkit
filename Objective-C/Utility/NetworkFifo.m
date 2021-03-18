//
//  NetworkFifo.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/24.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "NetworkFifo.h"
#import "../../C/XXtcpSocket.h"
#import "../../C/XXudpSocket.h"
#import "../../Object-C/XXocUtils.h"

static NSString * const kHost = @"host";
static NSString * const kPort = @"port";
static NSString * const kWay = @"way";  // 可选：tcp/udp
CONST_STRING(kTcp, tcp)
CONST_STRING(kUdp, udp)

typedef enum : NSUInteger {
    NetworkWayUnknown = 0,
    NetworkWayTcp,
    NetworkWayUdp,
} NetworkWay;

@interface NetworkFifo(){
    XXtcpSocketHandle _tcpHandle;
    XXudpSocketHandle _udpHandle;
}
@property (nonatomic,copy) NSString *host;
@property (nonatomic,assign) int port;
@property (nonatomic,assign) NetworkWay way;
@property (nonatomic,strong) NSOperationQueue *operationQueue;
@end

@implementation NetworkFifo
- (instancetype)initWithParam:(NSDictionary *)param{
    self = [super init];
    if(self){
        _tcpHandle = NULL;
        _udpHandle = NULL;
        
        if(param[kHost]) self.host = param[kHost];
        if(param[kPort]) self.port = [param[kPort] intValue];
        if(param[kWay]){
            NSString *wayString = param[kWay];
            if([wayString isEqualToString:kTcp]){
                self.way = NetworkWayTcp;
            }
            else if([wayString isEqualToString:kUdp]){
                self.way = NetworkWayUdp;
            }
            else{
                
            }
        }
        self.operationQueue = [[NSOperationQueue alloc] init];
        self.operationQueue.maxConcurrentOperationCount = 1;
        
        if(self.host && self.port>0 && NetworkWayUnknown!=self.way){
            if(self.way == NetworkWayTcp){
                _tcpHandle = xxtcpSocket_alloc(128*1024, 10, 200);
                xxtcpSocket_connectToHost(_tcpHandle, [self.host cStringUsingEncoding:NSUTF8StringEncoding], self.port);
            }
            else{
                
            }
        }
    }
    return self;
}
- (void)dealloc{
    [self.operationQueue cancelAllOperations];
    if(NULL != _tcpHandle) xxtcpSocket_free(&_tcpHandle);
    if(NULL != _udpHandle) xxudpSocket_free(&_udpHandle);
}
- (void)push:(NSData *)data{
    XXOC_WS;
    [self.operationQueue addOperationWithBlock:^{
        XXOC_SS;
        if(ss.way == NetworkWayTcp){
            [ss sendUsingTcp:data];
        }
        else if(ss.way == NetworkWayUdp){
            [ss sendUsingUdp:data];
        }
        else{
            
        }
    }];
}
-(void)sendUsingTcp:(NSData*)data{
    if(NULL == _tcpHandle) return;
    xxtcpSocket_send(_tcpHandle, (uint8_t*)data.bytes, data.length);
}
-(void)sendUsingUdp:(NSData*)data{
    if(NULL == _udpHandle || nil==self.host || 0==self.port) return;
    xxudpSocket_send(_udpHandle,
                     [self.host cStringUsingEncoding:NSUTF8StringEncoding],
                     self.port,
                     (int8_t*)data.bytes,
                     data.length);
}
@end

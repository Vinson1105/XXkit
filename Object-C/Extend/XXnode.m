#import "XXnode.h"
#import "../XXocDefine.h"

@interface XXnode()

typedef NS_ENUM(NSUInteger,XXnodeParentType){
    XXnodeParentType_Root,      // 根节点
    XXnodeParentType_Dict,      // 字典节点
    XXnodeParentType_Array,     // 列表节点
};
@property (nonatomic,assign) XXnodeParentType parentType;   // 父节点类型
@property (nonatomic,weak,nullable) XXnode *parent;         // 父节点
@property (nonatomic,strong,nullable) id data;              // 本节点数据,可能是String/Int/Dict/Array...
@property (nonatomic,weak,nullable) id rootData;            // 根节点数据,根节点有效
@property (nonatomic,copy,nullable) id key;                 // 本节点数据对应的key
@end

@implementation XXnode
- (id) value{
    return self.data;
}
- (void)setValue:(id)value{
    XXnode *node        = self;
    BOOL kChangedValue  = NO;
    while (nil != node.parent && (nil != node.parent.data || nil != node.parent.rootData)) {
        // 父节点
        XXnode *parent  = node.parent;
        // 父节点数据
        id parentData   = nil != parent.rootData ? parent.rootData : parent.data;
        // 本节点或者其余子节点数据
        id realValue    = kChangedValue ? node.value : value;
        if(!kChangedValue)
            kChangedValue = YES;
        
        switch (node.parentType) {
            case XXnodeParentType_Dict:{
                [parentData setObject:realValue forKey:node.key];
                break;
            }
            case XXnodeParentType_Array:{
                NSUInteger nodeIndex = [node.key unsignedIntegerValue];
                if(nodeIndex == [parentData count])
                    [parentData addObject:realValue];
                else
                    [parentData replaceObjectAtIndex:nodeIndex withObject:realValue];    // 若index>=count,异常抛出
                break;
            }
            default:
                break;
        }
        
        node = parent;
    }
}
- (StringBlock)string{
    XXOC_WS;
    return ^NSString*(void){
        return (NSString*)ws.data;
    };
}
- (NSUintBlock)nsuint{
    XXOC_WS;
    return ^NSUInteger(void){
        return [ws.data unsignedIntegerValue];
    };
}
- (NSIntBlock)nsint{
    XXOC_WS;
    return ^NSInteger(void){
        return [ws.data integerValue];
    };
}

- (DictNodeBlock)dict{
    XXOC_WS;
    
    return ^XXnode*(id key){
        id nodeData = nil != ws.rootData ? ws.rootData : ws.data;
        if(nil == nodeData){
            ws.data     = [NSMutableDictionary dictionary];
            nodeData    = ws.data;
        }
        
        if([nodeData isKindOfClass: NSClassFromString(@"NSMutableDictionary")]){
            NSMutableDictionary *nodeDictData   = nodeData;
            id value                            = [nodeDictData objectForKey:key];
            return [[XXnode alloc] initWithParent:ws
                                       ParentType:XXnodeParentType_Dict
                                              Key:key
                                            Value:value];
        }
        else {
            return nil;
        }
    };
}
- (ArrayNodeBlock)array{
    XXOC_WS;
    
    return ^XXnode*(uint index){
        id nodeData = nil != ws.rootData ? ws.rootData : ws.data;
        if(nil == nodeData){
            ws.data = [NSMutableArray array];
            nodeData = ws.data;
        }
        
        if([nodeData isKindOfClass: NSClassFromString(@"NSMutableArray")]){
            NSMutableArray *nodeArrayData   = nodeData;
            id value                        = [nodeArrayData count] > index ? [nodeArrayData objectAtIndex:index] : nil;
            return [[XXnode alloc] initWithParent:ws
                                       ParentType:XXnodeParentType_Array
                                              Key:@(index)
                                            Value:value];
        }
        else {
            return nil;
        }
    };
}

- (instancetype) initWithParent:(XXnode*)parent ParentType:(XXnodeParentType)parentType Key:(id)key Value:(id)value{
    self = [self init];
    if (self) {
        self.parent = parent;
        self.parentType = parentType;
        self.key = key;
        self.data = value;
    }
    return self;
}
- (void) resetWithParent:(XXnode*)parent ParentType:(XXnodeParentType)parentType Key:(id)key Value:(id)value{
    self.parent = parent;
    self.parentType = parentType;
    self.key = key;
    self.data = value;
}
- (instancetype) initWithData:(id)root{
    self = [self init];
    if (self) {
        self.parentType = XXnodeParentType_Root;
        self.rootData = root;
    }
    return self;
}
@end

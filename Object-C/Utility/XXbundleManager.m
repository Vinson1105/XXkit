//
//  XXbundleManager.m
//  XXkitDemo
//
//  Created by VINSON on 2019/10/30.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import "XXbundleManager.h"

@interface XXbundleManager()
@property (nonatomic,strong) NSMutableDictionary *nameToBundle;
@end

@implementation XXbundleManager
+(XXbundleManager*)sharedInstance{
    static XXbundleManager *manager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        manager = [[XXbundleManager alloc] init];
    });
    return manager;
}
+(NSBundle *)bundleWithName:(NSString *)name{
    return [[XXbundleManager sharedInstance] bundleWithName:name type:nil];
}
+(NSBundle *)bundleWithName:(NSString *)name type:(NSString *)type{
    return [[XXbundleManager sharedInstance] bundleWithName:name type:type];
}
+(nullable UIImage*) image:(NSString*)imageName inBundle:(NSString*)bundleName{
    NSBundle *bundle = nil == bundleName ? [NSBundle mainBundle] : [XXbundleManager bundleWithName:bundleName];
    if(nil == bundle){
        return nil;
    }
    return [UIImage imageNamed:imageName inBundle:bundle compatibleWithTraitCollection:nil];
}
+(UINib*)nib:(NSString *)nibName inBundle:(NSString *)bundleName{
    NSBundle *bundle = nil == bundleName ? [NSBundle mainBundle] : [XXbundleManager bundleWithName:bundleName];
    if(nil == bundle){
        return nil;
    }
    return [UINib nibWithNibName:nibName bundle:bundle];
}
+(UIStoryboard*) storyboard:(NSString*)storyboardName inBundle:(NSString*)bundleName{
    NSBundle *bundle = nil == bundleName ? [NSBundle mainBundle] : [XXbundleManager bundleWithName:bundleName];
    if(nil == bundle){
        return nil;
    }
    return [UIStoryboard storyboardWithName:storyboardName bundle:bundle];
}

-(NSBundle*)bundleWithName:(NSString*_Nullable)name type:(NSString*)type{
    if(nil == _nameToBundle[name]){
        NSString *path = [[NSBundle mainBundle] pathForResource:name ofType:nil==type?@"bundle":type];
        if(nil == path){
            return nil;
        }
        NSBundle *bundle = [NSBundle bundleWithPath:path];
        if(nil != bundle){
            _nameToBundle[name] = bundle;
        }
        return bundle;
    }
    return _nameToBundle[name];
}
@end

//
//  ModelClass.m
//  BlockTest
//
//  Created by long on 2021/1/19.
//

#import "ModelClass.h"

@implementation ModelClass

- (void)requestCompletion:(void(^)(BOOL isSuccess))requestBlcok{
    
}

- (NSString *)hobby{
    if (_hobby == nil) {
        _hobby = @"ModelClass hobby";
    }
    return _hobby;
    
}

@end

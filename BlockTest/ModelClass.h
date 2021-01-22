//
//  ModelClass.h
//  BlockTest
//
//  Created by long on 2021/1/19.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface ModelClass : NSObject

@property (nonatomic ,strong) NSString *hobby;

@property (nonatomic ,copy) void (^requestBlcok)(BOOL isSuccess);

- (void)requestCompletion:(void(^)(BOOL isSuccess))requestBlcok;

@end

NS_ASSUME_NONNULL_END

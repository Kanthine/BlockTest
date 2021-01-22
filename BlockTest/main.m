//
//  main.m
//  BlockTest
//
//  Created by long on 2021/1/19.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
#import "ModelClass.h"

int main(int argc, char * argv[]) {
    NSString * appDelegateClassName;
    @autoreleasepool {
        // Setup code that might create autoreleased objects goes here.
        
        ModelClass *model = [[ModelClass alloc] init];
        __weak typeof(model) weakModel = model;
        model.requestBlcok = ^(BOOL isSuccess) {
            __strong typeof(weakModel) strongModel = weakModel;
            NSLog(@"strongModel ==== %@",strongModel);
        };
        model.requestBlcok(YES);
        
        appDelegateClassName = NSStringFromClass([AppDelegate class]);
    }
    return UIApplicationMain(argc, argv, nil, appDelegateClassName);
}

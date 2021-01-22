//
//  ViewController.m
//  BlockTest
//
//  Created by long on 2021/1/19.
//

#import "ViewController.h"
#import <Masonry/Masonry.h>
#import "ModelClass.h"

@interface ViewController ()

@property (nonatomic ,strong) UIButton *aButton;
@property (nonatomic ,strong) ModelClass *model;
@property (nonatomic ,strong) NSString *string;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.view.backgroundColor = UIColor.whiteColor;
    [self.view addSubview:self.aButton];
    self.string = @"Hello Word";
    [self.aButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.equalTo(self.view.mas_top).with.offset(100);
        make.left.equalTo(self.view.mas_left).with.offset(30);
        make.width.height.mas_equalTo(50);
    }];
}

- (void)aButtonClick:(UIButton *)button{
        
    __weak typeof(self) weakSelf = self;
    ModelClass *model = [[ModelClass alloc] init];
    __weak typeof(model) weakModel = model;
    model.requestBlcok = ^(BOOL isSuccess) {
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            NSLog(@"string === %@",weakSelf.string);
            NSLog(@"weakModel ==== %@",weakModel);
        });
    };
    model.requestBlcok(YES);

//    NSLog(@"requestBlcok ==== %@",self.model.requestBlcok);
}

#pragma mark - setter and getters

- (ModelClass *)model{
    if (_model == nil) {
        _model = [[ModelClass alloc] init];
    }
    return _model;
}

- (UIButton *)aButton{
    if (_aButton == nil) {
        UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
        button.backgroundColor = [UIColor.grayColor colorWithAlphaComponent:0.5];
        [button addTarget:self action:@selector(aButtonClick:) forControlEvents:UIControlEventTouchUpInside];
        _aButton = button;
    }
    return _aButton;
}

@end

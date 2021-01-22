![Blocks知识点大纲](https://upload-images.jianshu.io/upload_images/7112462-d8dd1d5390bf9270.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


> Blocks 是 C 语言的扩充：持有自动变量的匿名函数。

_特别说明：本文代码如无说明，均在 ARC 下编译；在 MRC 下编译会做注释_。


#### 1、`Blocks` 语法

##### 1.1、`Blocks` 变量声明

`Blocks` 类型由返回值类型和参数类型列表构成。使用脱字符 `^` 声明 `Blocks` 类型的变量。

![声明Blocks类型的语法](https://upload-images.jianshu.io/upload_images/7112462-a55b708ca5bbd3d1.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

* 返回值类型：当 `Blocks` 被调用时返回值的类型；如 `int` 型。无返回值时设置为`void`。
* `Blocks` 变量名：带有脱字符 `^` 前缀并且封装在括号中
* 参数类型列表：被封装在括号中并且位于块名称的后面，其中的参数类型以逗号分隔。如果没有参数，将参数列表设置为 `void`。

声明的一些简单`Blocks` 变量：

```
//变量 sumBlock 拥有两个 int 类型参数，返回值也是 int 类型
int (^sumBlock)(int,int);

//变量 logBlock 拥有一个 NSString 类型参数，无返回值 void
void (^logBlock)(NSString *);

//变量 simpleBlock 没有参数 void，无返回值void
void (^simpleBlock)(void);
```

既然可以将变量设为`Blocks` 类型，那么也可以将函数与方法的参数、类属性设为 `Blocks` (_能够定义变量的地方就能够定义`Blocks`_ )：

```
@interface ModelClass : NSObject

//requestBlcok 用作对象的属性，使用 copy 修饰，具有文件作用域，被分配在堆区
@property (nonatomic ,copy) void (^requestBlcok)(BOOL isSuccess);

//requestBlcok 用于方法的参数，具有块作用域，被分配在栈区
- (void)requestCompletion:(void(^)(BOOL isSuccess))requestBlcok;

@end
```

使用 `typedef` 为 `Blocks`提供别名，可以简化 `Blocks` 类型变量名称：

```
//为 block 类型提供别名 RequestBlcok
typedef void(^RequestBlcok)(BOOL isSuccess);

@interface ModelClass : NSObject

//别名 RequestBlcok 用作对象的属性类型，使用 copy 修饰，具有文件作用域，被分配在堆区
@property (nonatomic ,copy) RequestBlcok requestBlcok;

//别名 RequestBlcok 用于方法的参数类型，具有块作用域，被分配在栈区
- (void)requestCompletion:(RequestBlcok)completion;

@end
```

_疑问 1：为何使用 `copy` 修饰属性变量 `Blocks`，？能用`strong` 嘛？_
答：当`Blocks`不持有自动变量时，使用 `retain` 、 `copy`、`strong` 都会持有该变量；但当`Blocks`持有作用域之外的自动变量时，使用 `retain` 修饰，被持有的自动变量引用计数没有变化，可能随时释放，存在内存问题！
而使用 `copy` 、`strong` 修饰的 `Blocks`，会调用 `_Block_object_assign()` 函数将持有的自动变量引用计数加 1，保证当使用时不会被释放！


##### 1.2、`Blocks` 常量表达式

定义`Blocks`常量表达式：以脱字符`^`开头，后跟返回值类型、参数列表、`Blocks` 主体：

![块常量表达式的语法](https://upload-images.jianshu.io/upload_images/7112462-6aba23b5c25d0b59.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

* 脱字符 `^` 表明该表达式是一个常量表达式；
* 脱字符 `^` 后的返回值类型 _(可选项)_ 设置了该`Blocks` 表达式返回值的类型；
* `Blocks` 参数被封装在括号中传递给`Blocks` 主体，每个`Blocks` 参数都由一个参数类型和一个参数名表示，`Blocks` 参数之间以逗号分隔。

`Blocks` 常量表达式的简单定义：

```
//有返回类型的 Blocks 常量表达式
^int(int a ,int b){
    return a + b;
};

/** 没有设置返回类型的 Blocks 常量表达式
 * @note 返回值类型 int 可以不做定义，因为编译器会从 return 语句推断出返回值类型。
 * @note Blocks常量表达式被编译器存储在栈区
 */
^(int a,int b){
    return a + b;
};
```

`Blocks` 被用作方法参数，类属性时，定义`Blocks`常量表达式：

```
ModelClass *test = [[ModelClass alloc] init];
test.requestBlcok = ^(BOOL isSuccess) {
    printf("Block 主体，执行某些操作\n");
};

[test requestCompletion:^(BOOL isSuccess) {
    printf("Block 主体，执行某些操作\n");
}];
```

`Blocks` 的声明与定义组合为一条语句：

```
/* @note Blocks常量表达式被编译器存储在栈区
 * @note 通过赋值操作将其赋给Blocks变量，此时被复制到堆上或者全局静态区。
 */
void (^logBlock)(NSString *) = ^(NSString *log){
    NSLog(@"log:%@",log);
};

//在Blocks定义中，没有参数可不使用void
void (^simpleBlock)(void) = ^{
    
};
```

如果在函数体外面声明并定义 `Blocks` 变量，那么它具有文件作用域，由编译器静态分配，存储在全局静态区的数据区：

```
static BOOL(^isEvenNumberBlock1)(int number) = ^BOOL(int number){return (number % 2) == 0 ? YES : NO;};
```

关于 C 的内存，可以阅读 [C 内存管理](https://www.jianshu.com/p/53fc117ffd82) 一文。

##### `Blocks` 变量声明 和 `Blocks` 常量表达式的语法区别


语法元素|`Blocks`变量声明|`Blocks`常量表达式
-|-|-
脱字符 `^`|标识`Blocks`变量声明的开始，脱字符位于`Blocks`变量名称之前，两者都被封装在圆括号中|标识`Blocks`常量表达式的开始
名称|`Blocks`变量的名称是必选项|`Blocks` 常量表达式没有名称
返回值类型|必选项，没有返回值时声明为`void` |可选项，因为编译器能够从`Blocks` 表达式推断出返回值类型。
参数|必选项，没有参数时声明为`void` |参数列表是可选项

#### 2、`Blocks` 是一个闭包

> `Blocks` 是一个实现的闭包，一个允许访问其常规作用域之外变量的函数。


在 C 函数中声明的变量具有块作用域、无链接；这意味着这些自动变量仅在该函数访问。
与 C 函数相比，`Blocks` 扩大了变量的作用域，可以访问 `Blocks` 主体之外的自动变量。

##### 2.1、持有被访问的自动变量

`Blocks` 访问其作用域之外的变量

```
void func(void){
    NSString *log = @"打印日志";
    int a = 5;
    NSLog(@"log1:%@  : %p : %p",log,log,&log);
    NSLog(@"a  1:%d  : %p",a,&a);
    void (^simpleBlock)(void) = ^{//block 声明并赋值
        NSLog(@"log2:%@  : %p : %p",log,log,&log);
        NSLog(@"a  2:%d  : %p",a,&a);
    };
    log = @"hello word!";
    a = 6;
    NSLog(@"log3:%@  : %p : %p",log,log,&log);
    NSLog(@"a  3:%d  : %p",a,&a);
    simpleBlock();//调用 block 变量
}

/* 打印数据
log1:打印日志  : 0x1000021c0 : 0x7ffeefbff5c8
a  1:5  : 0x7ffeefbff5c4
log3:hello word!  : 0x100002260 : 0x7ffeefbff5c8
a  3:6  : 0x7ffeefbff5c4
log2:打印日志  : 0x1000021c0 : 0x100705250
a  2:5  : 0x100705258
 */
```

分析打印数据：`Blocks` 表达式中的变量与作用域之外的自动变量，内存地址相同，但不是同一个指针指向该内存；类似于 _`Blocks` 表达式对作用域之外的变量进行浅拷贝_。

`Blocks` 表达式对通过浅拷贝 __持有__ 作用域之外的自动变量。

_疑问 2：`Blocks`变量为何要持有作用域之外的自动变量？如何持有？_

##### 2.2、存储类别说明符 `__block`

默认情况下，`Blocks` 常量表达式对作用域之外的局部变量无权修改，否则编译器报错 `Variable is not assignable (missing __block type specifier)`。
存储类别说明符 `__block` 用于指定将变量值设置到哪个存储区域，可以将这些变量切换为读写模式。

```
__block NSString *log = @"打印日志";
void (^simpleBlock)(void) = ^{
    log = @"hello word!";
};
```

`__block` 说明符不能与存储类别说明符 `auto`、`register`、`static`组合使用。


##### 2.3、`Blocks`访问全局变量

在`Blocks`常量表达式中可以直接修改全局变量，无需使用 `__block` 说明符。

```
int a = 3;//声明并赋值一个全局变量 a

int main(int argc, const char * argv[]) {
    void (^simpleBlock)(void) = ^{
        printf("a 1==== %d \n",a);
        a = 4;
        printf("a 2==== %d \n",a);
    };
    a = 5;
    simpleBlock();
    return 0;
}

/* 打印数据
a 1==== 5
a 2==== 4
*/
```

在 C 语言中，下述类型变量允许在 `Blocks` 常量表达式中直接修改：
* 静态变量：使用 `static` 修饰的变量；
* 全局变量：具有内部链接的全局变量、具有外部链接的全局变量。

同样的，在 OC 中对象的属性属于全局变量，也可以在 `Blocks` 常量表达式中直接修改。

_疑问 3：`Blocks`表达式中的全局变量为什么可以修改？`Blocks`持有该全局变量了嘛？_

#### 3、`Blocks` 底层实现

`Blocks` 是持有自动变量的匿名函数，允许访问其作用域之外的变量。那么，`Blocks` 到底是什么呢？利用 clang 将 Objective-C 代码转为 C ++ 源码：


```
clang -rewrite-objc main.m

//支持 ARC 并指定 Runtime 版本
clang -rewrite-objc -fobjc-arc -fobjc-runtime=ios-8.0.0 ModelClass.m

//支持 ARC 并指定 Runtime 版本
xcrun -sdk iphonesimulator clang -arch arm64 -rewrite-objc -fobjc-arc -fobjc-runtime=ios-8.0.0 main.m

//指定 sdk 和模拟器 并 指定 sdk 和真机
xcrun -sdk iphoneos clang -rewrite-objc main.m

//指定 sdk 和模拟器
xcrun -sdk iphonesimulator clang -rewrite-objc main.m

//指定具体某版本的模拟器
xcrun -sdk iphonesimulator9.3 clang -rewrite-objc main.m

//如果使用了第三SDK
xcrun -sdk iphonesimulator9.3 clang -rewrite-objc –F      /Users/goanywhere/Downloads/nbs-newlens-ios-2.3.6  main.m
```

##### 3.1、分析`Blocks` 的本质

> 变量有地址，同样的，函数也有地址，指向函数的指针中存储着函数代码的起始处地址。

Objective-C 代码：

```
int main(int argc, const char * argv[]) {
    int (^addBlock)(int,int) = ^(int a ,int b){
        return a + b;
    };
    printf("1 + 2 = %d \n",addBlock(1,2));
    return 0;
}
```

转换后相关的 C++ 源码：

```
/** Block 结构模版
 * @param isa 指向所属类的指针，
 * @param FuncPtr 函数指针，指向Block的功能实现函数
 */
struct __block_impl {
    void *isa;
    int Flags;
    int Reserved;
    void *FuncPtr;
};

/** Block的描述信息
 * @note Block_size 赋值 sizeof(struct __main_block_impl_0)
 *           意味着Block可能就是结构 __main_block_impl_0
 */
static struct __main_block_desc_0 {
    size_t reserved;
    size_t Block_size;//表明 Block 所占内存空间的大小
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};

/// 嵌套 __block_impl 与 __main_block_desc_0
struct __main_block_impl_0 {
    struct __block_impl impl;
    struct __main_block_desc_0* Desc;
    /** 结构体的构造函数
     * @param fp 函数指针，被赋值 __main_block_func_0()
     * @param desc 结构指针，指向结构 __main_block_desc_0
     */
    __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int flags=0) {
        impl.isa = &_NSConcreteStackBlock;//isa 指针，栈内存
        impl.Flags = flags;//标志位，0
        impl.FuncPtr = fp;//函数指针，指向 Block 的功能实现函数
        Desc = desc;//描述信息：block 内存
    }
};

/** 实现 OC 代码中Block的主要功能。
 * @param __cself 结构指针，类似于 OC 中的 self 指针；指向模板 (__block_impl *)addBlock；
 *                意味着 Block 主体是一个__main_block_impl_0的结构；
 * @param int a、int b ： Block 表达式的参数列表
 * @return 返回 int 类型
 */
static int __main_block_func_0(struct __main_block_impl_0 *__cself, int a, int b) {
    return a + b;
}

/** main() 函数的实现，分为 3 大部分：声明、实现、调用
 * 1、Block 声明：本质为函数指针 int(*addBlock)(int,int)，指向构造函数 __main_block_impl_0() 的起始位置
 * 2、Block 常量表达式：获取函数地址 &__main_block_impl_0()
 *     2.1、强制类型转换 ((int (*)(int, int))
 *     2.2、构造函数 __main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA))
 *     2.3、函数指针指向构造函数
 * 3、Block 的调用转为 ((int (*)(__block_impl *, int, int))((__block_impl *)addBlock)->FuncPtr)((__block_impl *)addBlock, 1, 2)
 *     3.1、函数 ((int (*)(__block_impl *, int, int))((__block_impl *)addBlock)->FuncPtr) 就是 __main_block_func_0()
 *     3.2、参数列表 ((__block_impl *)addBlock, 1, 2)
 *     3.3、__cself 指针指向模板类 (__block_impl *)addBlock
 *     3.4、调用 addBlock(1,2) 也就是调用函数指针 __block_impl.FuncPtr 指向的函数__main_block_func_0()
 */
int main(int argc, char * argv[]) {
    int (*addBlock)(int,int) = ((int (*)(int, int))&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA));
    printf("1 + 2 = %d \n",((int (*)(__block_impl *, int, int))((__block_impl *)addBlock)->FuncPtr)((__block_impl *)addBlock, 1, 2));
    return 0;
}
```


_疑问 4：在构造函数`__main_block_impl_0()`中，`isa`指针所指向的 `_NSConcreteStackBlock` 是什么？`isa`指针为何指向`_NSConcreteStackBlock`？_


__小结：__
* `Blocks` 本质是结构`__block_impl `，其成员`isa`指针表明在内存中的位置。
* `Blocks` 声明本质声明了一个特定类型的函数指针。
* `Blocks` 常量表达式本质上是一个指向 `__main_block_impl_0` 的结构指针。
* `Blocks` 调用本质是通过函数指针`__main_block_impl_0.FuncPtr` 调用特定函数。


##### 3.2、`Blocks`如何持有其作用域之外的自动变量？

> `Blocks` 与函数、方法类似，但除了是可执行代码外，`Blocks` 还持有与堆内存或栈内存绑定的变量！

```
int main(int argc, const char * argv[]) {
    int a = 3;///基本数据类型的自动变量
    NSString *string = @"Hello word!!";///OC 类：自动变量
    __weak NSString *weakString = @"弱引用";
    void (^simpleBlock)(void) = ^{
        NSLog(@"string1 ==== %@ : %p",string,string);
        NSLog(@"weakString = %@ : %p",weakString,weakString);
        printf("a1 ========= %d : %p \n",a,&a);
    };
    a = 4;
    string = @"打印日志";
    simpleBlock();
    return 0;
}
```

转为 C++ 代码：

```
/* 将一个对象的值复制给另一个对象
 * @param dst 目的对象：接收值
 * @param src 源对象：
 * @note  BLOCK_FIELD_IS_OBJECT 代表Objective-C对象，此处相当于调用 -retain 方法
 */
static void __main_block_copy_0(struct __main_block_impl_0*dst, struct __main_block_impl_0*src) {
    _Block_object_assign((void*)&dst->string, (void*)src->string, BLOCK_FIELD_IS_OBJECT);
    _Block_object_assign((void*)&dst->weakString, (void*)src->weakString, BLOCK_FIELD_IS_OBJECT);
}

/* 销毁一个对象
*/
static void __main_block_dispose_0(struct __main_block_impl_0*src) {
    _Block_object_dispose((void*)src->string, BLOCK_FIELD_IS_OBJECT);//相当于调用 -release 方法
    _Block_object_dispose((void*)src->weakString, BLOCK_FIELD_IS_OBJECT);
}

/* Block的描述信息
 * @note 持有 复制函数 与 释放函数 的函数指针
 */
static struct __main_block_desc_0 {
    size_t reserved;
    size_t Block_size;//表明 Block 所占内存空间的大小
    void (*copy)(struct __main_block_impl_0*, struct __main_block_impl_0*);//复制函数
    void (*dispose)(struct __main_block_impl_0*);//销毁函数
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0), __main_block_copy_0, __main_block_dispose_0};

struct __main_block_impl_0 {
    struct __block_impl impl;
    struct __main_block_desc_0* Desc;
    NSString *__strong string; // Blocks 持有作用域之外的自动变量
    NSString *__weak weakString;
    int a;
    
    /* 结构体的构造函数
     * @param fp 函数指针，被赋值 __main_block_func_0()
     * @param desc 结构指针，指向结构 __main_block_desc_0
     * @param 作用域之外的 _string、_weakString、_a 被赋值给该结构体成员
     */
    __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, NSString *__strong _string, NSString *__weak _weakString, int _a, int flags=0) : string(_string), weakString(_weakString), a(_a) {
        impl.isa = &_NSConcreteStackBlock;//isa 指针，栈内存
        impl.Flags = flags;//标志位，0
        impl.FuncPtr = fp;//函数指针，指向 Block 的功能实现函数
        Desc = desc;//描述信息：block 内存
    }
};

/* 实现 OC 代码中Block的主要功能
 * @note __cself->string 调用的是Block对象的成员 string ，并不是作用域之外的 string 变量
 */
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
    NSString *__strong string = __cself->string; // 复制的副本
    NSString *__weak weakString = __cself->weakString; // 复制的副本
    int a = __cself->a; // 复制的副本
    NSLog((NSString *)&__NSConstantStringImpl__var_folders_d8_yqx6_fxx1gzbwsghfv7n6r8w0000gn_T_main_490ccf_mi_2,string,string);
    NSLog((NSString *)&__NSConstantStringImpl__var_folders_d8_yqx6_fxx1gzbwsghfv7n6r8w0000gn_T_main_490ccf_mi_3,weakString,weakString);
    printf("a1 ========= %d : %p \n",a,&a);
}

int main(int argc, const char * argv[]) {
    int a = 3;
    NSString *string = (NSString *)&__NSConstantStringImpl__var_folders_d8_yqx6_fxx1gzbwsghfv7n6r8w0000gn_T_main_490ccf_mi_0;
    __attribute__((objc_ownership(weak))) NSString *weakString = (NSString *)&__NSConstantStringImpl__var_folders_d8_yqx6_fxx1gzbwsghfv7n6r8w0000gn_T_main_490ccf_mi_1;
    void (*simpleBlock)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, string, weakString, a, 570425344));
    a = 4;
    string = (NSString *)&__NSConstantStringImpl__var_folders_d8_yqx6_fxx1gzbwsghfv7n6r8w0000gn_T_main_490ccf_mi_4;
   ((void (*)(__block_impl *))((__block_impl *)simpleBlock)->FuncPtr)((__block_impl *)simpleBlock);
    return 0;
}
```

`Blocks` 持有其作用域之外的自动变量的大致过程：
* 1、`Blocks` 访问其作用域之外的自动变量时，将这些变量添加为实例`__main_block_impl_0`的结构成员并赋值；
* 2、当`Blocks`表达式从栈内存复制到堆内存或者全局静态区时，调用 `_Block_object_assign()` 函数将持有的自动变量也复制到堆内存或者全局静态区；
    Objective-C类对象浅拷贝其内存地址，基础数据类型拷贝其数据值；
* 3、功能实现函数`__main_block_func_0()`使用的变量`string`是实例`__main_block_impl_0`的结构成员`string`，而不是`Blocks`作用域之外的自动变量`string`;


###### 3.2.1、 函数`__main_block_copy_0()`与`__main_block_dispose_0()`的调用时机


函数`__main_block_copy_0()`的调用时机：
* 当`Blocks`变量调用`_Block_copy()` 函数 或 `-copy`方法时；
* 赋值操作，将 `Blocks` 表达式从栈内存复制到堆内存、或者全局静态区时；
 
 
 堆上的 `Block` 被废弃时，会调用`__main_block_dispose_0()`函数！


当`Block`从栈复制到堆上时，`Block`实例通过`_Block_object_assign()`函数，将持有的自动变量的引用计数加 1！
当`Block`从堆释放时，`Block`实例通过`_Block_object_dispose()`函数，释放持有自动变量的所有权！


##### 3.3、`__Block` 发挥什么作用？

既然 `Blocks` 持有作用域之外的自动变量，为何使用`__Block`修饰之后，修改`Blocks` 持有的自动变量，会对作用域之外的变量产生影响呢？

```
int main(int argc, const char * argv[]) {
    __block int a = 3;
    void (^simpleBlock)(void) = ^{
        printf("a 1==== %d \n",a);
        a = 4;
        printf("a 2==== %d \n",a);
    };
    a = 5;
    simpleBlock();
    return 0;
}
```

转为C++代码：

```
/** 将一个对象的值复制给另一个对象
* @param dst 目的对象：接收值
* @param src 源对象：
*/
static void __main_block_copy_0(struct __main_block_impl_0*dst, struct __main_block_impl_0*src) {
    _Block_object_assign((void*)&dst->a, (void*)src->a, BLOCK_FIELD_IS_BYREF);
}

/** 销毁一个对象
*/
static void __main_block_dispose_0(struct __main_block_impl_0*src) {
    _Block_object_dispose((void*)src->a, BLOCK_FIELD_IS_BYREF);
}

/** Block的描述信息
 */
static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;///表明 Block 所占内存空间的大小
  void (*copy)(struct __main_block_impl_0*, struct __main_block_impl_0*);///复制函数
  void (*dispose)(struct __main_block_impl_0*);///销毁函数
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0), __main_block_copy_0, __main_block_dispose_0};

/** __Block 变量转为结构体
 * @note __forwarding 指针，指向外部变量指针的内存地址
 */
struct __Block_byref_a_0 {
    void *__isa;//指向所属的类的 isa 指针
    __Block_byref_a_0 *__forwarding;
    int __flags;
    int __size;//该结构的内存大小
    int a;//该结构持有相当于原自动变量
};

struct __main_block_impl_0 {
    struct __block_impl impl;
    struct __main_block_desc_0* Desc;
    __Block_byref_a_0 *a; // by ref
    
    /** 结构体的构造函数
     * @param fp 函数指针，被赋值 __main_block_func_0()
     * @param desc 结构指针，指向结构 __main_block_desc_0
     * @param 作用域之外的 _a 被拷贝给该结构体成员
     */
    __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, __Block_byref_a_0 *_a, int flags=0) : a(_a->__forwarding) {
        impl.isa = &_NSConcreteStackBlock;//isa 栈内存
        impl.Flags = flags;
        impl.FuncPtr = fp;
        Desc = desc;
    }
};

/* 实现 OC 代码中Block的主要功能
 * @note __cself->a 调用的是Block对象的成员 a ，并不是作用域之外的变量 a
 *       a->__forwarding 是作用域之外的变量 a 的指针地址
 *       a->__forwarding->a 作用域之外的变量 a 的成员
 *       (a->__forwarding->a) = 4 本质上修改的还是作用域之外的变量值
 */
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
    __Block_byref_a_0 *a = __cself->a; // bound by ref
    printf("a 1==== %d \n",(a->__forwarding->a));
    (a->__forwarding->a) = 4;//修改的是作用域之外的变量值
    printf("a 2==== %d \n",(a->__forwarding->a));
}

/* mian() 函数
 * __block a 转为结构体 __Block_byref_a_0
 * a=3 被赋值给结构成员 __Block_byref_a_0.a
 * __forwarding 指针指向变量a的内存地址
 */
int main(int argc, const char * argv[]) {
    __attribute__((__blocks__(byref))) __Block_byref_a_0 a = {
        (void*)0,// __isa 指针:指向空指针NULL
        (__Block_byref_a_0 *)&a,// __forwarding 指针
        0,
        sizeof(__Block_byref_a_0),//__size 内存大小
        3
    };
    void (*simpleBlock)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, (__Block_byref_a_0 *)&a, 570425344));
    (a.__forwarding->a) = 5;
    ((void (*)(__block_impl *))((__block_impl *)simpleBlock)->FuncPtr)((__block_impl *)simpleBlock);
    return 0;
}
```

`__block` 发挥的作用：

* 1、`__block` 类似于强制转换，将修饰的变量转换为结构 `__Block_byref_a_0`；
    结构成员`__forwarding`指向`__block`变量的指针地址。
* 2、在功能函数 `__main_block_func_0()` 修改变量值: `(a->__forwarding->a)=4`，本质上修改的还是作用域之外的`__block`变量；

##### 3.4、`Blocks` 表达式访问的全局变量

`Blocks`对象可以持有作用域之外的自动变量；但并不会持有全局变量！

```
int a = 3;//全局变量
int main(int argc, const char * argv[]) {
    void (^simpleBlock)(void) = ^{
    static int b = 13;//静态变量
    void (^simpleBlock)(void) = ^{
        a = 4;
        b = 14;
    };
    a = 5;
    b = 15;
    simpleBlock();
    return 0;
}
```

转为 C++ 代码：

```
int a = 3;
struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  int *b;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int *_b, int flags=0) : b(_b) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};

static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
    int *b = __cself->b; // bound by copy
    a = 4;
    (*b) = 14;
}

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};

int main(int argc, const char * argv[]) {
    static int b = 13;
    void (*simpleBlock)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, &b));
    a = 5;
    b = 15;
    ((void (*)(__block_impl *))((__block_impl *)simpleBlock)->FuncPtr)((__block_impl *)simpleBlock);
    return 0;
}
```

执行函数`__main_block_func_0()` 在全局变量`a` 的作用域之内，可以直接使用全局变量`a`，不需要复制。
但是具有块作用域的静态变量`b`，在函数`__main_block_func_0()`的作用域之外，不能直接访问；将静态变量的地址保存在结构`__main_block_impl_0`，通过地址访问作用域之外的静态变量！


#### 4、`Blocks` 内存管理


`Blocks` 常量表达式的 `isa` 指针指向 `_NSConcreteStackBlock`，表明`Blocks`常量被编译器存储到栈区！与之类似的类有：`_NSConcreteGlobalBlock`、`_NSConcreteMallocBlock`。


`Blocks`所属的类|设置对象的存储域
-|-
`_NSConcreteStackBlock`|由编译器自动分配在栈区，并在合适时机释放
`_NSConcreteMallocBlock`|由 `_malloc()`函数分配在堆区
`_NSConcreteGlobalBlock`|全局静态区，存放全局变量


![内存分区](https://upload-images.jianshu.io/upload_images/7112462-a037a4789ffeab76.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


##### 4.1、类实例 `_NSConcreteGlobalBlock` 

`Blocks` 作为全局变量或者类的属性时，创建的`Blocks`对象属于`_NSConcreteGlobalBlock`。

```
//具有文件作用域，由程序静态分配，存储在全局静态区的数据区
static BOOL(^isEvenNumberBlock2)(int number) = ^BOOL(int number){return (number % 2) == 0 ? YES : NO;};
int main(int argc, const char * argv[]) {
   return 0;
}
```

声明并定义全局变量`isEvenNumberBlock2`，转为 C++ 代码：

```
static struct __isEvenNumberBlock2_block_impl_0 {
  struct __block_impl impl;
  struct __isEvenNumberBlock2_block_desc_0* Desc;
  __isEvenNumberBlock2_block_impl_0(void *fp, struct __isEvenNumberBlock2_block_desc_0 *desc, int flags=0) {
    impl.isa = &_NSConcreteGlobalBlock;//isa 初始化为 _NSConcreteGlobalBlock
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
```

全局变量`isEvenNumberBlock2`实例属于`_NSConcreteGlobalBlock`类。

![内存分析](https://upload-images.jianshu.io/upload_images/7112462-18eefafe55a6bfa3.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

即使在块作用域内创建`Blocks`变量，只要`Blocks`不持有作用于之外的自动变量，编译器就会将`Blocks`变量存储在全局静态区。

##### 4.2、`Blocks`变量从栈复制到堆上

由于`Blocks`可以看做 Objective-C 对象，而在 ARC 模式下编译器默认使用`__strong` 修饰变量；因此`simpleBlock`变量持有`Blocks`实例的所有权，类似于MRC下的`-retain` ！因此`simpleBlock`变量存储在堆区。

假如使用 `__weak`修饰`Block`变量，则该变量存储在栈区！

![从栈复制到堆上](https://upload-images.jianshu.io/upload_images/7112462-d0c49c717ea6deec.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

###### 4.2.1、什么情况下栈上的`Block`复制到堆上？

> 调用函数`_Block_copy()`将`Block`变量从栈复制到堆！

都在什么情况下，编译器会将栈上的`Block`变量复制到堆上呢？

* 调用`Block`的实例方法`-copy`时：如果`Block`配置在栈上，那么该`Block`从栈复制到堆上；
* ARC下`Block` 作为函数返回值时；
* ARC下将`Block`表达式赋值给有`__strong`修饰的 `id` 类型或 `Block`类型成员变量时: 编译器自动的将对象的`Block` 作为参数并调用`_Block_copy()` 函数；与`-copy`效果相同！
* 在方法名中含有`usingBlock` 的`Cocoa`方法；
* GCD的API传送的`Block`时；


###### 4.2.2、为什么把栈上的`Block`复制到堆上？

栈上的`Block`变量，如果其作用域结束，该`Block`就会被释放；由于`__Block`变量也配置在栈上，其变量作用域结束，则该`__Block`变量也被释放。

###### 例子：MRC 下`Block`变量作为函数返回值时

 ```
/* @note MRC 编译：Block常量表达式分配在栈内存，不被任何指针持有，当函数返回时，Block变量被释放；
 *           因此编译错误： Returning block that lives on the local stack
 *           调用 -copy 将Block变量从栈复制到堆上，解决该编译错误；
 *           需要在适当时机调用 -release 释放所有权，否则引起内存泄露；
 * @note ARC 编译，Block常量表达式作为函数返回值时会被编译成 autoreleased NSConcreteMallocBlock 类型，存储在堆内存
 *           编译器会自动释放，无需我们操心
 */
typedef  void (^SimpleBlock)(void);
SimpleBlock getBlockFunc(){
    int b = 20;
    return ^{
        NSLog(@"b ====== %d",b);
    };
}
 ```

>注意：将 `Block` 从栈复制到堆上需要消耗CPU资源。如果`Block`在栈上也能使用，则无需复制到堆上！

_疑问6：思考一下，在此处可以使用 `-autorelease` 或者 `-retain`方法嘛？_

###### 4.2.3、持有自动变量

当`Block`从栈上复制到堆上时，作用域之外的自动变量仍然存储在栈上，仅仅对应的`Block`结构成员被复制在堆上。

###### a、`Block`变量调用`-copy`

```
//MRC 下编译代码
int main(int argc, const char * argv[]) {
   //自动变量存储在栈区，地址都是高位地址
    NSString *log = @"hello word";//栈上，高地址
    int a = 5;//栈上，高地址
    
    NSLog(@"log1:%@  : %p : %p",log,log,&log);
    NSLog(@"a  1:%d  : %p",a,&a);
    
    SimpleBlock simpleBlock =  [^{
        NSLog(@"log2:%@  : %p : %p",log,log,&log);//堆上
        NSLog(@"a  2:%d  : %p",a,&a);//堆上
    } copy];
    
    log = @"打印日志";//栈上，高地址
    a = 6;//栈上，高地址
    NSLog(@"log3:%@  : %p : %p",log,log,&log);
    NSLog(@"a  3:%d  : %p",a,&a);
    
    simpleBlock();
    
    NSLog(@"log4:%@  : %p : %p",log,log,&log);//栈上，高地址
    NSLog(@"a  4:%d  : %p",a,&a);//栈上，高地址
    
    return 0;
}

/*打印数据
log1:hello word  : 0x1000021e0 : 0x7ffeefbff578
a  1:5  : 0x7ffeefbff574
log3:打印日志  : 0x100002280 : 0x7ffeefbff578
a  3:6  : 0x7ffeefbff574
log2:hello word  : 0x1000021e0 : 0x100502d10
a  2:5  : 0x100502d18
log4:打印日志  : 0x100002280 : 0x7ffeefbff578
a  4:6  : 0x7ffeefbff574
*/
```

持有的自动变量|`Block` 从栈`copy`到堆时的影响|`Block`从堆区`release`后
-|-|-
基本数据类型如`int`|从栈复制到堆|从堆区释放掉
Objective-C 对象|该对象引用计数加1，堆上的`Block`持有该对象|该对象引用计数减1，堆上的`Block`释放该对象所有权

###### b、对`Block`使用`-retain`、`-autorelease`操作

`Block` 能否调用使用`-autorelease`或者 `-retain`方法？

`Block`调用`-copy`从栈复制到堆时， 底层调用`_Block_object_assign()`函数持有被捕获的对象（自动变量）；从堆区释放时调用`_Block_object_dispose()`释放被捕获对象的所有权！

使用`-retain`或者`-autorelease` 获取`Block`所有权，引用计数`retainCount`加1；但是并没有调用`_Block_object_assign()`函数与`_Block_object_dispose()`，被持有自动变量仍然在栈内存中：
* 对于基本数据类型如`int`等，只要持有`Block`所有权，就可以正常使用这些被持有的基本数据类型；
* 对于Objective-C类对象，捕获操作将变量指针赋值到`Block`结构体实例的结构成员上！对于变量指针的赋值操作，对变量的所有权没有任何影响，对变量的`retainCount`也没有任何影响！
因此，捕获操作并没有捕获 Objective-C 类对象的所有权。

我们通过 `retain` 或者 `autorelease` 操作获取的` Block` 结构体实例的所有权，所以调用`Block`可以确保该对象存在！但是对被捕获的 Objective-C 类对象，`Block`结构体实例没有该变量的所有权，程序运行到此处，变量指针（_`Block` 结构体实例的结构成员_）指向的变量可能已被释放，此时程序可能闪退！！

被捕获的自动变量|`Block` 在栈上`retain`或者`autorelease`
-|-
基本数据类型如`int`|没影响
Objective-C 对象|没影响



如果`Block`没有捕获自动变量，那么它被配置在全局静态区，此时又没有必要再次获取所有权了！

>  获取`Block` 所有权的操作最好使用 `copy` 操作。

###### 被捕获的自动变量释放问题

* 在MRC下，被捕获的自动变量随着 `Block` 使用 `copy` 操作，引用计数加 1 。当我们使用 `release` 释放 `Block` 时，`Block` 会将持有的自动变量引用计数减 1！！
* 在 ARC 下，`Block` 会随着作用域的结束而被释放，此时被 `Block` 捕获的自动变量也随之释放！！



###### 4.2.3、`__block` 变量

通过 `copy` 操作，因为捕获的自动变量是指针，复制操作会使引用计数加 1！！那么 `copy` 将 `Block`  从栈复制到堆的时候，`__block` 变量会产生什么影响呢？？

`__block` 变量的配置存储域|`Block` 从栈复制到堆时的影响
-|-
栈|从栈复制到堆并被`Block` 持有
堆|`Block` 持有

###### a、在一个 `Block`中使用多个`__block` 变量

若在一个 `Block` 中使用 `__block` 变量，则 `Block` 持有该变量，该 `__block` 也必定配置在栈上。将 `Block` 从栈复制到堆上，所捕获的 `__block` 也被从栈上配置到堆上。


![在一个Block中使用__block变量](https://upload-images.jianshu.io/upload_images/7112462-41c346e2eaa82c0a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


__注意__：`__block` 变量若不在 `Block` 中使用，则不会被复制到堆区！`Block` 的复制操作，对不被捕获的`__block` 变量没有任何影响！！





###### b、在多个```Block```中使用某个```__block```变量

* 1、在多个```Block```中使用某个```__block```变量时，最初这些```Block```和```__block```变量都配置在栈上。
* 2、将任何一个```Block```从栈复制到堆上，这个```__block```变量也被从栈上复制到堆上被```Block```持有。
* 3、当剩下的```Block```从栈复制到堆上时，被复制的```Block```持有```__block```变量，并增加```__block```变量的引用计数。

![在多个Block中使用__block变量](https://upload-images.jianshu.io/upload_images/7112462-3ece29c6f33e240f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


######c、```Block```被废弃

就像Objective-C的引用计数管理一样：使用```__block```变量的```Block```持有```__block```。如果```Block```被废弃，它所持有的```__block```变量就被释放。

![Block的废弃和__block变量的释放](https://upload-images.jianshu.io/upload_images/7112462-4652ae519154ce91.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


######d、```__block```变量的```__forwarding ```指针


通过```Block``` 的复制，```__block```变量也从栈上复制到堆上，此时可同时访问栈上和堆上的```__block```变量！那么栈上的```__block```变量的```__forwarding ```指针指向何处？？堆上的```__block```变量的```__forwarding ```指针又指向何处？？




将栈上的```__block```变量复制到堆上，栈上的```__block```变量```__forwarding ```指针将指向被复制到堆上的```__block```变量； 堆上```__block```变量的```__forwarding ```指针指向其自身！！


![__block变量从栈复制到堆上](https://upload-images.jianshu.io/upload_images/7112462-90feb7b1c32e6a71.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


通过```__block```变量的```__forwarding ```指针，无论在```Block```表达式之中、在```Block```表达式之外，无论在栈上、还是复制到堆上。都能保证程序始终访问同一个```__block```变量！


######e 、```_Block_object_assign()```函数与```_Block_object_dispose()```函数


在```__block```变量有```__strong ```修饰时：当```__block```变量从栈复制到堆时，使用```_Block_object_assign()```函数持有赋值给```__block```变量的对象！当堆上的```__block```变量被废弃时，使用```_Block_object_dispose()```函数释放赋值给```__block```变量的对象。


被捕获的对象|```BLOCK_FIELD_IS_OBJECT```
-|-
```__block```变量|```BLOCK_FIELD_IS_BYREF```


#### 5、 `Blocks` 的循环引用问题

![Block循环引用](https://upload-images.jianshu.io/upload_images/7112462-53069a4da1f3f2a2.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


##### 5.1、 `Blocks` 典型的循环引用

如下代码，造成循环引用：

```
{
    ModelClass *model = [[ModelClass alloc] init];
    model.requestBlcok = ^(BOOL isSuccess) {
        NSLog(@"model ==== %@",model);
    };
    model.requestBlcok(YES);
}
```

转为 C++ 源码，只看关键部分：

```
struct __main_block_impl_0 {
    struct __block_impl impl;
    struct __main_block_desc_0* Desc;
    ModelClass *__strong model;
    __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, ModelClass *__strong _model, int flags=0) : model(_model) {
        impl.isa = &_NSConcreteStackBlock;
        impl.Flags = flags;
        impl.FuncPtr = fp;
        Desc = desc;
    }
};
```
 

结构体 `__main_block_impl_0` （即变量 `Block` ）持有 `model`；而 `model` 又持有该 `Block` ；因为双方互相持有，造成循环引用，导致内存无法释放！



##### 5.2、 使用 `__weak` 解决循环引用？


如下代码，在 `Block` 表达式中使用 `__weak` 避免循环引用

```
{
    ModelClass *model = [[ModelClass alloc] init];
    __weak typeof(model) weakModel = model;
    model.requestBlcok = ^(BOOL isSuccess) {
        NSLog(@"weakModel ==== %@",weakModel);
    };
    model.requestBlcok(YES);}
```

转为 C++ 源码，只看关键部分：


```
struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  ModelClass *__weak weakModel;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, ModelClass *__weak _weakModel, int flags=0) : weakModel(_weakModel) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
```

结构体 `__main_block_impl_0` 通过 `__weak` 弱引用 `model`；虽然 `model` 持有该结构体，但不会造成循环引用！

注意：由于是  `__weak` 修饰的弱引用， `__main_block_impl_0` 无法保证 弱引用对象的释放时机，所以有可能 _在使用弱引用对象时该变量已经被释放，`__weak` 指针指向 `nil`_ ！


##### 5.3、  `__weak`  与 `__strong` 成对使用， 解决循环引用！


`Block` 表达式外面声明弱引用  `__weak` ，表达式中使用`__strong`再对弱引用 强引用：

```
{
    ModelClass *model = [[ModelClass alloc] init];
    __weak typeof(model) weakModel = model;
    model.requestBlcok = ^(BOOL isSuccess) {
        __strong typeof(weakModel) strongModel = weakModel;
        NSLog(@"strongModel ==== %@",strongModel);
    };
    model.requestBlcok(YES);
}
```

疑问：使用了 `__strong` 对 `model` 强引用，这不是和当初一样，再次造成了循环引用？ 


转为 C++ 源码，分析关键部分：

```
struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  ModelClass *__weak weakModel;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, ModelClass *__weak _weakModel, int flags=0) : weakModel(_weakModel) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};

static void __main_block_func_0(struct __main_block_impl_0 *__cself, BOOL isSuccess) {
    ModelClass *__weak weakModel = __cself->weakModel; // bound by copy
    __attribute__((objc_ownership(strong))) typeof(weakModel) strongModel = weakModel;
    NSLog((NSString *)&__NSConstantStringImpl__var_folders_s5_127zrd4j7kl4llwhcl4cz3_40000gn_T_main_ecfcbd_mi_0,strongModel);
}
```

`strongModel` 在 `block` 内部 （函数 `__main_block_func_0()` 中） 仅仅作为自动变量使用，而不是结构体 `__main_block_impl_0` 的结构成员，因此不存在  `block` 变量持有  `model`，所以也就不会导致循环引用！
同时又通过强引用，保证了对 `model` 的所有权，不会导致使用时 `model` 被释放的情况发生！



##### 5.4、 为何 `Masonry` 不会导致循环引用？

在  `Masonry`  的 `Block` 表达式中，使用它的持有者，为何不会导致循环引用呢？

```
{
    [self.aView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.left.equalTo(self.view);
    }];
}
```

不妨去探寻下  `Masonry` 的关键代码：

```
@implementation UIView (MASAdditions)

/// block 作为方法的实参，属于自动变量，不被任何对象持有
/// 因此即使在该 block 中使用 self.view ，也不会导致循环引用！
- (NSArray *)mas_makeConstraints:(void(^)(MASConstraintMaker *))block {
    self.translatesAutoresizingMaskIntoConstraints = NO;
    MASConstraintMaker *constraintMaker = [[MASConstraintMaker alloc] initWithView:self];
    block(constraintMaker);
    return [constraintMaker install];
}

@end


@interface MASConstraintMaker : NSObject
/// 对 view 弱引用，也不会导致循环引用
@property (nonatomic, weak) UIView *view;
@property (nonatomic, strong) NSMutableArray *constraints;
@end
@implementation MASConstraintMaker

- (id)initWithView:(UIView *)view {
    self = [super init];
    if (!self) return nil;
    self.view = view;
    self.constraints = NSMutableArray.new;
    return self;
}
@end
```


* 1、 `Masonry`  的 `Block` 变量，仅仅作为方法参数，是一个自动变量、存储在栈区，不被任何对象持有！
因此即使在该 `block` 中使用 `self.view` ，也不会导致循环引用！
* 2、`MASConstraintMaker` 实例虽然引用了 `self.view` ，却是弱引用，也不会导致循环引用！


#### 6、 `Blocks` 面试题

* `Blocks` 被捕获的自动变量
*  `Blocks` 的本质、 `__block ` 变量的本质
*  `Blocks` 内存问题：什么时候配置在堆区？什么时候配置在栈区？什么时候配置在全局区！
*  `Blocks` 为何要使用  `-copy ` 操作？可不可以使用  `-retain ` 操作、 `-autorelease ` 操作
*  `Blocks`使用  `-copy ` 操作之后要不要  `-release ` ？被捕获的自动变量引用计数有何变化？被捕获的自动变量需不需要释放？
*  `Blocks`使用  `-copy ` 操作 对  `__block ` 变量 的影响？
*   `__block ` 变量赋值后， `__forwarding ` 指针的指向问题？

----

参考文章/图书

[C/C++语言的闭包](https://coolshell.cn/articles/8309.html)
[精通Objective-C]()
[Objective-C高级编程 iOS与OS X多线程和内存管理]()

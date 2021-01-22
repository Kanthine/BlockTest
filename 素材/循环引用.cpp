/// 由编译器自动分配在栈区，并在合适时机释放
__OBJC_RW_DLLIMPORT void *_NSConcreteStackBlock[32];
/// 由 _malloc() 函数分配在堆区；使用 -copy 从栈区复制到堆区；
__OBJC_RW_DLLIMPORT void *_NSConcreteMallocBlock[32];
/// 全局静态区，存放全局变量
__OBJC_RW_DLLIMPORT void *_NSConcreteGlobalBlock[32];



/// 函数 _Block_object_assign() 和 _Block_object_dispose() 的枚举值
enum {
    BLOCK_FIELD_IS_OBJECT   =  3,  // id, NSObject, __attribute__((NSObject)), block, ...
    BLOCK_FIELD_IS_BLOCK    =  7,  // block 变量
    BLOCK_FIELD_IS_BYREF    =  8,  // the on stack structure holding the __block variable
    BLOCK_FIELD_IS_WEAK     = 16,  // declared __weak, only used in byref copy helpers
    BLOCK_BYREF_CALLER      = 128, // called from __block (byref) copy/dispose support routines.
};

enum {
    BLOCK_ALL_COPY_DISPOSE_FLAGS =
        BLOCK_FIELD_IS_OBJECT | BLOCK_FIELD_IS_BLOCK | BLOCK_FIELD_IS_BYREF |
        BLOCK_FIELD_IS_WEAK | BLOCK_BYREF_CALLER
};

/** 将一个对象的值复制给另一个对象
 * @param destAddr 目的对象：接收值
 * @param object 源对象
 * @param flags 存储方案
 */
BLOCK_EXPORT void _Block_object_assign(void *destAddr, const void *object, const int flags);

/** 释放一个对象的所有权
 */
BLOCK_EXPORT void _Block_object_dispose(const void *object, const int flags);


#pragma mark - 循环引用

/// ***********************************************************
/// Objective-C 源代码
/// ***********************************************************

{
    ModelClass *model = [[ModelClass alloc] init];
    model.requestBlcok = ^(BOOL isSuccess) {
        NSLog(@"model ==== %@",model);
    };
    model.requestBlcok(YES);
}

/// ***********************************************************
/// 转 C++ 代码
/// ***********************************************************

struct __block_impl {
  void *isa;
  int Flags;
  int Reserved;
  void *FuncPtr;
};

/// 结构体 __main_block_impl_0 持有变量 model；而 model 又持有该结构体
/// 因为双方互相持有，造成循环引用，导致内存无法释放；
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

static void __main_block_func_0(struct __main_block_impl_0 *__cself, BOOL isSuccess) {
    ModelClass *__strong model = __cself->model; // bound by copy
    NSLog((NSString *)&__NSConstantStringImpl__var_folders_s5_127zrd4j7kl4llwhcl4cz3_40000gn_T_main_74692e_mi_0,model);
}

static void __main_block_copy_0(struct __main_block_impl_0*dst, struct __main_block_impl_0*src) {
    _Block_object_assign((void*)&dst->model, (void*)src->model, 3/*BLOCK_FIELD_IS_OBJECT*/);
}

static void __main_block_dispose_0(struct __main_block_impl_0*src) {
    _Block_object_dispose((void*)src->model, 3/*BLOCK_FIELD_IS_OBJECT*/);
}

static struct __main_block_desc_0 {
    size_t reserved;
    size_t Block_size;
    void (*copy)(struct __main_block_impl_0*, struct __main_block_impl_0*);
    void (*dispose)(struct __main_block_impl_0*);
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0), __main_block_copy_0, __main_block_dispose_0};

int main(int argc, char * argv[]) {
    NSString * appDelegateClassName;
    /* @autoreleasepool */ { __AtAutoreleasePool __autoreleasepool; 

        ModelClass *model = ((ModelClass *(*)(id, SEL))(void *)objc_msgSend)((id)((ModelClass *(*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("ModelClass"), sel_registerName("alloc")), sel_registerName("init"));
        ((void (*)(id, SEL, void (^ _Nonnull)(BOOL)))(void *)objc_msgSend)((id)model, sel_registerName("setRequestBlcok:"), ((void (*)(BOOL))&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, model, 570425344)));
        ((void (*(*)(id, SEL))(BOOL))(void *)objc_msgSend)((id)model, sel_registerName("requestBlcok"))(((bool)1));

        appDelegateClassName = NSStringFromClass(((Class (*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("AppDelegate"), sel_registerName("class")));
    }
    return UIApplicationMain(argc, argv, __null, appDelegateClassName);
}
static struct IMAGE_INFO { unsigned version; unsigned flag; } _OBJC_IMAGE_INFO = { 0, 2 };





#pragma mark - 循环引用 (使用 __weak 修饰)

/// ***********************************************************
/// Objective-C 源代码
/// ***********************************************************

{
    ModelClass *model = [[ModelClass alloc] init];
    __weak typeof(model) weakModel = model;
    model.requestBlcok = ^(BOOL isSuccess) {
        NSLog(@"weakModel ==== %@",weakModel);
    };
    model.requestBlcok(YES);
}

/// ***********************************************************
/// 转 C++ 代码
/// ***********************************************************

struct __block_impl {
  void *isa;
  int Flags;
  int Reserved;
  void *FuncPtr;
};

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
  void (*copy)(struct __main_block_impl_0*, struct __main_block_impl_0*);
  void (*dispose)(struct __main_block_impl_0*);
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0), __main_block_copy_0, __main_block_dispose_0};

/// 结构体 __main_block_impl_0 通过 __weak 弱引用 model；虽然 model 持有该结构体，但不会造成循环引用！
/// 由于是 __weak 修饰的弱引用， __main_block_impl_0 无法保证 弱引用对象的释放时机，所以有可能在使用它时，该变量已经被释放！
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

/// ((void (*)(BOOL))&__main_block_impl_0(  (void *)__main_block_func_0, &__main_block_desc_0_DATA, weakModel, 570425344    ))

static void __main_block_func_0(struct __main_block_impl_0 *__cself, BOOL isSuccess) {
    ModelClass *__weak weakModel = __cself->weakModel; // bound by copy
    NSLog((NSString *)&__NSConstantStringImpl__var_folders_s5_127zrd4j7kl4llwhcl4cz3_40000gn_T_main_7ea924_mi_0,weakModel);
}

static void __main_block_copy_0(struct __main_block_impl_0*dst, struct __main_block_impl_0*src) {
    _Block_object_assign((void*)&dst->weakModel, (void*)src->weakModel, 3/*BLOCK_FIELD_IS_OBJECT*/);
}

static void __main_block_dispose_0(struct __main_block_impl_0*src) {
    _Block_object_dispose((void*)src->weakModel, 3/*BLOCK_FIELD_IS_OBJECT*/);
}

int main(int argc, char * argv[]) {
    NSString * appDelegateClassName;
    /* @autoreleasepool */ { __AtAutoreleasePool __autoreleasepool;


        ModelClass *model = ((ModelClass *(*)(id, SEL))(void *)objc_msgSend)((id)((ModelClass *(*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("ModelClass"), sel_registerName("alloc")), sel_registerName("init"));
        __attribute__((objc_ownership(weak))) typeof(model) weakModel = model;
        ((void (*)(id, SEL, void (^ _Nonnull)(BOOL)))(void *)objc_msgSend)((id)model, sel_registerName("setRequestBlcok:"), ((void (*)(BOOL))&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, weakModel, 570425344)));
        ((void (*(*)(id, SEL))(BOOL))(void *)objc_msgSend)((id)model, sel_registerName("requestBlcok"))(((bool)1));

        appDelegateClassName = NSStringFromClass(((Class (*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("AppDelegate"), sel_registerName("class")));
    }
    return UIApplicationMain(argc, argv, __null, appDelegateClassName);
}
static struct IMAGE_INFO { unsigned version; unsigned flag; } _OBJC_IMAGE_INFO = { 0, 2 };




#pragma mark - 循环引用 (使用 __weak 与 __strong 修饰)


/// ***********************************************************
/// Objective-C 源代码
/// ***********************************************************
{
    ModelClass *model = [[ModelClass alloc] init];
    __weak typeof(model) weakModel = model;
    model.requestBlcok = ^(BOOL isSuccess) {
        __strong typeof(weakModel) strongModel = weakModel;
        NSLog(@"strongModel ==== %@",strongModel);
    };
    model.requestBlcok(YES);
}


/// ***********************************************************
/// 转 C++ 代码
/// ***********************************************************

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

/// strongModel 在 block 内部仅仅作为自动变量使用，而不是结构体 __main_block_impl_0 的结构成员
/// 一旦 block 执行结束block 中的自动变量就会被释放，所以 strongModel 不会引起循环引用
static void __main_block_func_0(struct __main_block_impl_0 *__cself, BOOL isSuccess) {
    ModelClass *__weak weakModel = __cself->weakModel; // bound by copy
    __attribute__((objc_ownership(strong))) typeof(weakModel) strongModel = weakModel;
    NSLog((NSString *)&__NSConstantStringImpl__var_folders_s5_127zrd4j7kl4llwhcl4cz3_40000gn_T_main_ecfcbd_mi_0,strongModel);
}

static void __main_block_copy_0(struct __main_block_impl_0*dst, struct __main_block_impl_0*src) {
    _Block_object_assign((void*)&dst->weakModel, (void*)src->weakModel, 3/*BLOCK_FIELD_IS_OBJECT*/);
}

static void __main_block_dispose_0(struct __main_block_impl_0*src) {
    _Block_object_dispose((void*)src->weakModel, 3/*BLOCK_FIELD_IS_OBJECT*/);
}

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
  void (*copy)(struct __main_block_impl_0*, struct __main_block_impl_0*);
  void (*dispose)(struct __main_block_impl_0*);
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0), __main_block_copy_0, __main_block_dispose_0};

int main(int argc, char * argv[]) {
    NSString * appDelegateClassName;
    /* @autoreleasepool */ { __AtAutoreleasePool __autoreleasepool;


        ModelClass *model = ((ModelClass *(*)(id, SEL))(void *)objc_msgSend)((id)((ModelClass *(*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("ModelClass"), sel_registerName("alloc")), sel_registerName("init"));
        __attribute__((objc_ownership(weak))) typeof(model) weakModel = model;
        ((void (*)(id, SEL, void (^ _Nonnull)(BOOL)))(void *)objc_msgSend)((id)model, sel_registerName("setRequestBlcok:"), ((void (*)(BOOL))&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, weakModel, 570425344)));
        ((void (*(*)(id, SEL))(BOOL))(void *)objc_msgSend)((id)model, sel_registerName("requestBlcok"))(((bool)1));

        appDelegateClassName = NSStringFromClass(((Class (*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("AppDelegate"), sel_registerName("class")));
    }
    return UIApplicationMain(argc, argv, __null, appDelegateClassName);
}
static struct IMAGE_INFO { unsigned version; unsigned flag; } _OBJC_IMAGE_INFO = { 0, 2 };

#  Quick流程
NSObject+Quick
 - quick_name

UIViewController+Quick（作为主导）
 - 增删改查Quickobject，将有name的SubView加入到QuickObject中
 - quick，在获取对应的kv数据进行块分拆
     - SubView的数据加载, [object quick]
 
 QuickIniter 
  - obj: kvdata: atVC:
 QuickAllocer - 实例化
  - obj: atVC:

 JSON文件（以某一个UIViewController为例）
 {
     Instance:{
         init;[]        // 可以存在一个加载顺序，可选项
         name1:{
             ...
         },
     },
     Alloc:[
         {
             name:
             class:    // 实例化的对象类型
             ...
         }
     ],
 }


[1] UIViewController初始化
[2] 调用push，在UINavigationController的扩展中，[vc quick]

#  Quick可用键值说明

## UIView
* CornerRadius
* BorderWidth
* BorderColor
* BackgroundColor

## UILabel
* Text
* TextColor
* Font
* TextAlignment - [Center,Left,Right]
* NumberOfLines

## UIButton
* Text - {Normal:xx,Selected:xx,Disabled:xx,NormalDisabled:xx,SelectedDisabled:xx}
* TextColor - {Normal:xx,Selected:xx,Disabled:xx,NormalDisabled:xx,SelectedDisabled:xx}
* Image - {Normal:xx,Selected:xx,Disabled:xx,NormalDisabled:xx,SelectedDisabled:xx}

## UIImage
* Image - 名称/[非暗黑模式,暗黑模式]

## UINavigationBar
* Translucent
* BackgroundColor
* BackItemColor

## UINavigationItem
* LeftItem （未完善）
* RightItem （未完善）

## UIViewController
* Title
* NavigationBar
* NavigationBarHidden - 影响super和child
* NavigationItem
* View - 对自身页面的配置
* SubView - 对所有子页面，包括直接还是间接
* bottomBarHidden - 影响child

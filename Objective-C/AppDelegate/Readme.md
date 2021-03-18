# 关于XXappDelegate和XXsceneDelegate的相关使用

## 使用Scene需要注意

- 在info.plist中增加

  ```xml
  <key>UIApplicationSceneManifest</key>
  <dict>
  	<key>UIApplicationSupportsMultipleScenes</key>
  	<false/>
  	
  	<key>UISceneConfigurations</key>
  	<dict>
  		<key>UIWindowSceneSessionRoleApplication</key>
  		<array>
  			<dict>
                  /// 此配置的名称
  				<key>UISceneConfigurationName</key>
  				<string>Default Configuration</string>
                  
                  /// 继承UIResponder<UIWindowSceneDelegate>的文件名称
  				<key>UISceneDelegateClassName</key>
                  <string>SceneDelegate</string>
                  
                  /// 使用哪个故事板文件
  				<key>UISceneStoryboardFile</key>
  				<string>Main</string>
  			</dict>
  		</array>
  	</dict>
  </dict>
  ```

- 设置XXappDelegate中的宏isUsingScene为1

- 修改AppDelegate的继承，使其继承XXappDelegate

- 修改SceneDelegate的继承，使其继承XXsceneDelegate

- 重载函数willConnectToSession，并在函数中初始化Navigation

  ```objective-c
  - (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions API_AVAILABLE(ios(13.0)){
      [self createNavigationWithIntitalViewControllerAtStoryboard:@"Main" bundle:[NSBundle mainBundle]];
  }
  ```

  

## 不使用Scene需要注意

- 修改AppDelegate的继承，使其继承XXappDelegate

- 重载函数didFinishLaunchingWithOptions，并在函数中初始化Navigation

  ```objective-c
  - (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary<UIApplicationLaunchOptionsKey,id> *)launchOptions{
      [self createNavigationWithIntitalViewControllerAtStoryboard:@"Main" bundle:nil];
      return YES;
  }
  ```

  
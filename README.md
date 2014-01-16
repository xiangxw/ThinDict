## 截图
![ThinDict](http://ubuntuone.com/6kIBTINvbaHqFbzzXjnGEM)

## 安装方法
#### ubuntu下（支持的版本为12.04-14.04）：

```
    sudo apt-add-repository ppa:xiangxw5689/thindict
    sudo apt-get update
    sudo apt-get install thindict
```

卸载：`sudo apt-get purge thindict`

#### 其他Linux发行版可以编译安装：

0. 安装依赖库。依赖qt4, qtwebkit以及qxt，在Debian/Ubuntu下安装命令为：

```
    sudo apt-get install libqt4-dev libqtwebkit-dev libqxt-dev
```
    
0. 执行qmake

```
    qmake PREFIX=/usr
```
    
0. 生成翻译文件

```
    lrelease thindict.pro
```
	
0. 编译

```
    make
```
    
0. 获取root权限，并安装，在Debian/Ubuntu下命令为：

```
    sudo make install
```

0. 卸载。进入源代码目录执行：`qmake PREFIX=/usr`，然后获取root权限执行`make uninstall`。在Debian/Ubuntu下命令为：

```
    qmake PREFIX=/usr
    sudo make uninstall
```

_**欢迎其他Linux发行版用户提供打包，并添加到该安装说明中～**_

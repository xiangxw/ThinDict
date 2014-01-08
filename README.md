## 截图
![ThinDict](http://ubuntuone.com/7jRKC15enMSYJf6tqvv8T2)

## 安装方法
#### ubuntu下（支持的版本为12.04-14.04）：

    sudo apt-add-repository ppa:xiangxw5689/thindict
    sudo apt-get update
    sudo apt-get install thindict

卸载：`sudo apt-get purge thindict`

#### 其他Linux发行版可以编译安装：

    sudo apt-get install libqt4-dev libqtwebkit-dev
    qmake PREFIX=/usr
    make
    sudo make install

卸载：进入源代码目录执行：`qmake PREFIX=/usr; sudo make uninstall`

_**欢迎其他Linux发行版用户提供打包，并添加到该安装说明中～**_

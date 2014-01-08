## 截图
![LDict_016](http://git.oschina.net/uploads/images/2014/0108/155408_7fa1df26_11073.png)

## 安装方法
#### ubuntu下（支持的版本为12.04-14.04）：

    sudo apt-add-repository ppa:xiangxw5689/ldict
    sudo apt-get update
    sudo apt-get install ldict

卸载：`sudo apt-get purge ldict`

#### 其他Linux发行版可以编译安装：

    sudo apt-get install libqt4-dev libqtwebkit-dev
    qmake PREFIX=/usr
    make
    sudo make install

卸载：进入源代码目录执行：`qmake PREFIX=/usr; sudo make uninstall`

_**欢迎其他Linux发行版用户提供打包，并添加到该安装说明中～**_

## 提交错误或建议
http://git.oschina.net/xiangxw/LDict/issues

## 文档
http://git.oschina.net/xiangxw/LDict/wikis/home

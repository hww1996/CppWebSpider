# CppWebSpider
只是使用了Linux类库和STL
> 还有以下一些东西需要去做

* 优化算法，特别是CookieJar的算法
* 添加一些cookie的一些功能
* 增加head，put，delete方法
* 增加url编码，常用字符编码转换等
* 为http头编码
* 添加一些错误的处理

***

### 编译
1.makefile编译

	$ make
2.cmake编译（在根目录下）

	$ mkdir build
	$ cd build
	$ cmake ..
	$ make
### 使用
先定义`CppWebSpider::request`对象，然后使用`get`或者`post`方法，返回为`CppWebSpider::response`对象。

可以参考一下`main.cpp`中的代码，已测试通过。

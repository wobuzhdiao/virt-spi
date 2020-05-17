# virt_spi

#### 介绍
在linux下实现一个虚拟的spi控制器驱动，用以学习linux spi子系统提供的spi device driver接口的注册与联系

#### 软件架构
软件架构说明


	在上一章中，我们已经完成了虚拟i2c控制器的模拟，本章我们准备完成一个虚拟spi控制器，该章的
的流程和上一章类似。
本篇文章的目的如下:
1. 当你没有开发板，又想学习spi驱动模型，并想进行linux spi device driver的练习时；
2. 当你完成了spi device driver，在硬件开发板出来之前进行spi device driver的验证时。

本章的内容涉及spi驱动模块的内容，这些内容在之前已经分析过，由兴趣的童鞋可查看。


 virtual spi master的实现分析
     在之前分析spi 驱动时，针对spi控制器而言，即是填充spi master结构体相关的成员变量，针对spi master而言，主要设置spi master支持的通信模式、总线号、芯片选择个数等参数，并设置进行spi通信的接口spi_master->transfer或spi_master->transfer_one_message (新版本已使用transfer_one_message接口替换transfer接口，新版本的内核的spi模块为每一个控制器提供了kthread_worker机制，为每一个spi控制器提供了工作线程，由该工作线程进行spi message的处理，并最终调用transfer_one_message进行一个message的处理)。
	在本虚拟spi控制器的实现中，我们为该虚拟spi控制器关联一个virtual_spi_master_info_t类型的变
量，该变量中的list链表用于管理该虚拟spi控制器上所挂载的虚拟spi device；
	我们为每一个虚拟spi device定义了数据结构virtual_spi_dev_info_t，该结构体包括了芯片选择号、
寄存器地址信息。
	我们设置spi_master的setup接口、transfer_one_messahe接口，分别为
virtual_spi_master_setup、virtual_spi_transfer。其中virtual_spi_master_setup接口中主要判断注册的虚拟spi设备的芯片选择号是否在合理范围中；而接口virtual_spi_transfer则主要处理spi_message上所有的spi_transfer信息（包括读写寄存器）。
	如下即为spi_master、virtual_spi_master_info_t、virtual_spi_dev_info_t以及接口的关联图。我们
实现的虚拟spi控制器驱动也就是完成下图的关联图。
![输入图片说明](https://images.gitee.com/uploads/images/2020/0517/143838_53472834_2318483.png "屏幕截图.png")
	针对virtual controller driver的实现，主要包括如下几个步骤（我们还是借助platform驱动模型实现virtual controller driver）：
	1. 创建一个platform device，用于实现与virtual controller driver对应的platform driver进行匹配；
	2. 创建一个platform driver，在该driver的probe接口中实现如下功能：
		a. 调用spi_alloc_master申请spi_master及其关联的virtual_spi_master_info_t类型的内存；
		b. 调用spi_register_master完成spi master的注册；
		c. 调用spi_new_device，向virtual spi master注册虚拟spi device；

 spi device driver的实现流程说明
	通过以上两步已经实现了virtual spi master、virtual spi device，现在我们就可以实现spi 
device driver，进行spi device与spi  device driver的联系了。实现的流程如下：
	1. 定义spi_driver类型的变量，并实现virtual_spi_dev_match类型变量的设置，并实现struc spi_driver中的probe接口、remove接口；
	2. 在上述1中实现的probe接口中，我们可以实现该spi device对应的字符设备（并实现ioctl，可实现读写寄存器命令），从而应用程序即可通过操作字符设备文件，从而完成与virtual i2c device的通信。

spi device字符设备文件的访问
	当完成以上内容后，我们即可以在应用层中，通过访问字符设备文件，从而完成与virtual spi device的访问。
	
	
	本文主要简要说明了virtual spi controller、virtual spi device的实现，用于进行spi模块的练习
（尤其在身边没有开发板，又想学习linux spi驱动时），至于本章的代码，已经上传至csdn上，链接在回复中（本虚拟spi controller虽然实现了spi 驱动的大致框架，但针对spi 通信模式并没有模拟，这是该虚拟控制器的一个缺点，但起码可以练习spi device驱动了）。

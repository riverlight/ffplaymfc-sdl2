# ffplaymfc-sdl2
使用 SDL 2.0 的 ffplay

----------------------

初始版本 forked from :  https://github.com/leixiaohua1020/ffplaymfc

----------------------

ffplay 官方版本使用的依然是 SDL 1.2 ，在显示上有很多缺陷，无法支持 D-ROI 和 LIIPT 等技术，所以我用 SDL 2.0 重写了它的显示部分

-----------------------

现在除了可以解码显示，还支持把解码的流或者文件保存为 *.264、*.aac 和 *.flv 文件
flv 转换器使用的是 https://github.com/riverlight/H264toFLVConverter 
文件存储目录好像是 d:\\workroom\\testroom， 如果没有的话，请创建该目录

-------------------------
# 20160909 : 支持输出音视频帧的时间戳了
# 20160810 : 把 dll 补齐了
# 20160628 : 现在支持保存 flv 文件了
# 20160624 ：支持保存 h.264和 aac 文件


I've finished task ...
我完成了Path Tracing算法和多线程的应用，以及部分microfacet(最终未成功)
多线程部分使用了openmp库，在render.cpp中加入了一行#pragma omp parallel for，还在cmakelist中添加了部分内容，更新了framebuffer的索引计算方式，使之可并行化
加速效果比较明显，生成一张spp=8，200*200的图像，使用多线程为11s，不使用多线程是67s
最终结果的分别为1024*1024 spp=32和512*512 spp=8的图像

实现了参数插值，Bilnn-Phong反射模型，Texture mapping，Bump mapping，Displacement mapping和提高题中的双线性纹理插值
在本次新的几个函数中，rasterizer.cpp文件中的rasterizer_triangle函数实现了对三角形内个点法向量、颜色、纹理颜色的差值。
main.cpp文件中，phong_frament_shader函数中实现了blinn-phong模型，texture_fragment_shader中实现了纹理贴图，bump_fragment_shader实现了凹凸贴图,displacement_fragment_shader实现了位移贴图。
texture.cpp文件中，getColorBilinear函数中使用双线性插值，实现了用双线性插值进行纹理采样。

所有结果均以方法名存储在image文件夹下，其中双线性纹理插值和其对比使用128*128大小的纹理
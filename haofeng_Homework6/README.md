
I've finished task ...
完成了包围盒求交，BVH查找等基础部分
完成了SAH加速

对比结果，SAH比NAIVE建树时间要长（SAH有时是0有时是1，NAIVE一直是0），但是渲染时间短了，从16s提高到13s
使用了12个bucket，bucket的划分根据长度来均分的，在最长的轴上从最小到最大分成12等分，然后遍历object，用setoff来得到其应该对应的是哪个bucket
之后遍历计算cost，因为Ctrav是一样的，对极值无影响，所以我忽略了它，计算0~i和i+1~11两部分的cost
遍历得到最小的cost情况，并依据这种情况将object分开，继续递归划分子包围盒

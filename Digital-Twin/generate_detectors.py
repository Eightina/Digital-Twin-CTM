from xml.etree.ElementTree import Element, ElementTree

# 创建元素,传入tag
root = Element("additional")
# 根据实验设计，车道按照从右到左
arc_ids = [
    [],
    [9, 1, 10],
    [12, 2, 11],
    [13, 3, 14],
    [16, 4, 15],
    [18, 5, 17],
    [19, 6, 20],
    [22, 7, 21],
    [23, 8, 24],
]


def pretty(e, level=0):
    # 用于格式化xml数据
    if len(e) > 0:
        e.text = "\n" + "\t" * (level + 1)
        for child in e:
            pretty(child, level + 1)
        child.tail = child.tail[:-1]
    e.tail = "\n" + "\t" * level


for edge in range(1, 9):
    for lane in range(3):
        arc_id = arc_ids[edge][lane]
        for det in range(100):
            e = Element("laneAreaDetector")
            # 设置元素的属性和text，get方法能获取属性，set方法能设置属性
            e.set("file", "e2output.xml")
            e.set("freq", "60")
            e.set("friendlyPos", "x")
            e.set("id", "arc_{}_{}".format(str(arc_id), str(det)))
            e.set("lane", "{}_{}".format(edge, lane))
            e.set("length", "11.36")
            e.set("pos", str(0 + det * 11.36))
            root.append(e)
pretty(root)

et = ElementTree(root)


# 为了便于观察，导入tostring方法,
# 它可以看一个元素变成xml以后转换成怎样的字符串


# 元素和元素之间关系问题，可以为一个元素添加子元素
# e2 = Element('Row')
# e3 = Element('Open')
# e3.text = '8.69'
# # 让open作为row的子元素
# e2.append(e3)

# # 然后让Row作为Data的子元素
# e.append(e2)
# 去掉Data的text


# 将元素字符串写入到文件当中去，创建ElementTree
# 使用write方法直接写文件名
et.write("./SUMO_Input/dets.add.xml")

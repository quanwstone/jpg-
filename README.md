demo验证利用MFC进行JPEG图片显示.

首先利用libjpeg将jpeg图片进行解码，然后转换成RBGA模式.

最后利用DC进行绘制.

注意点为DC绘制为32bit.故需要涉及到转换.

# bit operation 核心功能 API 说明

---

### 1.1 数据置位

可以针对uint8_t、uint16_t、uint32_t数据变量某几位置位

~~~
BITS_SET(data, bits)
~~~

### 1.2 数据清零

可以针对uint8_t、uint16_t、uint32_t数据变量某几位清零

~~~
BITS_CLR(data, bits)
~~~

### 1.3 数据翻转

可以针对uint8_t、uint16_t、uint32_t数据变量某几位翻转

~~~
BITS_TGL(data, bits)
~~~

### 1.4 读取数据位

可以针对uint8_t、uint16_t、uint32_t数据变量某几位读取值

~~~
BITS_READ(data, bits)
~~~


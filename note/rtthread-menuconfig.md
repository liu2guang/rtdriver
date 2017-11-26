##1. config条目##

config PKG_USING_xxx(pkg包名)
	变量类型 "提示信息"
	depends on n/宏
	default 默认值
	prompt 提示

if PKG_USING_ELOG

    config PKG_ELOG_PATH
        string
        default "/packages/tools/easylogger"
    
    config PKG_ELOG_VER
        string
        default "2.0.0"

endif

变量类型有:
1. bool: 在menuconfig下可以选择y和n.
2. tristate: 在menuconfig下可以选择y和n和m.
3. string: 字符串
4. hex: 16进制
5. int: 10进制

##2. menu条目##

menu "菜单名"
	config ...
	config ...
endmenu

##3. choice条目, 单选##

choice 
	prompt "提示信息"
	default 默认值

	config ...
	config ...

endchoice

##4. comment条目##

comment "帮助信息"
	

